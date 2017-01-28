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
              _context(nullptr) {
    this->create_default_types();
}

void __ast_builder_impl::create_default_types() noexcept {

    this->_the_void_type = new ast_void_type();
    this->_the_void_ptr_type = this->get_pointer_type(this->_the_void_type);
    this->_the_boolean_type = this->get_integer_type(1, true);

    for(uint32_t size = 1; size <= 64; size *= 2) {
        this->_signed_integer_types[size] = new ast_integer_type(size, false);
        this->_unsigned_integer_types[size] = new ast_integer_type(size, true);
    }

    this->_real_types[32] = new ast_real_type(32);
    this->_real_types[64] = new ast_real_type(64);
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

ast_function_type* __ast_builder_impl::get_function_type(ast_type* rtype, list<ast_type>* params) noexcept {
    ptr<list<ast_type>> p(params);
    return this->get_function_type(rtype, p);
}

ast_function_type* __ast_builder_impl::get_function_type(ast_type* rtype, ptr<list<ast_type>> params) noexcept {
    auto key = funckey_t(rtype, __unbox(params));
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


ast_expr* __ast_builder_impl::make_add_expr(ast_expr* lhs, ast_expr* rhs) const noexcept {
    auto t = this->maxtype(lhs->type, rhs->type);

    switch(t->get_tree_type()) {
    case tree_type_id::ast_integer_type:
        return new ast_binary_op(t, ast_op::add, this->widen(t, lhs), this->widen(t, rhs));
    case tree_type_id::ast_real_type:
        return new ast_binary_op(t, ast_op::fadd, this->widen(t, lhs), this->widen(t, rhs));
    }
    //TODO: error?
    return nullptr;
}

ast_expr* __ast_builder_impl::make_sub_expr(ast_expr* lhs, ast_expr* rhs) const noexcept {
    auto t = this->maxtype(lhs->type, rhs->type);

    switch(t->get_tree_type()) {
    case tree_type_id::ast_integer_type:
        return new ast_binary_op(t, ast_op::sub, this->widen(t, lhs), this->widen(t, rhs));
    case tree_type_id::ast_real_type:
        return new ast_binary_op(t, ast_op::fsub, this->widen(t, lhs), this->widen(t, rhs));
    }
    //TODO: error?
    return nullptr;
}

ast_expr* __ast_builder_impl::make_mul_expr(ast_expr* lhs, ast_expr* rhs) const noexcept {
    auto t = this->maxtype(lhs->type, rhs->type);

    switch(t->get_tree_type()) {
    case tree_type_id::ast_integer_type:
        return new ast_binary_op(t, ast_op::mul, this->widen(t, lhs), this->widen(t, rhs));
    case tree_type_id::ast_real_type:
        return new ast_binary_op(t, ast_op::fmul, this->widen(t, lhs), this->widen(t, rhs));
    }
    //TODO: error?
    return nullptr;
}

ast_expr* __ast_builder_impl::make_div_expr(ast_expr* lhs, ast_expr* rhs) const noexcept {
    auto t = this->maxtype(lhs->type, rhs->type);

    switch(t->get_tree_type()) {
    case tree_type_id::ast_integer_type:
        if((bool) t->as<ast_integer_type>()->is_unsigned) {
            return new ast_binary_op(t, ast_op::udiv, this->widen(t, lhs), this->widen(t, rhs));
        }
        else {
            return new ast_binary_op(t, ast_op::idiv, this->widen(t, lhs), this->widen(t, rhs));
        }
    case tree_type_id::ast_real_type:
        return new ast_binary_op(t, ast_op::fdiv, this->widen(t, lhs), this->widen(t, rhs));
    }
    //TODO: error?
    return nullptr;
}

ast_expr* __ast_builder_impl::make_mod_expr(ast_expr* lhs, ast_expr* rhs) const noexcept {
    auto t = this->maxtype(lhs->type, rhs->type);

    switch(t->get_tree_type()) {
    case tree_type_id::ast_integer_type:
        if((bool) t->as<ast_integer_type>()->is_unsigned) {
            return new ast_binary_op(t, ast_op::umod, this->widen(t, lhs), this->widen(t, rhs));
        }
        else {
            return new ast_binary_op(t, ast_op::imod, this->widen(t, lhs), this->widen(t, rhs));
        }
    case tree_type_id::ast_real_type:
        return new ast_binary_op(t, ast_op::fmod, this->widen(t, lhs), this->widen(t, rhs));
    }
    //TODO: error?
    return nullptr;
}

ast_expr* __ast_builder_impl::make_neg_expr(ast_expr* e) const noexcept {
    if(e->type->is<ast_integer_type>()) {
        return new ast_unary_op(e->type, ast_op::ineg, e);
    }
    else if(e->type->is<ast_real_type>()) {
        return new ast_unary_op(e->type, ast_op::fneg, e);
    }
    //TODO: error?
    return nullptr;
}

ast_expr* __ast_builder_impl::make_eq_expr(ast_expr* lhs, ast_expr* rhs) const noexcept {
    auto t      = this->maxtype(lhs->type, rhs->type);
    auto lexpr  = this->widen(t, lhs);
    auto rexpr  = this->widen(t, rhs);

    if(t->is<ast_real_type>()) {
        return new ast_binary_op(this->_the_boolean_type, ast_op::fcmp_oeq, rexpr, lexpr);
    }
    else if(t->is<ast_integer_type>() || t->is<ast_pointer_type>()) {
        return new ast_binary_op(this->_the_boolean_type, ast_op::cmp_eq, rexpr, lexpr);
    }
    //TODO: error?
    return nullptr;
}

ast_expr* __ast_builder_impl::make_ne_expr(ast_expr* lhs, ast_expr* rhs) const noexcept {
    auto t      = this->maxtype(lhs->type, rhs->type);
    auto rexpr  = this->widen(t, lhs);
    auto lexpr  = this->widen(t, rhs);

    if(t->is<ast_real_type>()) {
        return new ast_binary_op(this->_the_boolean_type, ast_op::fcmp_one, rexpr, lexpr);
    }
    else if(t->is<ast_integer_type>() || t->is<ast_pointer_type>()) {
        return new ast_binary_op(this->_the_boolean_type, ast_op::cmp_ne, rexpr, lexpr);
    }
    //TODO: error?
    return nullptr;
}

ast_expr* __ast_builder_impl::make_lt_expr(ast_expr* lhs, ast_expr* rhs) const noexcept {
    auto t      = this->maxtype(lhs->type, rhs->type);
    auto rexpr  = this->widen(t, lhs);
    auto lexpr  = this->widen(t, rhs);

    switch(t->get_tree_type()) {
    case tree_type_id::ast_real_type:
        return new ast_binary_op(this->_the_boolean_type, ast_op::fcmp_olt, rexpr, lexpr);
    case tree_type_id::ast_integer_type:
        if((bool) t->as<ast_integer_type>()->is_unsigned) {
            return new ast_binary_op(this->_the_boolean_type, ast_op::icmp_ult, rexpr, lexpr);
        }
        else {
            return new ast_binary_op(this->_the_boolean_type, ast_op::icmp_slt, rexpr, lexpr);
        }
    case tree_type_id::ast_pointer_type:
        return new ast_binary_op(this->_the_boolean_type, ast_op::icmp_ult, rexpr, lexpr);
    }
    //TODO: error?
    return nullptr;
}

ast_expr* __ast_builder_impl::make_le_expr(ast_expr* lhs, ast_expr* rhs) const noexcept {
    auto t      = this->maxtype(lhs->type, rhs->type);
    auto rexpr  = this->widen(t, lhs);
    auto lexpr  = this->widen(t, rhs);

    switch(t->get_tree_type()) {
    case tree_type_id::ast_real_type:
        return new ast_binary_op(this->_the_boolean_type, ast_op::fcmp_ole, rexpr, lexpr);
    case tree_type_id::ast_integer_type:
        if((bool) t->as<ast_integer_type>()->is_unsigned) {
            return new ast_binary_op(this->_the_boolean_type, ast_op::icmp_ule, rexpr, lexpr);
        }
        else {
            return new ast_binary_op(this->_the_boolean_type, ast_op::icmp_sle, rexpr, lexpr);
        }
    case tree_type_id::ast_pointer_type:
        return new ast_binary_op(this->_the_boolean_type, ast_op::icmp_ule, rexpr, lexpr);
    }
    //TODO: error?
    return nullptr;
}

ast_expr* __ast_builder_impl::make_gt_expr(ast_expr* lhs, ast_expr* rhs) const noexcept {
    auto t      = this->maxtype(lhs->type, rhs->type);
    auto rexpr  = this->widen(t, lhs);
    auto lexpr  = this->widen(t, rhs);

    switch(t->get_tree_type()) {
    case tree_type_id::ast_real_type:
        return new ast_binary_op(this->_the_boolean_type, ast_op::fcmp_ogt, rexpr, lexpr);
    case tree_type_id::ast_integer_type:
        if((bool) t->as<ast_integer_type>()->is_unsigned) {
            return new ast_binary_op(this->_the_boolean_type, ast_op::icmp_ugt, rexpr, lexpr);
        }
        else {
            return new ast_binary_op(this->_the_boolean_type, ast_op::icmp_sgt, rexpr, lexpr);
        }
    case tree_type_id::ast_pointer_type:
        return new ast_binary_op(this->_the_boolean_type, ast_op::icmp_ugt, rexpr, lexpr);
    }
    //TODO: error?
    return nullptr;
}

ast_expr* __ast_builder_impl::make_ge_expr(ast_expr* lhs, ast_expr* rhs) const noexcept {
    auto t      = this->maxtype(lhs->type, rhs->type);
    auto rexpr  = this->widen(t, lhs);
    auto lexpr  = this->widen(t, rhs);

    switch(t->get_tree_type()) {
    case tree_type_id::ast_real_type:
        return new ast_binary_op(this->_the_boolean_type, ast_op::fcmp_oge, rexpr, lexpr);
    case tree_type_id::ast_integer_type:
        if((bool) t->as<ast_integer_type>()->is_unsigned) {
            return new ast_binary_op(this->_the_boolean_type, ast_op::icmp_uge, rexpr, lexpr);
        }
        else {
            return new ast_binary_op(this->_the_boolean_type, ast_op::icmp_sge, rexpr, lexpr);
        }
    case tree_type_id::ast_pointer_type:
        return new ast_binary_op(this->_the_boolean_type, ast_op::icmp_uge, rexpr, lexpr);
    }
    //TODO: error?
    return nullptr;
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
            for(int i = 0; i < lhs->parameter_types->size(); i++) {
                if(!builder.sametype((*lhs->parameter_types)[i], (*rhs->parameter_types)[i])) {
                    return false;
                }
            }
            return true;
        }
    }
    return false;
}

bool __ast_builder_impl::sametype(ast_type* lhs, ast_type* rhs) const noexcept {
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
        case tree_type_id::ast_record_type:
            //TODO:
            break;
        }
    }
    return false;
}

ast_expr* __ast_builder_impl::cast_to(ast_integer_type* itype, ast_expr* expr) const noexcept {
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
                return this->cast_to(this->get_integer_type(to_bitwidth, false), expr);
            }

            uint32_t    from_bitwidth       = expr->type->as<ast_real_type>()->bitwidth;

            if(to_bitwidth >= from_bitwidth) {
                return new ast_cast(itype, ast_op::ftoi, expr);
            }
            else if(to_bitwidth < from_bitwidth) {
                //TODO: error
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
    //TODO: handle error
    return nullptr;
}


ast_expr* __ast_builder_impl::cast_to(ast_real_type* rtype, ast_expr* expr) const noexcept {
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
                return this->cast_to(this->get_integer_type(to_bitwidth, from_is_unsigned), expr);
            }
        }
    }
    //TODO: handle error
    return nullptr;
}

ast_expr* __ast_builder_impl::cast_to(ast_pointer_type* ptype, ast_expr* expr) const noexcept {
    ast_type*   eltype      = ptype->element_type;

    switch(expr->get_tree_type()) {
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
    //TODO: handle error
    return nullptr;
}

}


