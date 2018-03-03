/*
 * xi_type_func.cpp
 *
 *  Created on: Mar 1, 2018
 *      Author: derick
 */

#include "xi_type_func.hpp"
#include "xi_builder.hpp"
#include "error.hpp"

namespace xcc {

/* ===================== *
 * = Sametype Function = *
 * ===================== */

xi_sametype_func::xi_sametype_func(const ast_builder_impl_t& b) noexcept
        : ast_sametype_func(b),
          _xi_builder((const xi_builder&)b){
    this->addmethod(&XI_SAMETYPE_METHOD_NAME(xi_auto));
    this->addmethod(&XI_SAMETYPE_METHOD_NAME(xi_const));
    this->addmethod(&XI_SAMETYPE_METHOD_NAME(xi_reference));
    this->addmethod(&XI_SAMETYPE_METHOD_NAME(xi_tuple));
    this->addmethod(&XI_SAMETYPE_METHOD_NAME(xi_decl));
}

/* ----------------- *
 * Sametype for auto *
 * ----------------- */

XI_SAMETYPE_FUNC(xi_auto, lhs, rhs) {
    if(is_auto_type(rhs)) {
        return true;
    }
    return false;
}

/* ------------------ *
 * Sametype for const *
 * ------------------ */

XI_SAMETYPE_FUNC(xi_const, lhs, rhs) {
    if(is_const_type(rhs)) {
        return this->visit(lhs->type, rhs->as<xi_const_type>()->type);
    }
    return false;
}

/* --------------------------- *
 * Sametype for reference type *
 * --------------------------- */

XI_SAMETYPE_FUNC(xi_reference, lhs, rhs) {
    if(is_reference_type(rhs)) {
        return this->visit(lhs->type, rhs->as<xi_reference_type>()->type);
    }
    return false;
}

/* ----------------------- *
 * Sametype for tuple type *
 * ----------------------- */

XI_SAMETYPE_FUNC(xi_tuple, lhs, rhs) {
    if(is_tuple_type(rhs)) {
        return this->sametype_list(lhs->types, rhs->as<xi_tuple_type>()->types);
    }
    return false;
}

/* ---------------------- *
 * Sametype for decl type *
 * ---------------------- */

XI_SAMETYPE_FUNC(xi_decl, lhs, rhs) {
    if(is_decl_type(rhs)) {
        return lhs->declaration == rhs->as<xi_decl_type>()->declaration;
    }
    return false;
}

/* ================= *
 * = Cast Function = *
 * ================= */

xi_cast_func::xi_cast_func(const ast_builder_impl_t& b) noexcept
        : ast_cast_func(b),
          _xi_builder((const xi_builder&) b) {
    this->addmethod(&XI_CAST_METHOD_NAME(xi_const));
    this->addmethod(&XI_CAST_METHOD_NAME(xi_reference));
    this->addmethod(&XI_CAST_METHOD_NAME(xi_tuple));
    this->addmethod(&XI_CAST_METHOD_NAME(xi_decl));
    this->addmethod(&XI_CAST_METHOD_NAME(ast_pointer));
}

static inline ast_expr* bitcast_to(ast_type* tt, ast_expr* fe) noexcept {
    auto ce = new ast_cast(tt, ast_op::bitcast, fe);
    ce->value_type = fe->value_type;
    return ce;
}

/* ----------------------- *
 * Rules for cast to const *
 * ----------------------- */

XI_CAST_FUNC(xi_const, tt, fe) {
    ast_type*       te = tt->type;

    // Cast from another const type
    // ----------------------------
    if(is_const_type(fe->type)) {
        auto        ft = fe->type->as<xi_const_type>();
        if(_builder.sametype(te, ft)) {
            return fe;                                      // SAME TYPE
        }
        else {
            return bitcast_to(tt, this->visit(te, bitcast_to(ft, fe)));
        }
    }

    // Cast from non-const same type
    // -----------------------------
    if(_builder.sametype(te, fe->type)) {
        return bitcast_to(tt, fe);
    }

    __throw_unhandled_ast_type(__FILE__, __LINE__, fe->type, "CAST_FUNC(xi_const)");
}

/* --------------------------- *
 * Rules for cast to reference *
 * --------------------------- */

XI_CAST_FUNC(xi_reference, tt, fe) {
    ast_type*       te_t = tt->type;

    // Cast from another reference type
    // --------------------------------
    if(is_reference_type(fe->type)) {
        auto        ft   = fe->type->as<xi_reference_type>();
        ast_type*   fe_t = ft->type;
        if(_builder.sametype(te_t, fe_t)) {
            return fe;                                      // SAME TYPE
        }
        else {
            //TODO: pointer type conversion
        }
    }

    __throw_unhandled_ast_type(__FILE__, __LINE__, fe->type, "CAST_FUNC(xi_reference)");
}

/* ----------------------- *
 * Rules for cast to tuple *
 * ----------------------- */

/* ---------------------- *
 * Rules for cast to decl *
 * ---------------------- */

/* ------------------------- *
 * Rules for cast to pointer *
 * ------------------------- */

XI_CAST_FUNC(ast_pointer, tt, fe) {
    ast_type*       te_t = tt->element_type;

    // Cast from another pointer type
    // ------------------------------
    if(is_pointer_type(fe->type)) {
        auto        ft      = fe->as<ast_pointer_type>();
        ast_type*   fe_t    = ft->element_type;

        if(_builder.sametype(te_t, fe_t)) {
            return fe;                                      // SAME TYPE
        }
        else {
            //TODO: pointer type conversion
        }
    }

    __throw_unhandled_ast_type(__FILE__, __LINE__, fe->type, "CAST_FUNC(ast_pointer) xi version");
}

}


