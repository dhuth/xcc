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
          _xi_builder((xi_builder&)b){
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
    __throw_unhandled_ast_type(__FILE__, __LINE__, rhs, "SAMETYPE_FUNC(xi_decl)");
}

/* ================= *
 * = Cast Function = *
 * ================= */

xi_cast_func::xi_cast_func(const ast_builder_impl_t& b) noexcept
        : ast_cast_func(b),
          _xi_builder((xi_builder&) b) {
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
    __throw_unhandled_ast_type(__FILE__, __LINE__, fe->type, "CAST_FUNC(xi_const)");
}

/* --------------------------- *
 * Rules for cast to reference *
 * --------------------------- */

XI_CAST_FUNC(xi_reference, tt, fe) {
    __throw_unhandled_ast_type(__FILE__, __LINE__, fe->type, "CAST_FUNC(xi_reference)");
}

/* ----------------------- *
 * Rules for cast to tuple *
 * ----------------------- */

XI_CAST_FUNC(xi_tuple, tt, fe) {
    __throw_unhandled_ast_type(__FILE__, __LINE__, fe->type, "CAST_FUNC(xi_tuple)");
}

/* ---------------------- *
 * Rules for cast to decl *
 * ---------------------- */

XI_CAST_FUNC(xi_decl, tt, fe) {
    __throw_unhandled_ast_type(__FILE__, __LINE__, fe->type, "CAST_FUNC(xi_decl)");
}

/* ------------------------- *
 * Rules for cast to pointer *
 * ------------------------- */

XI_CAST_FUNC(ast_pointer, tt, fe) {
    ast_type*       t_elt = tt->element_type;

    // Cast from another pointer type
    // ------------------------------
    if(is_pointer_type(fe->type)) {
        ast_type*   f_elt = fe->type->as<ast_pointer_type>()->element_type;
        if(_builder.sametype(t_elt, f_elt)) return fe;  // same type
        else                                return bitcast_to(tt, fe);
    }

    // Cast from an integer type
    // -------------------------
    if(is_integer_type(fe->type)) {
        auto            ft = fe->type->as<ast_integer_type>();
        uint32_t        fw = ft->bitwidth;
        bool            fu = ft->is_unsigned;
        if(!fu) {
            return this->visit(tt,
                    this->visit(_builder.get_integer_type(fw, true), fe));
        }
        else {
            return new ast_cast(tt, ast_op::utop, fe);
        }
    }

    // Cast from an array type
    // -----------------------
    if(is_array_type(fe->type)) {
        //TODO: check that array expression is addressable
        auto            ft      = fe->type->as<ast_array_type>();
        ast_type*       f_elt   = ft->element_type;

        return new ast_addressof(tt,
                _builder.make_index_expr(fe, _builder.make_zero(_builder.get_size_type())));
    }

    __throw_unhandled_ast_type(__FILE__, __LINE__, fe->type, "CAST_FUNC(ast_pointer)");
}

/* =================== *
 * = Widens Function = *
 * =================== */

xi_widens_func::xi_widens_func(const ast_builder_impl_t& b) noexcept
        : ast_widens_func(b),
          _xi_builder((xi_builder&) b) {
    this->addmethod(&XI_WIDENS_METHOD_NAME(xi_const));
    this->addmethod(&XI_WIDENS_METHOD_NAME(xi_reference));
    this->addmethod(&XI_WIDENS_METHOD_NAME(xi_tuple));
    this->addmethod(&XI_WIDENS_METHOD_NAME(xi_decl));
    this->addmethod(&XI_WIDENS_METHOD_NAME(ast_pointer));
}

#define WIDENS_RETURN_FALSE             return false;
#define WIDENS_RETURN_NOWORK            return true;
#define WIDENS_RETURN_WORK(c, n)        { c += n; return true; }

/* ------------------------- *
 * Rules for widens to const *
 * ------------------------- */

XI_WIDENS_FUNC(xi_const, tt, fe, cost) {
    __throw_unhandled_ast_type(__FILE__, __LINE__, fe->type, "WIDENS_FUNC(xi_const)");
}

/* ----------------------------- *
 * Rules for widens to reference *
 * ----------------------------- */

XI_WIDENS_FUNC(xi_reference, tt, fe, cost) {
    __throw_unhandled_ast_type(__FILE__, __LINE__, fe->type, "WIDENS_FUNC(xi_reference)");
}

/* ------------------------- *
 * Rules for widens to tuple *
 * ------------------------- */

XI_WIDENS_FUNC(xi_tuple, tt, fe, cost) {
    __throw_unhandled_ast_type(__FILE__, __LINE__, fe->type, "WIDENS_FUNC(xi_tuple)");
}

/* ------------------------ *
 * Rules for widens to decl *
 * ------------------------ */

XI_WIDENS_FUNC(xi_decl, tt, fe, cost) {
    __throw_unhandled_ast_type(__FILE__, __LINE__, fe->type, "WIDENS_FUNC(xi_decl)");
}

/* --------------------------- *
 * Rules for widens to pointer *
 * --------------------------- */

XI_WIDENS_FUNC(ast_pointer, tt, fe, cost) {
    ast_type* ft = fe->type;

    // from another pointer type
    // -------------------------
    if(is_pointer_type(ft)) {
        ast_type* tpel      = tt->element_type;
        ast_type* fpel      = ft->as<ast_pointer_type>()->element_type;

        if(_builder.sametype(tpel, fpel)) {
            WIDENS_RETURN_NOWORK
        }
        else if(is_void_type(tpel)) {
            WIDENS_RETURN_WORK(cost, 4)
        }
    }

    // from an array type
    // ------------------
    if(is_array_type(ft)) {
        //TODO: check that expression is addressable
        ast_type* tp_el     = tt->element_type;
        ast_type* fa_el     = ft->as<ast_array_type>()->element_type;

        if(_builder.sametype(tp_el, fa_el)) {
            WIDENS_RETURN_WORK(cost, 1)
        }
        else {
            WIDENS_RETURN_FALSE
        }
    }
    WIDENS_RETURN_FALSE
}

}


