/*
 * xi_builder.cpp
 *
 *  Created on: Feb 1, 2017
 *      Author: derick
 */

#include "xi_builder.hpp"
#include "xi_lower.hpp"

// passes
#include "xi_pass_finalize_types.hpp"
#include "xi_pass_typecheck.hpp"

namespace xcc {

xi_builder::xi_builder(translation_unit& tu)
    : ast_builder<>(tu),
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
    }

    return ast_builder<>::get_declaration_type(decl);
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
    return ast_builder<>::make_deref_expr(expr);
}

ast_expr* xi_builder::make_memberref_expr(ast_expr* mexpr, const char* name) {
    return new xi_named_memberref_expr(mexpr, name);
}

ast_expr* xi_builder::make_fieldref_expr(ast_expr* objexpr, xi_field_decl* field) {
    return new ast_memberref(field->type, objexpr, field->field_index);
}

ast_expr* xi_builder::make_memberref_expr(ast_type* type, const char* name) {
    return new xi_static_named_memberref_expr(type, name);
}

ast_expr* xi_builder::make_fieldref_expr(ast_type* objexpr, xi_field_decl* field) {
    throw std::runtime_error(__FILE__ ":" + std::to_string(__LINE__) + ": Not implemented");
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

ast_expr* xi_builder::widen(ast_type* desttype, ast_expr* expr) const {
    return ast_builder<>::widen(desttype, expr);
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

    return ast_builder<>::sametype(lhs, rhs);
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

void xi_builder::set_type_widens(ast_type* from, ast_type* to) {
    auto from_iter = this->_type_rules_widens.find(from);
    if(from_iter == this->_type_rules_widens.end()) {
        this->_type_rules_widens[from] = std::vector<ast_type*>();
    }
    this->_type_rules_widens[from].push_back(to);
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

    xi_finalize_types_pass finalize_types(*this);
    finalize_types.visit(this->global_namespace);

    xi_typecheck_pass typecheck_pass(*this);
    typecheck_pass.visit(this->global_namespace);

    this->lower_pass();
}

}


