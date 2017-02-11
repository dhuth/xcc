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

namespace xcc {

std::string ast_default_name_mangler::operator()(ast_tree* t)                     { return this->visit(t); }
std::string ast_default_name_mangler::operator()(std::string prefix, ast_tree* t) { return prefix + this->visit(t); }

std::string ast_default_name_mangler::mangle_variable(ast_variable_decl* decl) {
    return decl->name;
}
std::string ast_default_name_mangler::mangle_function(ast_function_decl* decl) {
    std::stringstream s;
    s << (std::string) decl->name << "F$";
    for(auto param : decl->parameters) {
        s << (std::string) this->visit(param->type) << ".";
    }
    s << "$";
    return s.str();
}
std::string ast_default_name_mangler::mangle_record(ast_record_decl* decl) {
    return decl->name;
}
std::string ast_default_name_mangler::mangle_record_member(ast_record_member_decl* decl) {
    return (std::string) decl->parent->as<ast_record_decl>()->name + "." + (std::string) decl->name;
}
std::string ast_default_name_mangler::mangle_void_type(ast_void_type*) {
    return "void";
}
std::string ast_default_name_mangler::mangle_integer_type(ast_integer_type* tp) {
    if(tp->is_unsigned) {
        return "u" + std::to_string((uint32_t) tp->bitwidth);
    }
    else {
        return "i" + std::to_string((uint32_t) tp->bitwidth);
    }
}
std::string ast_default_name_mangler::mangle_real_type(ast_real_type* tp) {
    return "f" + std::to_string((uint32_t) tp->bitwidth);
}
std::string ast_default_name_mangler::mangle_array_type(ast_array_type* tp) {
    return this->visit(tp->element_type) + "a$" + std::to_string((uint32_t) tp->size) + ".$";
}
std::string ast_default_name_mangler::mangle_pointer_type(ast_pointer_type* tp) {
    return this->visit(tp->element_type) + "p";
}
std::string ast_default_name_mangler::mangle_function_type(ast_function_type* tp) {
    std::stringstream s;
    s << this->visit(tp->return_type) << "f$";
    for(auto param_tp : tp->parameter_types) {
        s << this->visit(param_tp) << ".";
    }
    s << "$";
    return s.str();
}
std::string ast_default_name_mangler::mangle_record_type(ast_record_type* tp) {
    return (std::string) tp->declaration->name + "r$";
}




__ast_builder_impl::__ast_builder_impl(ast_name_mangler_t mangler) noexcept
            : get_mangled_name(mangler),
              _pointer_types(0, std::hash<ast_type*>(), sametype_predicate(*this)),
              _function_types(0, functype_hasher(*this), samefunctype_predicate(*this)),
              _the_nop_stmt(new ast_nop_stmt()),
              _the_break_stmt(new ast_break_stmt()),
              _the_continue_stmt(new ast_continue_stmt()) {
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

ast_record_type* __ast_builder_impl::get_record_type(ast_record_decl* decl) noexcept {
    auto key = decl;
    if(this->_record_types.find(key) == this->_record_types.end()) {
        this->_record_types[key] = new ast_record_type(decl);
    }
    return this->_record_types[key];
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
            list<ast_type>          plist(pvec);
            return this->get_function_type(rtype, &plist);
        }
    case tree_type_id::ast_record_decl:
        return this->get_record_type(decl->as<ast_record_decl>());
    case tree_type_id::ast_record_member_decl:
        return decl->as<ast_record_member_decl>()->type;
    }
    //TODO: error unhandled
    return nullptr;
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

    return new ast_integer(this->get_integer_type(bitwidth ,true), value);
}

ast_expr* __ast_builder_impl::make_real(const char* txt) const noexcept {
    llvm::APFloat value(std::atof(txt));

    return new ast_real(this->get_real_type(64), value);
}

ast_expr* __ast_builder_impl::make_zero(ast_type* tp) const noexcept {
    switch(tp->get_tree_type()) {
    case tree_type_id::ast_integer_type:        return new ast_integer(tp, llvm::APSInt::get(0));
    case tree_type_id::ast_real_type:           return new ast_real(tp,    llvm::APFloat(0.0));
    }
    throw std::runtime_error("unhandled ast type " + std::to_string((uint32_t) tp->get_tree_type()));
}

ast_expr* __ast_builder_impl::make_cast_expr(ast_type* desttype, ast_expr* expr) const {
    switch(desttype->get_tree_type()) {
    case tree_type_id::ast_integer_type:            return this->cast_to(desttype->as<ast_integer_type>(), expr);
    case tree_type_id::ast_real_type:               return this->cast_to(desttype->as<ast_real_type>(),    expr);
    case tree_type_id::ast_pointer_type:            return this->cast_to(desttype->as<ast_pointer_type>(), expr);
    }
    throw std::runtime_error("unhandled ast type " + std::to_string((uint32_t) desttype->get_tree_type()));
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
            case ast_op::ge:    new_op  = ast_op::icmp_ult; break;
            }
        }
        else {
            switch(op) {
            case ast_op::eq:    new_op  = ast_op::cmp_eq;   break;
            case ast_op::ne:    new_op  = ast_op::cmp_ne;   break;
            case ast_op::lt:    new_op  = ast_op::icmp_ult; break;
            case ast_op::le:    new_op  = ast_op::icmp_ule; break;
            case ast_op::gt:    new_op  = ast_op::icmp_ugt; break;
            case ast_op::ge:    new_op  = ast_op::icmp_uge; break;
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

ast_expr* __ast_builder_impl::make_op_expr(ast_op op, ast_expr* lhs, ast_expr* rhs) {
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

    // Shift
    case ast_op::shl:
    case ast_op::shr:

    default:
        throw std::runtime_error("unhandled operator in __ast_builder_impl::make_op_expr(binary)");
    }
}

ast_expr* __ast_builder_impl::make_op_expr(ast_op op, ast_expr* expr) {
    throw std::runtime_error("unhandled operator in __ast_builder_imppl::make_op_expr(unary)");
}

ast_expr* __ast_builder_impl::make_declref_expr(ast_decl* decl) {
    return new ast_declref(this->get_declaration_type(decl), decl);
}

ast_expr* __ast_builder_impl::make_memberref_expr(ast_expr* obj, ast_record_member_decl* member) {
    return new ast_memberref(this->get_declaration_type(member), obj, member);
}

ast_expr* __ast_builder_impl::make_deref_expr(ast_expr* e) const {
    assert(e->type->is<ast_pointer_type>());

    ast_type* et = e->type->as<ast_pointer_type>()->element_type;
    return new ast_deref(et, e);
}

ast_expr* __ast_builder_impl::make_addressof_expr(ast_expr* e) {
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
    assert(fexpr->type->is<ast_function_type>());

    ast_type* t = fexpr->type->as<ast_function_type>()->return_type;
    return new ast_invoke(t, fexpr, args);
}

uint32_t __ast_builder_impl::foldu32(ast_expr* e) const {
    switch(e->get_tree_type()) {
    case tree_type_id::ast_integer:
        {
            llvm::APSInt value = e->as<ast_integer>()->value;
            return std::atoi(value.toString(10).c_str());
        }
    }
    throw std::runtime_error("unhandled expr " + std::to_string((int) e->get_tree_type()) + " in foldu32\n");
}

ast_stmt* __ast_builder_impl::make_nop_stmt() const noexcept {
    return this->_the_nop_stmt;
}

ast_stmt* __ast_builder_impl::make_expr_stmt(ast_expr* e) const noexcept {
    return new ast_expr_stmt(e);
}

ast_stmt* __ast_builder_impl::make_assign_stmt(ast_expr* lhs, ast_expr* rhs) noexcept {
    auto dest = this->make_addressof_expr(lhs);
    auto src  = this->make_cast_expr(lhs->type, rhs);

    return new ast_assign_stmt(dest, src);
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

static inline bool __sametype(const __ast_builder_impl& builder, const ast_pointer_type* lhs, const ast_pointer_type* rhs) {
    return builder.sametype(lhs->element_type, rhs->element_type);
}

static inline bool __sametype(const __ast_builder_impl& builder, const ast_function_type* lhs, const ast_function_type* rhs) {
    if(builder.sametype(lhs->return_type, rhs->return_type)) {
        if(lhs->parameter_types->size() == rhs->parameter_types->size()) {
            for(uint32_t i = 0; i < lhs->parameter_types->size(); i++) {
                if(!builder.sametype((*lhs->parameter_types)[i], (*rhs->parameter_types)[i])) {
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

ast_expr* __ast_builder_impl::widen(ast_type* typedest, ast_expr* expr) const {
    switch(typedest->get_tree_type()) {
    case tree_type_id::ast_integer_type:        return this->cast_to(typedest->as<ast_integer_type>(), expr);
    case tree_type_id::ast_real_type:           return this->cast_to(typedest->as<ast_real_type>(),    expr);
    case tree_type_id::ast_pointer_type:        return this->cast_to(typedest->as<ast_pointer_type>(), expr);
    }
    throw std::runtime_error("unhandled " + std::string(typedest->get_tree_type_name()) + " in __ast_builder_impl::widen\n");
}

}


