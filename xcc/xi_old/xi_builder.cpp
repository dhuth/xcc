/*
 * xi_builder.cpp
 *
 *  Created on: Feb 1, 2017
 *      Author: derick
 */

#include <xi_old/xi_builder.hpp>
#include <xi_old/xi_lower.hpp>
#include <xi_old/xi_pass_finalize_types.hpp>
#include <xi_old/xi_pass_typecheck.hpp>

namespace xcc {

xi_builder::xi_builder(translation_unit& tu)
    : base_builder_t(tu),
      _lower_walker(new xi_lower_walker(*this)),
      _the_infered_type(new xi_infered_type()) {
    //TODO: settup builtin type rules
}

xi_const_type* xi_builder::get_const_type(ast_type* type) noexcept {
    if(this->_all_consttypes.find(type) == this->_all_consttypes.end()) {
        auto ct = new xi_const_type(type);
        this->_all_consttypes[type] = ct;
        return ct;
    }
    return this->_all_consttypes[type];
}

xi_array_type* xi_builder::get_array_type(ast_type* eltype, list<ast_expr>* dims) const noexcept {
    return new xi_array_type(eltype, dims);
}

xi_infered_type* xi_builder::get_infered_type() const noexcept {
    return this->_the_infered_type;
}

xi_ref_type* xi_builder::get_ref_type(ast_type* tp) noexcept {
    assert(!tp->is<xi_ref_type>());
    if(this->_all_reftypes.find(tp) == this->_all_reftypes.end()) {
        auto rt = new xi_ref_type(tp);
        this->_all_reftypes[tp] = box(rt);
        return rt;
    }
    return this->_all_reftypes[tp];
}

xi_object_type* xi_builder::get_object_type(xi_type_decl* decl) noexcept {
    if(this->_all_objecttypes.find(decl) == this->_all_objecttypes.end()) {
        auto ot = new xi_object_type(decl);
        this->_all_objecttypes[decl] = ot;
        return ot;
    }
    return this->_all_objecttypes[decl];
}

ast_type* xi_builder::get_declaration_type(ast_decl* decl) noexcept {
    switch(decl->get_tree_type()) {
    case tree_type_id::xi_parameter_decl:           return decl->as<xi_parameter_decl>()->type;
    case tree_type_id::xi_function_decl:
        {
            xi_function_decl*       fdecl = decl->as<xi_function_decl>();
            ast_type*               rtype = fdecl->return_type;
            std::vector<ast_type*>  pvec;
            for(auto p: fdecl->parameters) {
                pvec.push_back(p->type);
            }
            list<ast_type>*         plist = new list<ast_type>(pvec);
            return this->get_function_type(rtype, box(plist));
        }
    case tree_type_id::xi_struct_decl:
    case tree_type_id::xi_class_decl:
    case tree_type_id::xi_mixin_decl:
        {
            return this->get_object_type(decl->as<xi_type_decl>());
        }
    case tree_type_id::xi_field_decl:
        {
            return decl->as<xi_field_decl>()->type;
        }
    case tree_type_id::xi_group_decl:
        {
            return new xi_group_type(map<ast_type, ast_decl>(decl->as<xi_group_decl>()->declarations, [&](ast_decl* d){
                return this->get_declaration_type(d);
            }));
        }
    }

    return base_builder_t::get_declaration_type(decl);
}

ast_decl* xi_builder::find_member(ast_namespace_decl* decl, const char* name) {
    auto sname = std::string(name);
    for(auto m: decl->declarations) {
        if(sname == (std::string) m->name) {
            return m;
        }
    }
    return nullptr;
}

ast_decl* xi_builder::find_member(xi_type_decl* decl, const char* name) {
    throw std::runtime_error("Not implemented");
}

xi_type_decl* xi_builder::find_type_decl(const char* name) {
    for(auto decl: unbox(this->context->findall(name, true))) {
        if(decl->is<xi_type_decl>()) {
            return decl->as<xi_type_decl>();
        }
    }
    return nullptr;
}


void find_members(xi_type_decl* decl, member_search_parameters& p) {
    if(std::find(p.searched.begin(), p.searched.end(), decl) == p.searched.end()) {
        p.searched.push_back(decl);
        switch(decl->get_tree_type()) {
        case tree_type_id::xi_struct_decl:
            find_members(decl->as<xi_struct_decl>(), p);
            break;
        default:
            throw std::runtime_error("at " __FILE__ " : " + std::to_string(__LINE__) + " Unhandled type: " + decl->get_tree_type_name());
        }
    }
}

ptr<list<xi_member_decl>> xi_builder::find_instance_members(xi_type_decl* decl, const char* name) {
    member_search_parameters params(name, true, false);

    find_members(decl, params);
    return params.found;
}

ptr<list<xi_member_decl>> xi_builder::find_static_members(xi_type_decl* decl, const char* name) {
    member_search_parameters params(name, true, true);

    find_members(decl, params);
    return params.found;
}

ast_expr* xi_builder::make_default_initializer(ast_type* tp) {
    if(tp->is<xi_object_type>()) {
        return new xi_zero_initializer_expr(tp, tp->as<xi_object_type>()->declaration);
    }
    else {
        return this->make_zero(tp);
    }
}

ast_expr* xi_builder::make_op(xi_operator op, ast_expr* expr) {
    return xcc::copyloc(new xi_op_expr(nullptr, op, new list<ast_expr>({expr})), expr);
}

ast_expr* xi_builder::make_op(xi_operator op, ast_expr* lhs, ast_expr* rhs) {
    return xcc::copyloc(new xi_op_expr(nullptr, op, new list<ast_expr>({lhs, rhs})), lhs, rhs);
}

ast_expr* xi_builder::make_op(xi_operator op, list<ast_expr>* operands) {
    //TODO: copyloc
    return new xi_op_expr(nullptr, op, operands);
}

ast_expr* xi_builder::make_deref_expr(ast_expr* expr) const {
    assert(expr->type->is<xi_ref_type>() || expr->type->is<ast_pointer_type>()); //TODO: maybe not until typecheck
    if(expr->type->is<xi_ref_type>()) {
        return new ast_deref(expr->type->as<xi_ref_type>()->element_type, expr);
    }
    return base_builder_t::make_deref_expr(expr);
}

ast_expr* xi_builder::make_memberref_expr(ast_expr* mexpr, const char* name) {
    return new xi_named_memberref_expr(mexpr, name);
    //xi_member_decl* decl = this->find_instance_member(mexpr->type, name);
    //return new xi_bound_member(mexpr, decl);
}

ast_expr* xi_builder::make_memberref_expr(ast_type* type, const char* name) {
    return new xi_static_named_memberref_expr(type, name);
}

ast_expr* xi_builder::make_declref_expr(ast_decl* decl) {
    if(decl->is<xi_group_decl>()) {
        auto gexpr = new xi_group_expr(map<ast_expr, ast_decl>(
                    decl->as<xi_group_decl>()->declarations,
                    [&](ast_decl* d) { return this->make_declref_expr(d); }));
        gexpr->type = this->get_declaration_type(decl);
        return gexpr;
    }
    return base_builder_t::make_declref_expr(decl);
}

ast_expr* xi_builder::make_cast_expr(ast_type* type, ast_expr* expr) const {
    assert(type != nullptr && expr != nullptr);
    return xcc::copyloc(new ast_cast(type, ast_op::none, expr), expr);
}

ast_expr* xi_builder::make_index_expr(ast_expr* arrexpr, list<ast_expr>* index_exprs) {
    assert(arrexpr != nullptr && index_exprs != nullptr);
    return xcc::copyloc(new xi_index_expr(nullptr, arrexpr, index_exprs), arrexpr);
}

ast_expr* xi_builder::make_call_expr(ast_expr* fexpr, list<ast_expr>* args) const {
    assert(fexpr != nullptr && args != nullptr);
    return xcc::copyloc(new ast_invoke(nullptr, fexpr, args), fexpr);
}

ast_expr* xi_builder::make_ctor_expr(ast_type* tp, list<ast_expr>* args) {
    //
    // 1. find __init__ funcion (TODO: function group)
    //

    auto funcdels   = box(filter<xi_function_decl>(this->find_all_declarations("__init__")));
    auto ctordecls  = box(new list<ast_decl>());
    for(auto f: funcdels) {
        xi_parameter_decl* selfparam = f->parameters[0];
        if(this->isrefof(selfparam->type, tp)) {
            ctordecls->append(f);
        }
    }

    //
    // 2. make ctor declaration reference
    //

    ast_expr* funcexpr = nullptr;
    if(ctordecls->size() == 0) {
        // TODO: is this an error?
        // there are no constructors
        assert(false);
    }
    else if(ctordecls->size() == 1) {
        funcexpr = this->make_declref_expr((*ctordecls)[0]);
    }
    else {
        funcexpr = this->make_declref_expr(new xi_group_decl("__init__", ctordecls));
    }

    //
    // 3. make temp
    //

    auto tmp              = this->define_local_variable(tp);
    ast_expr* tmp_expr    = this->make_declref_expr(tmp);
    auto      invoke_args = new list<ast_expr>(tmp_expr);
    for(auto e: args) {
        invoke_args->append(e);
    }

    //
    // 4. make stmt expression
    //
    ast_expr* invoke_expr = this->make_call_expr(funcexpr, invoke_args);
    ast_stmt* invoke_stmt = this->make_expr_stmt(invoke_expr);

    return this->make_stmt_expr(new list<ast_stmt>(invoke_stmt), tmp_expr);
}

ast_stmt* xi_builder::make_return_stmt(ast_type* rt, ast_expr* expr) const noexcept {
    assert(expr != nullptr);
    return xcc::copyloc(new ast_return_stmt(this->make_cast_expr(rt, expr)), expr);
}

ast_stmt* xi_builder::make_assign_stmt(xi_operator op, ast_expr* lhs, ast_expr* rhs) const noexcept {
    assert(lhs != nullptr && rhs != nullptr);
    return xcc::copyloc(new xi_assign_stmt(op, lhs, rhs), lhs, rhs);
}

ast_stmt* xi_builder::make_for_stmt(ast_local_decl* decl, ast_expr* iterexpr, ast_stmt* body) const noexcept {
    assert(body != nullptr);
    return new xi_foriter_stmt(decl, iterexpr, body);
}

//TODO: visibility
ast_expr* xi_builder::make_instance_memberref_expr(xi_type_decl* decl, ast_expr* objexpr, const char* name, const source_span& span) {
    auto members = this->find_instance_members(decl, name);

    if(members->size() == 0) {
        //TODO: error
        throw std::runtime_error("WHAT");
    }
    else if(members->size() == 1) {
        auto m    = (*members)[0];
        auto type = this->get_declaration_type(m);
        return this->setloc(new xi_bound_memberref_expr(type, objexpr, m), span);
    }
    else {
        throw std::runtime_error("WHAT");
    }
}

//TODO: visibility
ast_expr* xi_builder::make_static_memberref_expr(xi_type_decl* decl, const char* name, const source_span& span) {
    auto members = this->find_static_members(decl, name);

    if(members->size() == 0) {
        throw std::runtime_error("WHAT");
    }
    else if(members->size() == 1) {
        auto m    = (*members)[0];
        auto type = this->get_declaration_type(m);
        return this->setloc(new xi_static_memberref_expr(type, m), span);
    }
    else {
        throw std::runtime_error("WAHT");
    }
}



bool xi_builder::widens(ast_type* ftype, ast_type* ttype) const {
    if(ftype == ttype) return true;
#   define WIDEN_CASE(ft, tt, f, t, s)\
    if(ftype->is<ft>() && ttype->is<tt>()) {\
        auto f = ftype->as<ft>();\
        auto t = ttype->as<tt>();\
        s\
    }

    WIDEN_CASE(xi_object_type,      xi_ref_type,    obj_t,  ref_t,      return widens(obj_t, ref_t->element_type);)
    WIDEN_CASE(ast_integer_type,    ast_real_type,  it,     ft,         return true;)
    WIDEN_CASE(ast_type,            xi_const_type,  t,      con_t,      return widens(t, con_t->type);)

    return base_builder_t::widens(ftype, ttype);
#   undef WIDEN_CASE
}

ast_expr* xi_builder::widen(ast_type* ttype, ast_expr* expr, uint32_t& effort) const {
#   define WIDEN_CASE(ft, tt, f, t, s)\
    if(expr->type->is<ft>() && ttype->is<tt>()) {\
        auto f = expr->type->as<ft>();\
        auto t = ttype->as<tt>();\
        s;\
    }

    WIDEN_CASE(xi_object_type, xi_ref_type, obj_t, ref_t,
            auto newexpr = new ast_addressof(ref_t, expr);
            this->copyloc(newexpr, expr);
            return newexpr;
    )
    WIDEN_CASE(ast_type, xi_const_type, t, con_t,
            auto newexpr = new ast_cast(con_t, ast_op::none, expr);
            this->copyloc(newexpr, this->widen(con_t, expr, effort));
            return newexpr;
    )
    WIDEN_CASE(ast_integer_type, ast_real_type, it, rt,
            effort++;
            return ast_builder::widen(rt, expr);
    )
    WIDEN_CASE(ast_real_type,    ast_real_type, ft, tt,
            if((uint32_t) ft->bitwidth != (uint32_t) tt->bitwidth) {
                effort++;
                return ast_builder::widen(tt, expr);
            }
    )

    effort = 0;
    return base_builder_t::widen(ttype, expr);
}

bool xi_builder::sametype(ast_type* lhs, ast_type* rhs) const {

    if(lhs == rhs) return true;
    if(lhs->get_tree_type() != rhs->get_tree_type()) return false;

    switch(lhs->get_tree_type()) {
    //case tree_type_id::xi_array_type:
    case tree_type_id::xi_const_type:
        {
            return this->sametype(lhs->as<xi_const_type>()->type,
                                  rhs->as<xi_const_type>()->type);
        }
    case tree_type_id::xi_object_type:
        {
            //TODO: better declaration comparison???
            return lhs->as<xi_object_type>()->declaration ==
                   rhs->as<xi_object_type>()->declaration;
        }
    case tree_type_id::xi_ref_type:
        {
            return this->sametype(lhs->as<xi_ref_type>()->element_type,
                                  rhs->as<xi_ref_type>()->element_type);
        }
    }

    return base_builder_t::sametype(lhs, rhs);
}

ast_decl* xi_builder::find_declaration(const char* name) noexcept {
    auto alldecls = this->context->findall(name);

    bool searching_functions = false;
    auto found_functions = box(new list<ast_decl>());

    for(auto decl: alldecls) {
        if(!searching_functions) {
            if(decl->is<xi_function_decl>()) {
                searching_functions = true;
            }
            else {
                return decl;
            }
        }
        found_functions->append(decl);
    }

    if(found_functions->size() == 0) {
        return nullptr;
    }
    else if(found_functions->size() == 1) {
        return (*found_functions)[0];
    }

    return new xi_group_decl(name, found_functions);
}

void xi_builder::dump_parse() {
    this->dump_parse(std::cout);
}

void xi_builder::dump_parse(std::ostream& ostr) {
    for(auto tp: this->all_types) {
        ast_printer::print(tp, ostr);
    }

    for(auto f: this->all_functions) {
        ast_printer::print(f, ostr);
    }
}

void xi_builder::dump_unit() {
    this->dump_unit(std::cout);
}

void xi_builder::dump_unit(std::ostream& ostr) {
    for(auto f: this->tu.global_function_declarations) {
        ast_printer::print(f, ostr);
    }
}

ast_decl* xi_builder::lower(ast_decl* decl) {
    return this->_lower_walker->lower_decl(decl);
}

ast_stmt* xi_builder::lower(ast_stmt* stmt) {
    return this->_lower_walker->lower_stmt(stmt);
}

ast_expr* xi_builder::lower(ast_expr* expr) {
    return this->_lower_walker->lower_expr(expr);
}

ast_type* xi_builder::lower(ast_type* type) {
    return this->_lower_walker->lower_type(type);
}

void xi_builder::lower_pass() {
    for(auto f : this->all_functions) {
        f->generated_function = this->lower_function(f);
    }

    for(auto f: this->all_functions) {
        this->lower_body(f);
        this->tu.append(f->generated_function);
    }
}

void xi_builder::generate() {
    //TODO: do all xi passes

    //xi_finalize_types_pass finalize_types(*this);
    //finalize_types.visit(this->global_namespace);

    xi_typecheck_pass typecheck_pass(*this);
    typecheck_pass.visit(this->global_namespace);

    xi_finalize_types_pass finalize_types(*this);
    finalize_types.visit(this->global_namespace);

    this->lower_pass();
}

}


