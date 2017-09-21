/*
 * ast_builder.cpp
 *
 *  Created on: Jan 22, 2017
 *      Author: derick
 */

#include <string>
#include <iostream>
#include <sstream>

#include <exception>
#include "ast_builder.hpp"
#include "frontend.hpp"


namespace xcc {

__ast_builder_impl::__ast_builder_impl(translation_unit& tu, ast_name_mangler* mangler) noexcept
            : _mangler_ptr(mangler),
              get_mangled_name(*mangler),
              tu(tu),
              global_namespace(new ast_namespace_decl("global")),
              _pointer_types(0, std::hash<ast_type*>(), sametype_predicate(*this)),
              _function_types(0, functype_hasher(*this), samefunctype_predicate(*this)),
              _record_types(0, typelist_hasher(*this), sametypelist_predicate(*this)),
              _the_nop_stmt(new ast_nop_stmt()),
              _the_break_stmt(new ast_break_stmt()),
              _the_continue_stmt(new ast_continue_stmt()) {

    this->context = new ast_namespace_context(nullptr, this->global_namespace);
    this->create_default_types();
}

void __ast_builder_impl::create_default_types() noexcept {

    this->_the_void_type = new ast_void_type();
    this->_the_void_ptr_type = this->get_pointer_type(this->_the_void_type);

    this->_unsigned_integer_types[1]        = new ast_integer_type(1, true);
    for(uint32_t size = 1; size <= 64; size *= 2) {
        this->_signed_integer_types[size]   = new ast_integer_type(size, false);
        this->_unsigned_integer_types[size] = new ast_integer_type(size, true);
    }

    this->_real_types[32] = new ast_real_type(32);
    this->_real_types[64] = new ast_real_type(64);
    this->_the_boolean_type = this->get_integer_type(1, true);

    this->_false_value  = new ast_integer(this->_the_boolean_type, llvm::APSInt::get(0));
    this->_true_value   = new ast_integer(this->_the_boolean_type, llvm::APSInt::get(1));
}

ast_void_type* __ast_builder_impl::get_void_type() const noexcept {
    return this->_the_void_type;
}

ast_integer_type* __ast_builder_impl::get_integer_type(uint32_t bitwidth, bool is_unsigned) const noexcept {
    if(is_unsigned) {
        return this->_unsigned_integer_types.at(bitwidth);
    }
    else {
        return this->_signed_integer_types.at(bitwidth);
    }
}

ast_integer_type* __ast_builder_impl::get_bool_type() const noexcept {
    return this->get_integer_type(1, true);
}

ast_real_type* __ast_builder_impl::get_real_type(uint32_t bitwidth) const noexcept {
    return this->_real_types.at(bitwidth);
}

ast_pointer_type* __ast_builder_impl::get_pointer_type(ast_type* eltype) noexcept {
    auto key = eltype;
    if(this->_pointer_types.find(key) == this->_pointer_types.end()) {
        this->_pointer_types[key] = new ast_pointer_type(eltype);
    }
    return this->_pointer_types[key];
}

ast_array_type* __ast_builder_impl::get_array_type(ast_type* eltype, uint32_t size) noexcept {
    auto key = std::make_tuple(eltype, size);
    if(this->_array_types.find(key) == this->_array_types.end()) {
        this->_array_types[key] = new ast_array_type(eltype, size);
    }
    return this->_array_types[key];
}

ast_function_type* __ast_builder_impl::get_function_type(ast_type* rtype, ptr<list<ast_type>> params) noexcept {
    auto key = functypekey_t(rtype, unbox(params));
    if(this->_function_types.find(key) == this->_function_types.end()) {
        this->_function_types[key] = new ast_function_type(rtype, params);
    }
    return this->_function_types[key];
}

ast_record_type* __ast_builder_impl::get_record_type(ptr<list<ast_type>> types) noexcept {
    if(this->_record_types.find(types) == this->_record_types.end()) {
        this->_record_types[types] = new ast_record_type(types);
    }
    return this->_record_types[types];
}

ast_type* __ast_builder_impl::get_string_type(uint32_t length) noexcept {
    return this->get_array_type(this->get_char_type(), length);
}

ast_type* __ast_builder_impl::get_char_type() noexcept {
    return this->get_integer_type(8, true);
}

ast_type* __ast_builder_impl::get_declaration_type(ast_decl* decl) noexcept {
    switch(decl->get_tree_type()) {
    case tree_type_id::ast_variable_decl:
        return decl->as<ast_variable_decl>()->type;
    case tree_type_id::ast_local_decl:
        return decl->as<ast_local_decl>()->type;
    case tree_type_id::ast_parameter_decl:
        return decl->as<ast_parameter_decl>()->type;
    case tree_type_id::ast_function_decl:
        {
            ast_function_decl*      fdecl   = decl->as<ast_function_decl>();
            ast_type*               rtype   = fdecl->return_type;
            std::vector<ast_type*>  pvec;
            for(auto p: fdecl->parameters) {
                pvec.push_back(p->type);
            }
            list<ast_type>*         plist = new list<ast_type>(pvec);
            return this->get_function_type(rtype, box(plist));
        }
    }
    //TODO: error unhandled
    throw std::runtime_error(__FILE__ ":" + std::to_string(__LINE__) + " Unhandled " + std::string(decl->get_tree_type_name()) + "\n");
}

ast_namespace_decl* __ast_builder_impl::define_namespace(const char*name) noexcept {
    auto decl = this->context->find(name, false);
    if(decl != nullptr && decl->is<ast_namespace_decl>()) {
        return decl->as<ast_namespace_decl>();
    }
    else {
        auto new_ns = new ast_namespace_decl(name);
        this->context->insert(name, new_ns);
        return new_ns;
    }
}

ast_expr* __ast_builder_impl::make_integer(const char* txt, uint8_t radix) const noexcept {
    auto bitsneeded = llvm::APInt::getBitsNeeded(llvm::StringRef(txt), radix);
    llvm::APSInt value(llvm::APInt(bitsneeded, llvm::StringRef(txt), radix));

    uint32_t bitwidth = 0;
    if(bitsneeded <= 8)  { bitwidth = 8;  }
    if(bitsneeded <= 16) { bitwidth = 16; }
    if(bitsneeded <= 32) { bitwidth = 32; }
    if(bitsneeded <= 64) { bitwidth = 64; }

    assert(bitwidth != 0);

    return new ast_integer(this->get_integer_type(bitwidth, false), value);
}

ast_expr* __ast_builder_impl::make_real(const char* txt) const noexcept {
    llvm::APFloat value(std::atof(txt));

    return new ast_real(this->get_real_type(64), value);
}

ast_expr* __ast_builder_impl::make_string(const char* txt, uint32_t start, uint32_t length) noexcept {
    char dest[length + 1];
    std::strncpy(dest, &txt[start], length);
    dest[length] = '\0';
    return new ast_string(this->get_string_type(length + 1), std::string(dest));
}

ast_expr* __ast_builder_impl::make_true() const noexcept {
    return this->_true_value;
}

ast_expr* __ast_builder_impl::make_false() const noexcept {
    return this->_false_value;
}

ast_expr* __ast_builder_impl::make_zero(ast_type* tp) const noexcept {
    switch(tp->get_tree_type()) {
    case tree_type_id::ast_integer_type:        return new ast_integer(tp, llvm::APSInt::get(0));
    case tree_type_id::ast_real_type:           return new ast_real(tp,    llvm::APFloat(0.0));
    case tree_type_id::ast_record_type:
        {
            auto rectype = tp->as<ast_record_type>();
            ptr<list<ast_expr>> exprlist = new list<ast_expr>();
            for(auto ftp: rectype->field_types) {
                exprlist->append(this->make_zero(ftp));
            }
            return new ast_record(tp, exprlist);
        }
    case tree_type_id::ast_array_type:
        {
            auto arrtype = tp->as<ast_array_type>();
            ptr<list<ast_expr>> exprlist = new list<ast_expr>();
            for(size_t i = 0; i < arrtype->size; i++) {
                exprlist->append(this->make_zero(arrtype->element_type));
            }
            return new ast_array(tp, exprlist);
        }
    }
    throw std::runtime_error("unhandled ast type " + std::to_string((uint32_t) tp->get_tree_type()));
}

ast_expr* __ast_builder_impl::make_cast_expr(ast_type* desttype, ast_expr* expr) const {
    return this->make_lower_cast_expr(desttype, expr);
}

ast_expr* __ast_builder_impl::make_lower_cast_expr(ast_type* desttype, ast_expr* expr) const {
    switch(desttype->get_tree_type()) {
    case tree_type_id::ast_integer_type:            return this->cast_to(desttype->as<ast_integer_type>(), expr);
    case tree_type_id::ast_real_type:               return this->cast_to(desttype->as<ast_real_type>(),    expr);
    case tree_type_id::ast_pointer_type:            return this->cast_to(desttype->as<ast_pointer_type>(), expr);
    case tree_type_id::ast_record_type:             return this->cast_to(desttype->as<ast_record_type>(),  expr);
    }
    throw std::runtime_error("unhandled ast type " + std::string(desttype->get_tree_type_name()) + " in __ast_builder::make_lower_cast_expr");
}

static ast_expr* make_arithmetic_op_expr(__ast_builder_impl* builder, ast_op op, ast_expr* lhs, ast_expr* rhs) {
    auto    t       = builder->maxtype(lhs->type, rhs->type);
    auto    wlhs    = builder->widen(t, lhs);
    auto    wrhs    = builder->widen(t, rhs);

    ast_op  new_op  = ast_op::none;

    if(t->is<ast_integer_type>()) {
        if(t->as<ast_integer_type>()->is_unsigned) {
            switch(op) {
            case ast_op::add:   new_op  = ast_op::iadd; break;
            case ast_op::sub:   new_op  = ast_op::isub; break;
            case ast_op::mul:   new_op  = ast_op::imul; break;
            case ast_op::div:   new_op  = ast_op::udiv; break;
            case ast_op::mod:   new_op  = ast_op::umod; break;
            }
        }
        else {
            switch(op) {
            case ast_op::add:   new_op  = ast_op::iadd; break;
            case ast_op::sub:   new_op  = ast_op::isub; break;
            case ast_op::mul:   new_op  = ast_op::imul; break;
            case ast_op::div:   new_op  = ast_op::idiv; break;
            case ast_op::mod:   new_op  = ast_op::imod; break;
            }
        }
    }
    else if(t->is<ast_real_type>()) {
        switch(op) {
        case ast_op::add:       new_op  = ast_op::fadd; break;
        case ast_op::sub:       new_op  = ast_op::fsub; break;
        case ast_op::mul:       new_op  = ast_op::fmul; break;
        case ast_op::div:       new_op  = ast_op::fdiv; break;
        case ast_op::mod:       new_op  = ast_op::fmod; break;
        }
    }
    else if(t->is<ast_pointer_type>()) {
        switch(op) {
        case ast_op::add:       new_op  = ast_op::iadd; break;
        case ast_op::sub:       new_op  = ast_op::isub; break;
        }
    }

    assert(new_op != ast_op::none);
    return new ast_binary_op(t, new_op, wlhs, wrhs);
}

static ast_expr* make_comparison_op_expr(__ast_builder_impl* builder, ast_op op, ast_expr* lhs, ast_expr* rhs) {
    auto    t       = builder->maxtype(lhs->type, rhs->type);
    auto    wlhs    = builder->widen(t, lhs);
    auto    wrhs    = builder->widen(t, rhs);

    ast_op  new_op  = ast_op::none;

    if(t->is<ast_integer_type>()) {
        if(t->as<ast_integer_type>()->is_unsigned) {
            switch(op) {
            case ast_op::eq:    new_op  = ast_op::cmp_eq;   break;
            case ast_op::ne:    new_op  = ast_op::cmp_ne;   break;
            case ast_op::lt:    new_op  = ast_op::icmp_ult; break;
            case ast_op::le:    new_op  = ast_op::icmp_ule; break;
            case ast_op::gt:    new_op  = ast_op::icmp_ugt; break;
            case ast_op::ge:    new_op  = ast_op::icmp_uge; break;
            }
        }
        else {
            switch(op) {
            case ast_op::eq:    new_op  = ast_op::cmp_eq;   break;
            case ast_op::ne:    new_op  = ast_op::cmp_ne;   break;
            case ast_op::lt:    new_op  = ast_op::icmp_slt; break;
            case ast_op::le:    new_op  = ast_op::icmp_sle; break;
            case ast_op::gt:    new_op  = ast_op::icmp_sgt; break;
            case ast_op::ge:    new_op  = ast_op::icmp_sge; break;
            }
        }
    }
    else if(t->is<ast_real_type>()) {
        switch(op) {
        case ast_op::eq:        new_op  = ast_op::cmp_eq;   break;
        case ast_op::ne:        new_op  = ast_op::cmp_ne;   break;
        case ast_op::lt:        new_op  = ast_op::fcmp_olt; break;
        case ast_op::le:        new_op  = ast_op::fcmp_ole; break;
        case ast_op::gt:        new_op  = ast_op::fcmp_ogt; break;
        case ast_op::ge:        new_op  = ast_op::fcmp_ole; break;
        }
    }
    else if(t->is<ast_pointer_type>()) {
        switch(op) {
        case ast_op::eq:        new_op  = ast_op::cmp_eq;   break;
        case ast_op::ne:        new_op  = ast_op::cmp_ne;   break;
        }
    }

    assert(new_op != ast_op::none);
    return new ast_binary_op(builder->get_bool_type(), new_op, wlhs, wrhs);
}

static ast_expr* make_logical_op_expr(__ast_builder_impl* builder, ast_op op, ast_expr* lhs, ast_expr* rhs) {
    auto    blhs    = builder->make_cast_expr(builder->get_bool_type(), lhs);
    auto    brhs    = builder->make_cast_expr(builder->get_bool_type(), rhs);

    ast_op  new_op  = ast_op::none;

    switch(op) {
    case ast_op::logical_and:   new_op = ast_op::land; break;
    case ast_op::logical_or:    new_op = ast_op::lor;  break;
    }

    assert(new_op != ast_op::none);
    return new ast_binary_op(builder->get_bool_type(), new_op, blhs, brhs);
}

static ast_expr* make_bitwise_op_expr(__ast_builder_impl* builder, ast_op op, ast_expr* lhs, ast_expr* rhs) {
    auto    t       = builder->maxtype(lhs->type, rhs->type);

    assert(t->is<ast_integer_type>());

    auto    wlhs    = builder->widen(t, lhs);
    auto    wrhs    = builder->widen(t, rhs);

    ast_op  new_op  = ast_op::none;

    switch(op) {
    case ast_op::binary_and:    new_op  = ast_op::band;     break;
    case ast_op::binary_or:     new_op  = ast_op::bor;      break;
    case ast_op::binary_xor:    new_op  = ast_op::bxor;     break;
    }

    assert(new_op != ast_op::none);
    return new ast_binary_op(t, new_op, wlhs, wrhs);
}

static ast_expr* make_shift_op_expr(__ast_builder_impl* builder, ast_op op, ast_expr* lhs, ast_expr* rhs) {
    assert(lhs->is<ast_integer_type>());
    assert(rhs->is<ast_integer_type>());

    ast_op  new_op  = ast_op::none;

    switch(op) {
    case ast_op::shl:   new_op = ast_op::bshl; break;
    case ast_op::shr:
        if(lhs->as<ast_integer_type>()->is_unsigned) {
            new_op = ast_op::bshr;
        }
        else {
            new_op = ast_op::ashr;
        }
    }

    assert(new_op != ast_op::none);
    return new ast_binary_op(lhs->type, new_op, lhs, rhs);
}

ast_expr* __ast_builder_impl::make_op_expr(ast_op op, ast_expr* lhs, ast_expr* rhs) {
    return this->make_lower_op_expr(op, lhs, rhs);
}

ast_expr* __ast_builder_impl::make_lower_op_expr(ast_op op, ast_expr* lhs, ast_expr* rhs) {
    assert(is_highlevel_op(op));

    switch(op) {
    // Basic arithmetic
    case ast_op::add:
    case ast_op::sub:
    case ast_op::mul:
    case ast_op::div:
    case ast_op::mod:
        return make_arithmetic_op_expr(this, op, lhs, rhs);

    // Comparison
    case ast_op::eq:
    case ast_op::ne:
    case ast_op::lt:
    case ast_op::le:
    case ast_op::gt:
    case ast_op::ge:
        return make_comparison_op_expr(this, op, lhs, rhs);

    // Logical
    case ast_op::logical_and:
    case ast_op::logical_or:
        return make_logical_op_expr(this, op, lhs, rhs);

    // Bitwise
    case ast_op::binary_and:
    case ast_op::binary_or:
    case ast_op::binary_xor:
        return make_bitwise_op_expr(this, op, lhs, rhs);

    // Shift
    case ast_op::shl:
    case ast_op::shr:
        return make_shift_op_expr(this, op, lhs, rhs);

    default:
        throw std::runtime_error("unhandled operator in __ast_builder_impl::make_op_expr(binary)");
    }
}

ast_expr* __ast_builder_impl::make_op_expr(ast_op op, ast_expr* expr) {
    assert(is_highlevel_op(op));

    switch(op) {
    case ast_op::add:
        return expr;
    case ast_op::sub:
        {
            if(expr->type->is<ast_integer_type>()) {
                return new ast_unary_op(expr->type, ast_op::ineg, expr);
            }
            else if(expr->type->is<ast_real_type>()) {
                return new ast_unary_op(expr->type, ast_op::fneg, expr);
            }
            break;
        }
    case ast_op::logical_not:
        {
            return new ast_unary_op(this->get_bool_type(), ast_op::lnot, expr);
        }
    case ast_op::binary_not:
        {
            return new ast_unary_op(expr->type, ast_op::bnot, expr);
        }
    }
    throw std::runtime_error("unhandled operator in __ast_builder_imppl::make_op_expr(unary)");
}

ast_expr* __ast_builder_impl::make_declref_expr(ast_decl* decl) {
    return new ast_declref(this->get_declaration_type(decl), decl);
}

ast_expr* __ast_builder_impl::make_memberref_expr(ast_expr* obj, uint32_t member) {
    assert(obj->type->is<ast_record_type>());
    return new ast_memberref(obj->type->as<ast_record_type>()->field_types[member], obj, member);
}

ast_expr* __ast_builder_impl::make_deref_expr(ast_expr* e) const {
    assert(e->type->is<ast_pointer_type>());

    if(e->is<ast_addressof>()) {
        return e->as<ast_addressof>()->expr;
    }

    ast_type* et = e->type->as<ast_pointer_type>()->element_type;
    return new ast_deref(et, e);
}

ast_expr* __ast_builder_impl::make_addressof_expr(ast_expr* e) {
    return this->make_lower_addressof_expr(e);
}

ast_expr* __ast_builder_impl::make_lower_addressof_expr(ast_expr* e) {
    if(e->is<ast_deref>()) {
        return e->as<ast_deref>()->expr;
    }

    ast_type* t = this->get_pointer_type((ast_type*) e->type);
    return new ast_addressof(t, e);
}

ast_expr* __ast_builder_impl::make_index_expr(ast_expr* arrexpr, ast_expr* idxexpr) const {
    assert(idxexpr->type->is<ast_integer_type>());

    //TODO: pointer arithetic

    ast_type* t = arrexpr->type->as<ast_array_type>()->element_type;
    return new ast_index(t, arrexpr, idxexpr);
}

ast_expr* __ast_builder_impl::make_call_expr(ast_expr* fexpr, list<ast_expr>* args) const {
    return this->make_lower_call_expr(fexpr, args);
}

ast_expr* __ast_builder_impl::make_lower_call_expr(ast_expr* fexpr, list<ast_expr>* args) const {
    assert(fexpr->type->is<ast_function_type>());

    auto ftype = fexpr->type->as<ast_function_type>();
    list<ast_expr>* new_args = new list<ast_expr>();
    for(uint32_t i = 0; i < args->size(); i++) {
        auto ptp    = (*ftype->parameter_types)[i];
        auto narg   = (*args)[i];

        new_args->append(this->make_lower_cast_expr(ptp, narg));
    }

    ast_type* t = fexpr->type->as<ast_function_type>()->return_type;
    if(fexpr->is<ast_declref>() && fexpr->as<ast_declref>()->declaration->is<ast_function_decl>()) {
        return new ast_call(t, fexpr->as<ast_declref>()->declaration, new_args);
    }
    else {
        return new ast_invoke(t, fexpr, new_args);
    }
}

ast_expr* __ast_builder_impl::make_stmt_expr(list<ast_stmt>* stmts, ast_expr* expr) const noexcept {
    return new ast_stmt_expr(expr->type, stmts, expr);
}

uint32_t __ast_builder_impl::foldu32(ast_expr* e) {
    e = this->fold(e);
    switch(e->get_tree_type()) {
    case tree_type_id::ast_integer:
        {
            llvm::APSInt value = e->as<ast_integer>()->value;
            return std::atoi(value.toString(10).c_str());
        }
    }
    throw std::runtime_error("unhandled expr " + std::string(e->get_tree_type_name()) + " in foldu32\n");
}

uint64_t __ast_builder_impl::foldu64(ast_expr* e) {
    e = this->fold(e);
    switch(e->get_tree_type()) {
    case tree_type_id::ast_integer:
        {
            llvm::APSInt value = e->as<ast_integer>()->value;
            return std::atol(value.toString(10).c_str());
        }
    }
    throw std::runtime_error("unhandled expr " + std::string(e->get_tree_type_name()) + " in foldu64\n");
}

ast_expr* __ast_builder_impl::fold(ast_expr* e) {
    switch(e->get_tree_type()) {
    case tree_type_id::ast_integer:
    case tree_type_id::ast_real:
        return e;
    }
    throw std::runtime_error("unhandled expr " + std::string(e->get_tree_type_name()) + " in fold\n");
}

ast_stmt* __ast_builder_impl::make_nop_stmt() const noexcept {
    return this->_the_nop_stmt;
}

ast_stmt* __ast_builder_impl::make_expr_stmt(ast_expr* e) const noexcept {
    return new ast_expr_stmt(e);
}

ast_stmt* __ast_builder_impl::make_assign_stmt(ast_expr* lhs, ast_expr* rhs) noexcept {
    return this->make_lower_assign_stmt(lhs, rhs);
}

ast_stmt* __ast_builder_impl::make_lower_assign_stmt(ast_expr* lhs, ast_expr* rhs) noexcept {
    auto dest = lhs;
    auto src  = this->make_lower_cast_expr(lhs->type, rhs);

    return new ast_assign_stmt(dest, src);
}

ast_stmt* __ast_builder_impl::make_block_stmt() const noexcept {
    return new ast_block_stmt();
}

ast_stmt* __ast_builder_impl::make_break_stmt() const noexcept {
    return unbox(this->_the_break_stmt);
}

ast_stmt* __ast_builder_impl::make_continue_stmt() const noexcept {
    return unbox(this->_the_continue_stmt);
}

ast_stmt* __ast_builder_impl::make_return_stmt(ast_type* t, ast_expr* expr) const noexcept {
    if(!t->is<ast_void_type>()) {
        //TODO: cast to return type
        return new ast_return_stmt(this->make_cast_expr(t, expr));
    }
    else {
        return new ast_return_stmt(nullptr);
    }
}

ast_stmt* __ast_builder_impl::make_if_stmt(ast_expr* cond, ast_stmt* tstmt, ast_stmt* fstmt) const noexcept {
    //TODO: constant condition check ???
    return new ast_if_stmt(this->make_cast_expr(this->_the_boolean_type, cond), tstmt, fstmt);
}

ast_stmt* __ast_builder_impl::make_while_stmt(ast_expr* cond, ast_stmt* stmt) const noexcept {
    //TODO: constant condition check ???
    return new ast_while_stmt(this->make_cast_expr(this->_the_boolean_type, cond), stmt);
}

ast_stmt* __ast_builder_impl::make_for_stmt(ast_stmt* init_stmt, ast_expr* cond, ast_stmt* each, ast_stmt* body) const noexcept {
    //TODO: constant condition check ???
    if(init_stmt->is<ast_decl_stmt>()) {
        ast_local_decl* decl = init_stmt->as<ast_decl_stmt>()->decl;
        return new ast_block_stmt(
                    new xcc::list<ast_local_decl>(decl),
                    new xcc::list<ast_stmt>(this->make_for_stmt(this->_the_nop_stmt, cond, each, body)));
    }
    else {
        return new ast_for_stmt(init_stmt, cond, each, body);
    }
}

void __ast_builder_impl::emit(ast_stmt* stmt) noexcept {
    auto ctxt = dynamic_cast<ast_block_context*>(unbox(this->context));
    assert(ctxt != nullptr);
    ctxt->emit(stmt);
}

static inline bool __sametype(const __ast_builder_impl& builder, const ast_integer_type* lhs, const ast_integer_type* rhs) {
    return (uint32_t) lhs->bitwidth    == (uint32_t) rhs->bitwidth    &&
           (bool)     lhs->is_unsigned == (bool)     rhs->is_unsigned;
}

static inline bool __sametype(const __ast_builder_impl& builder, const ast_real_type* lhs, const ast_real_type* rhs) {
    return (uint32_t) lhs->bitwidth == (uint32_t) rhs->bitwidth;
}

static inline bool __sametype(const __ast_builder_impl& builder, const ast_array_type* lhs, const ast_array_type* rhs) {
    return builder.sametype(lhs->element_type, rhs->element_type) &&
           (uint32_t) lhs->size == (uint32_t) rhs->size;
}

static inline bool __sametype(const __ast_builder_impl& builder, const ast_record_type* lhs, const ast_record_type* rhs) {
    if(lhs->field_types->size() != rhs->field_types->size()) {
        return false;
    }
    for(uint32_t i = 0; i < lhs->field_types->size(); i++) {
        if(!builder.sametype(lhs->field_types[i], rhs->field_types[i])) {
            return false;
        }
    }
    return true;
}

static inline bool __sametype(const __ast_builder_impl& builder, const ast_pointer_type* lhs, const ast_pointer_type* rhs) {
    return builder.sametype(lhs->element_type, rhs->element_type);
}

static inline bool __sametype(const __ast_builder_impl& builder, const ast_function_type* lhs, const ast_function_type* rhs) {
    if(builder.sametype(lhs->return_type, rhs->return_type)) {
        if(lhs->parameter_types->size() == rhs->parameter_types->size()) {
            for(uint32_t i = 0; i < lhs->parameter_types->size(); i++) {
                if(!builder.sametype(lhs->parameter_types[i], rhs->parameter_types[i])) {
                    return false;
                }
            }
            return true;
        }
    }
    return false;
}

bool __ast_builder_impl::sametype(ast_type* lhs, ast_type* rhs) const {
    if(lhs == rhs) { return true; }
    if(lhs->get_tree_type() == rhs->get_tree_type()) {
        switch(lhs->get_tree_type()) {
        case tree_type_id::ast_void_type:
            return true;
        case tree_type_id::ast_integer_type:
            return __sametype(*this, lhs->as<ast_integer_type>(), rhs->as<ast_integer_type>());
        case tree_type_id::ast_real_type:
            return __sametype(*this, lhs->as<ast_real_type>(), rhs->as<ast_real_type>());
        case tree_type_id::ast_pointer_type:
            return __sametype(*this, lhs->as<ast_pointer_type>(), rhs->as<ast_pointer_type>());
        case tree_type_id::ast_array_type:
            return __sametype(*this, lhs->as<ast_array_type>(), rhs->as<ast_array_type>());
        case tree_type_id::ast_record_type:
            return __sametype(*this, lhs->as<ast_record_type>(), rhs->as<ast_record_type>());
        case tree_type_id::ast_function_type:
            return __sametype(*this, lhs->as<ast_function_type>(), rhs->as<ast_function_type>());
        }
        throw std::runtime_error("unhandled type " + std::to_string((int) lhs->get_tree_type()) + " in sametype\n");
    }
    return false;
}

static ast_type* __maxtype(const __ast_builder_impl* builder, ast_integer_type* lhs, ast_type* rhs) {
    uint32_t                lhs_width       = lhs->bitwidth;
    bool                    lhs_is_unsigned = lhs->is_unsigned;

    switch(rhs->get_tree_type()) {
    case tree_type_id::ast_integer_type:
        {
            uint32_t        rhs_width       = rhs->as<ast_integer_type>()->bitwidth;
            bool            rhs_is_unsigned = rhs->as<ast_integer_type>()->is_unsigned;

            return builder->get_integer_type(
                    std::max(lhs_width, rhs_width),
                    lhs_is_unsigned & rhs_is_unsigned);
        }
    case tree_type_id::ast_real_type:
        {
            uint32_t        rhs_width       = rhs->as<ast_real_type>()->bitwidth;

            return builder->get_real_type(
                    std::max(lhs_width, rhs_width));
        }
    }
    throw std::runtime_error("unhandled type " + std::to_string((int) rhs->get_tree_type()) + " in maxtype(int)\n");
}

static ast_type* __maxtype(const __ast_builder_impl* builder, ast_real_type* lhs, ast_type* rhs) {
    uint32_t                lhs_width       = lhs->bitwidth;

    switch(rhs->get_tree_type()) {
    case tree_type_id::ast_real_type:
        {
            uint32_t        rhs_width       = rhs->as<ast_real_type>()->bitwidth;

            return builder->get_real_type(std::max(lhs_width, rhs_width));
        }
    case tree_type_id::ast_integer_type:
        {
            uint32_t        rhs_width       = rhs->as<ast_integer_type>()->bitwidth;

            return builder->get_real_type(std::max(lhs_width, rhs_width));
        }
    }
    throw std::runtime_error("unhandled type " + std::to_string((int) rhs->get_tree_type()) + " in maxtype(real)\n");
}

ast_type* __ast_builder_impl::maxtype(ast_type* lhs, ast_type* rhs) const {
    switch(lhs->get_tree_type()) {
    case tree_type_id::ast_integer_type:
        return __maxtype(this, lhs->as<ast_integer_type>(), rhs);
    case tree_type_id::ast_real_type:
        return __maxtype(this, lhs->as<ast_real_type>(), rhs);
    }
    throw std::runtime_error("unhandled type " + std::to_string((int) lhs->get_tree_type()) + " in maxtype\n");
}

ast_expr* __ast_builder_impl::cast_to(ast_integer_type* itype, ast_expr* expr) const {
    uint32_t    to_bitwidth       = itype->bitwidth;
    bool        to_is_unsigned    = itype->is_unsigned;
    switch(expr->type->get_tree_type()) {
    case tree_type_id::ast_integer_type:
        {
            uint32_t    from_bitwidth       = expr->type->as<ast_integer_type>()->bitwidth;
            bool        from_is_unsigned    = expr->type->as<ast_integer_type>()->is_unsigned;

            if(to_bitwidth < from_bitwidth) {
                return new ast_cast(itype, ast_op::trunc, expr);
            }
            else if(to_bitwidth > from_bitwidth) {
                if(to_is_unsigned) {
                    return new ast_cast(itype, ast_op::zext, expr);
                }
                else {
                    return new ast_cast(itype, ast_op::sext, expr);
                }
            }
            else {
                return new ast_cast(itype, ast_op::bitcast, expr);
            }
        }
    case tree_type_id::ast_real_type:
        {
            if(to_is_unsigned) {
                return this->cast_to(itype, this->cast_to(this->get_integer_type(to_bitwidth, false), expr));
            }

            uint32_t    from_bitwidth       = expr->type->as<ast_real_type>()->bitwidth;

            if(to_bitwidth >= from_bitwidth) {
                return new ast_cast(itype, ast_op::ftoi, expr);
            }
            else if(to_bitwidth < from_bitwidth) {
                //TODO: ???
                return new ast_cast(itype, ast_op::ftoi, expr);
            }
        }
        break;
    case tree_type_id::ast_pointer_type:
        {
            if(!to_is_unsigned) {
                return this->cast_to(this->get_integer_type(to_bitwidth, true), expr);
            }

            return new ast_cast(itype, ast_op::utop, expr);
        }
    }
    throw std::runtime_error("unhandled type " + std::to_string((int) expr->type->get_tree_type()) + " in cast_to(int)\n");
}


ast_expr* __ast_builder_impl::cast_to(ast_real_type* rtype, ast_expr* expr) const {
    uint32_t    to_bitwidth     = rtype->bitwidth;

    switch(expr->type->get_tree_type()) {
    case tree_type_id::ast_real_type:
        {
            uint32_t    from_bitwidth   = expr->type->as<ast_real_type>()->bitwidth;

            if(to_bitwidth > from_bitwidth) {
                return new ast_cast(rtype, ast_op::fext, expr);
            }
            else if(to_bitwidth < from_bitwidth) {
                return new ast_cast(rtype, ast_op::ftrunc, expr);
            }
            else {
                return expr;
            }
        }
    case tree_type_id::ast_integer_type:
        {
            uint32_t    from_bitwidth       = expr->type->as<ast_integer_type>()->bitwidth;
            bool        from_is_unsigned    = expr->type->as<ast_integer_type>()->is_unsigned;

            if(to_bitwidth >= from_bitwidth) {
                if(from_is_unsigned) {
                    return new ast_cast(rtype, ast_op::utof, expr);
                }
                else {
                    return new ast_cast(rtype, ast_op::itof, expr);
                }
            }
            else {
                return this->cast_to(rtype, this->cast_to(this->get_integer_type(to_bitwidth, from_is_unsigned), expr));
            }
        }
    }
    throw std::runtime_error("unhandled type " + std::to_string((int) expr->type->get_tree_type()) + " in cast_to(real)\n");
}

ast_expr* __ast_builder_impl::cast_to(ast_pointer_type* ptype, ast_expr* expr) const {
    ast_type*   eltype      = ptype->element_type;

    switch(expr->type->get_tree_type()) {
    case tree_type_id::ast_pointer_type:
        return new ast_cast(ptype, ast_op::bitcast, expr);
    case tree_type_id::ast_integer_type:
        {
            uint32_t    bitwidth    = expr->as<ast_integer_type>()->bitwidth;
            bool        is_unsigned = expr->as<ast_integer_type>()->is_unsigned;
            if(!is_unsigned) {
                return this->cast_to(ptype, this->cast_to(this->get_integer_type(bitwidth, true), expr));
            }
            return new ast_cast(ptype, ast_op::utop, expr);
        }
    }
    throw std::runtime_error("unhandled type " + std::string(expr->type->get_tree_type_name()) + " in __ast_builder_impl::cast_to(ptr)\n");
}

ast_expr* __ast_builder_impl::cast_to(ast_record_type* rtype, ast_expr* expr) const {
    if(this->sametype(rtype, expr->type)) {
        return expr;
    }
    throw std::runtime_error("I'm lazy\n");
}

bool __ast_builder_impl::widens(ast_type* from_type, ast_type* to_type) const {
    switch(from_type->get_tree_type()) {
    case tree_type_id::ast_integer_type:
        {
            uint32_t    bw_from = from_type->as<ast_integer_type>()->bitwidth;
            uint32_t    bw_to;
            bool        isu_from = from_type->as<ast_integer_type>()->is_unsigned;
            bool        isu_to;
            if(to_type->is<ast_integer_type>()) {
                bw_to = to_type->as<ast_integer_type>()->bitwidth;
                isu_to = to_type->as<ast_integer_type>()->is_unsigned;
            }
            else if(to_type->is<ast_real_type>()) {
                bw_to = to_type->as<ast_real_type>()->bitwidth;
                isu_to = false;
            }
            else {
                break;
            }

            if(isu_from) {
                return bw_from <= bw_to;
            }
            else {
                return (bw_from <= bw_to) && (!isu_to);
            }
        }
    case tree_type_id::ast_real_type:
        {
            if(to_type->is<ast_integer_type>()) {
                return false;
            }
            else if(to_type->is<ast_real_type>()) {
                uint32_t bw_from = from_type->as<ast_real_type>()->bitwidth;
                uint32_t bw_to   = to_type->as<ast_real_type>()->bitwidth;
                return bw_from <= bw_to;
            }
            else {
                break;
            }
        }
    }
    throw std::runtime_error("unhandled " + std::string(from_type->get_tree_type_name()) + " -> " +
                                            std::string(to_type->get_tree_type_name())   + " in __ast_builder_impl::widens\n");
}

ast_expr* __ast_builder_impl::widen(ast_type* typedest, ast_expr* expr) const {
    switch(typedest->get_tree_type()) {
    case tree_type_id::ast_integer_type:        return this->cast_to(typedest->as<ast_integer_type>(), expr);
    case tree_type_id::ast_real_type:           return this->cast_to(typedest->as<ast_real_type>(),    expr);
    case tree_type_id::ast_pointer_type:        return this->cast_to(typedest->as<ast_pointer_type>(), expr);
    }
    throw std::runtime_error("unhandled " + std::string(typedest->get_tree_type_name()) + " in __ast_builder_impl::widen\n");
}

bool __ast_builder_impl::is_ptrof(ast_type* ptype, ast_type* el) {
    return ptype->is<ast_pointer_type>() && this->sametype(ptype->as<ast_pointer_type>()->element_type, el);
}

bool __ast_builder_impl::is_arrayof(ast_type* atype, ast_type* el) {
    return atype->is<ast_array_type>() && this->sametype(atype->as<ast_array_type>()->element_type, el);
}

ptr<ast_context> __ast_builder_impl::get_context() noexcept {
    return this->context;
}

void __ast_builder_impl::set_context(ptr<ast_context> ctx) noexcept {
    this->context = ctx;
}

void __ast_builder_impl::clear_context() noexcept {
    this->context = nullptr;
}

void __ast_builder_impl::push_namespace(ast_namespace_decl* ns) noexcept {
    this->push_context<ast_namespace_context>(ns);
}

void __ast_builder_impl::push_block(ast_block_stmt* block) noexcept {
    this->push_context<ast_block_context>(block);
}

void __ast_builder_impl::pop() noexcept { this->pop_context(); }

ast_typedef_decl* __ast_builder_impl::define_named_type(const char* name, ast_type* tp) noexcept {
    return new ast_typedef_decl(name, tp);
}

ast_variable_decl* __ast_builder_impl::define_global_variable(ast_type* type, const char* name) noexcept {
    auto var = new ast_variable_decl(name, type, this->make_zero(type));
    var->is_extern = false;
    var->is_extern_visible = true;

    this->context->insert(name, var);
    this->tu.append(var);
    return var;
}

ast_type* __ast_builder_impl::get_return_type() noexcept { return this->context->get_return_type(); }

ast_decl* __ast_builder_impl::find_declaration(const char* name) noexcept {
    return this->context->find(name);
}

ptr<list<ast_decl>> __ast_builder_impl::find_all_declarations(const char* name) noexcept {
    return this->context->findall(name, true);
}

ast_variable_decl* __ast_builder_impl::define_global_variable(ast_type* type, const char* name, ast_expr* ivalue) noexcept {
    auto var = new ast_variable_decl(name, type, ivalue);

    this->context->insert(name, var);
    this->tu.append(var);
    return var;
}

ast_local_decl* __ast_builder_impl::define_local_variable(ast_type* type, const char* name) noexcept {
    auto var = new ast_local_decl(name, type, nullptr);

    this->context->insert(name, var);
    return var;
}

ast_local_decl* __ast_builder_impl::define_local_variable(ast_type* type) noexcept {
    auto var = new ast_local_decl("$annon", type, nullptr);

    this->context->insert(var->name->c_str(), var);
    return var;
}

}


