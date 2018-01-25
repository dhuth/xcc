/*
 * ast_builder.cpp
 *
 *  Created on: Jan 22, 2017
 *      Author: derick
 */

#include <string>
#include <iostream>
#include <sstream>

#include "ast_builder.hpp"
#include "frontend.hpp"
#include "error.hpp"


namespace xcc {

__ast_builder_impl::__ast_builder_impl(
        translation_unit& tu,
        ast_name_mangler* mangler,
        ast_type_comparer* type_comparer,
        ast_type_hasher* type_hasher) noexcept
            : _mangler_ptr(mangler),
              _type_comparer_ptr(type_comparer),
              _type_hasher_ptr(type_hasher),
              get_mangled_name(*mangler),
              tu(tu),
              global_namespace(new ast_namespace_decl("global")),
              _the_nop_stmt(new ast_nop_stmt()),
              _the_break_stmt(new ast_break_stmt()),
              _the_continue_stmt(new ast_continue_stmt()),
              _pointer_types(*type_hasher, *type_comparer),
              _array_types(*type_hasher, *type_comparer),
              _function_types(*type_hasher, *type_comparer),
              _record_types(*type_hasher, *type_comparer) {

    this->context = new ast_namespace_context(nullptr, this->global_namespace);
    this->create_default_types();
}

__ast_builder_impl::~__ast_builder_impl() noexcept {
    delete this->_type_comparer_ptr;
    delete this->_type_hasher_ptr;
    delete this->_mangler_ptr;
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
    return this->_pointer_types.get_new_as<ast_pointer_type>(eltype);
}

ast_array_type* __ast_builder_impl::get_array_type(ast_type* eltype, uint32_t size) noexcept {
    return this->_array_types.get_new_as<ast_array_type>(eltype, size);
}

ast_function_type* __ast_builder_impl::get_function_type(ast_type* rtype, ptr<list<ast_type>> params) noexcept {
    return this->_function_types.get_new_as<ast_function_type>(rtype, params);
}

ast_record_type* __ast_builder_impl::get_record_type(ptr<list<ast_type>> types) noexcept {
    return this->_record_types.get_new_as<ast_record_type>(types);
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
    default:
        __throw_unhandled_tree_type(__FILE__, __LINE__, decl, "__ast_builder_impl::get_declaration_type");
    }
}

ast_decl* __ast_builder_impl::make_namespace_decl(const char* name, list<ast_decl>* decls) const noexcept {
    return new ast_namespace_decl(name, decls);
}

ast_decl* __ast_builder_impl::make_local_decl(const char* name, ast_type* type, ast_expr* expr) const noexcept {
    return new ast_local_decl(name, type, expr);
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
    default:
        __throw_unhandled_ast_type(__FILE__, __LINE__, tp, "__ast_builder_impl::make_zero");
    }
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
    default:
        __throw_unhandled_ast_type(__FILE__, __LINE__, desttype, "__ast_builder_impl::make_zero");
    }
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
        __throw_unhandled_operator(__FILE__, __LINE__, op, "__ast_builder_impl::make_lower_op_expr(binary)");
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
    default:
        __throw_unhandled_operator(__FILE__, __LINE__, op, "__ast_builder_impl::make_op_expr(unary)");
    }
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

ast_stmt* __ast_builder_impl::make_block_stmt(list<ast_stmt>* stmts) const noexcept {
    return new ast_block_stmt(new list<ast_local_decl>(), stmts);
}

ast_stmt* __ast_builder_impl::make_block_stmt(ast_local_decl* decl, list<ast_stmt>* stmts) const noexcept {
    return new ast_block_stmt(new list<ast_local_decl>(decl), stmts);
}

ast_stmt* __ast_builder_impl::make_block_stmt(list<ast_local_decl>* decls, list<ast_stmt>* stmts) const noexcept {
    return new ast_block_stmt(decls, stmts);
}

ast_stmt* __ast_builder_impl::make_break_stmt() const noexcept {
    return unbox(this->_the_break_stmt);
}

ast_stmt* __ast_builder_impl::make_continue_stmt() const noexcept {
    return unbox(this->_the_continue_stmt);
}

ast_stmt* __ast_builder_impl::make_return_stmt(ast_type* t, ast_expr* expr) const noexcept {
    if(!t->is<ast_void_type>()) {
        return new ast_return_stmt(this->make_cast_expr(t, expr));
    }
    else {
        return new ast_return_stmt(nullptr);
    }
}

ast_stmt* __ast_builder_impl::make_if_stmt(ast_expr* cond, ast_stmt* tstmt, ast_stmt* fstmt) const noexcept {
    return new ast_if_stmt(this->make_cast_expr(this->_the_boolean_type, cond), tstmt, fstmt);
}

ast_stmt* __ast_builder_impl::make_while_stmt(ast_expr* cond, ast_stmt* stmt) const noexcept {
    return new ast_while_stmt(this->make_cast_expr(this->_the_boolean_type, cond), stmt);
}

ast_stmt* __ast_builder_impl::make_for_stmt(ast_stmt* init_stmt, ast_expr* cond, ast_stmt* each, ast_stmt* body) const noexcept {
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

bool __ast_builder_impl::sametype(ast_type* lhs, ast_type* rhs) const {
    return this->_type_comparer_ptr->operator()(lhs, rhs);
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
    default:
        __throw_unhandled_ast_type(__FILE__, __LINE__, rhs, "__maxtype(int)");
    }
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
    default:
        __throw_unhandled_ast_type(__FILE__, __LINE__, rhs, "__maxtype(real)");
    }
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
    default:
        __throw_unhandled_ast_type(__FILE__, __LINE__, expr, "__ast_builder_impl::cast_to(int)");
    }
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
    default:
        __throw_unhandled_ast_type(__FILE__, __LINE__, expr, "__ast_builder_impl::cast_to(real)");
    }
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
    default:
        __throw_unhandled_ast_type(__FILE__, __LINE__, expr, "__ast_builder_impl::cast_to(pointer)");
    }
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
    default:
        __throw_unhandled_ast_type(__FILE__, __LINE__, typedest, "__ast_builder_impl::widen");
    }
}

void __ast_builder_impl::push_namespace(ast_namespace_decl* ns) noexcept {
    this->push_context<ast_namespace_context>(ns);
}

void __ast_builder_impl::push_function(ast_function_decl* func) noexcept {
    this->push_context<ast_function_context>(func);
}

void __ast_builder_impl::push_block(ast_block_stmt* block) noexcept {
    this->push_context<ast_block_context>(block);
}

void __ast_builder_impl::insert_at_global_scope(ast_decl* decl) noexcept {
    this->global_namespace->declarations->append(decl);
}

ast_decl* __ast_builder_impl::find_declaration(ast_context* context, const char* name) const noexcept {
    return context->find(name);
}

ast_decl* __ast_builder_impl::find_declaration(const char* name) noexcept {
    return this->find_declaration(this->context, name);
}

ptr<list<ast_decl>> __ast_builder_impl::find_all_declarations(ast_context* context, const char* name) const noexcept {
    return context->findall(name, true);
}

ptr<list<ast_decl>> __ast_builder_impl::find_all_declarations(const char* name) noexcept {
    return this->find_all_declarations(this->context, name);
}

}


