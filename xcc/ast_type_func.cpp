/*
 * ast_type_widen.cpp
 *
 *  Created on: Feb 28, 2018
 *      Author: derick
 */

#include "ast_type_func.hpp"
#include "error.hpp"

namespace xcc {



/* ===================== *
 * = Sametype Function = *
 * ===================== */

ast_sametype_func::ast_sametype_func(const ast_builder_impl_t& builder) noexcept
        : _builder(builder) {
    this->addmethod(&SAMETYPE_METHOD_NAME(ast_void));
    this->addmethod(&SAMETYPE_METHOD_NAME(ast_integer));
    this->addmethod(&SAMETYPE_METHOD_NAME(ast_real));
    this->addmethod(&SAMETYPE_METHOD_NAME(ast_array));
    this->addmethod(&SAMETYPE_METHOD_NAME(ast_pointer));
    this->addmethod(&SAMETYPE_METHOD_NAME(ast_function));
    this->addmethod(&SAMETYPE_METHOD_NAME(ast_record));
}

bool ast_sametype_func::sametype_list(ref<list<ast_type>> lhs, ref<list<ast_type>> rhs)  noexcept {
    if(lhs->size() != rhs->size()) {
        return false;
    }
    else {
        auto litr = lhs->begin();
        auto ritr = rhs->begin();
        while(litr != lhs->end()) {
            if(!this->visit(*litr, *ritr)) {
                return false;
            }
            litr++;
            ritr++;
        }
        return true;
    }
}

/* ----------------- *
 * Sametype for void *
 * ----------------- */

SAMETYPE_FUNC(ast_void, lhs, rhs) {
    return is_void_type(rhs);
}

/* -------------------- *
 * Sametype for integer *
 * -------------------- */

SAMETYPE_FUNC(ast_integer, lhs, rhs) {
    if(is_integer_type(rhs)) {
        return lhs->bitwidth == rhs->as<ast_integer_type>()->bitwidth &&
                lhs->is_unsigned == rhs->as<ast_integer_type>()->is_unsigned;
    }
    return false;
}

/* ----------------- *
 * Sametype for real *
 * ----------------- */

SAMETYPE_FUNC(ast_real, lhs, rhs) {
    if(is_real_type(rhs)) {
        return lhs->bitwidth == rhs->as<ast_real_type>()->bitwidth;
    }
    return false;
}

/* ------------------ *
 * Sametype for array *
 * ------------------ */

SAMETYPE_FUNC(ast_array, lhs, rhs) {
    if(is_array_type(rhs)) {
        return lhs->size == rhs->as<ast_array_type>()->size &&
                this->visit(lhs->element_type, rhs->as<ast_array_type>()->element_type);
    }
    return false;
}

/* ------------------------- *
 * Sametype for pointer type *
 * ------------------------- */

SAMETYPE_FUNC(ast_pointer, lhs, rhs) {
    if(is_pointer_type(rhs)) {
        return this->visit(lhs->element_type, rhs->as<ast_pointer_type>()->element_type);
    }
    return false;
}

/* -------------------------- *
 * Sametype for function type *
 * -------------------------- */

SAMETYPE_FUNC(ast_function, lhs, rhs) {
    if(is_function_type(rhs)) {
        return
                this->visit(lhs->return_type, rhs->as<ast_function_type>()->return_type) &&
                this->sametype_list(lhs->parameter_types, rhs->as<ast_function_type>()->parameter_types);
    }
    return false;
}

/* ------------------------ *
 * Sametype for record type *
 * ------------------------ */
SAMETYPE_FUNC(ast_record, lhs, rhs) {
    if(is_record_type(rhs)) {
        return
                lhs->is_packed == rhs->as<ast_record_type>()->is_packed &&
                this->sametype_list(lhs->field_types, rhs->as<ast_record_type>()->field_types);
    }
    return false;
}



/* ================= *
 * = Cast Function = *
 * ================= */

ast_cast_func::ast_cast_func(const ast_builder_impl_t& builder) noexcept
        : _builder(builder) {
    this->addmethod(&CAST_METHOD_NAME(ast_integer));
    this->addmethod(&CAST_METHOD_NAME(ast_real));
    this->addmethod(&CAST_METHOD_NAME(ast_pointer));
}

static inline ast_expr* bitcast_to(ast_type* tt, ast_expr* fe) noexcept {
    auto ce = new ast_cast(tt, ast_op::bitcast, fe);
    ce->value_type = fe->value_type;
    return ce;
}

/* ---------------------- *
 * Rules for cast integer *
 * ---------------------- */

CAST_FUNC(ast_integer, tt, fe) {
    uint32_t    tw = tt->bitwidth;
    bool        tu = tt->is_unsigned;

    // Cast from another integer type
    // ------------------------------
    if(is_integer_type(fe->type)) {
        auto        ft  = fe->type->as<ast_integer_type>();
        uint32_t    fw  = ft->bitwidth;
        bool        fu  = ft->is_unsigned;

        if(tw < fw) {
            return new ast_cast(tt, ast_op::trunc, fe);
        }
        else if(tw == fw) {
            if(tu == fu) return fe; // same type
            else         return bitcast_to(tt, fe);
        }
        else if(tw > fw) {
            if(tu) {
                return new ast_cast(tt, ast_op::zext, fe);
            }
            else {
                return new ast_cast(tt, ast_op::sext, fe);
            }
        }
    }

    // Cast from a floating point type
    // -------------------------------
    if(is_real_type(fe->type)) {
        auto        ft  = fe->type->as<ast_real_type>();
        uint32_t    fw  = ft->bitwidth;

        if(tu) {
            return new ast_cast(tt, ast_op::ftou, fe);
        }
        else {
            return new ast_cast(tt, ast_op::ftoi, fe);
        }
    }

    // Cast from a pointer type
    // ------------------------
    if(is_pointer_type(fe->type)) {
        ast_type*   ft = fe->type->as<ast_pointer_type>();
        if(tu) {
            return new ast_cast(tt, ast_op::ptou, fe);
        }
        else {
            return bitcast_to(tt, this->visit(_builder.get_integer_type(tw, true), fe));
        }
    }

    __throw_unhandled_ast_type(__FILE__, __LINE__, fe->type, "CAST_FUNC");
}

/* ---------------------- *
 * Rules for cast to real *
 * ---------------------- */

CAST_FUNC(ast_real, tt, fe) {
    uint32_t tw = tt->bitwidth;

    // Cast from another real type
    // ---------------------------
    if(is_real_type(fe->type)) {
        auto            ft = fe->type->as<ast_real_type>();
        uint32_t        fw = ft->bitwidth;

        if(tw > fw) {
            return new ast_cast(tt, ast_op::fext, fe);
        }
        else if(tw < fw) {
            return new ast_cast(tt, ast_op::ftrunc, fe);
        }
        else {
            return fe; // same type
        }
    }

    // Cast from an integer type
    // -------------------------
    if(is_integer_type(fe->type)) {
        auto            ft = fe->type->as<ast_integer_type>();
        uint32_t        fw = ft->bitwidth;
        bool            fu = ft->is_unsigned;

        if(fu) {
            return new ast_cast(tt, ast_op::utof, fe);
        }
        else {
            return new ast_cast(tt, ast_op::itof, fe);
        }
    }

    __throw_unhandled_ast_type(__FILE__, __LINE__, fe->type, "CAST_FUNC");
}

/* ------------------------- *
 * Rules for cast to pointer *
 * ------------------------- */

CAST_FUNC(ast_pointer, tt, fe) {
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



/* ==================== *
 * = Maxtype Function = *
 * ==================== */

ast_maxtype_func::ast_maxtype_func(const ast_builder_impl_t& builder) noexcept
        : _builder(builder) {
    this->addmethod(&MAXTYPE_METHOD_NAME(ast_integer));
    this->addmethod(&MAXTYPE_METHOD_NAME(ast_real));
    this->addmethod(&MAXTYPE_METHOD_NAME(ast_pointer));
}

/* ------------------------- *
 * Maxtype for integer types *
 * ------------------------- */

MAXTYPE_FUNC(ast_integer, lhs, rhs) {

    // rhs is an integer type
    //-----------------------
    if(is_integer_type(rhs)) {
        uint32_t        lw  = lhs->bitwidth;
        bool            lu  = lhs->is_unsigned;
        uint32_t        rw  = rhs->as<ast_integer_type>()->bitwidth;
        bool            ru  = rhs->as<ast_integer_type>()->is_unsigned;

        if(lu == ru) {
            if(lw >= rw)        return lhs;
            else                return rhs;
        }
        else if(lu) {
            return this->visit(_builder.get_integer_type(lw, false), rhs);
        }
        else if(ru) {
            return this->visit(lhs, _builder.get_integer_type(rw, false));
        }
    }

    // rhs is a real type
    // ------------------
    if(is_real_type(rhs)) {
        return rhs;
    }

    __throw_unhandled_ast_type(__FILE__, __LINE__, rhs, "MAXTYPE_FUNC(ast_integer)");
}

/* ---------------------- *
 * Maxtype for real types *
 * ---------------------- */

MAXTYPE_FUNC(ast_real, lhs, rhs) {

    // rhs is a real type
    // ------------------
    if(is_real_type(rhs)) {
        uint32_t        lw = lhs->bitwidth;
        uint32_t        rw = rhs->as<ast_real_type>()->bitwidth;

        if(lw >= rw) return lhs;
        else         return rhs;
    }

    // lhs is an integer type
    // ----------------------
    if(is_integer_type(rhs)) {
        return lhs;
    }

    __throw_unhandled_ast_type(__FILE__, __LINE__, rhs, "MAXTYPE_FUNC(ast_real)");
}



/* =================== *
 * = Widens Function = *
 * =================== */

ast_widens_func::ast_widens_func(const ast_builder_impl_t& builder) noexcept
        : _builder(builder) {
    this->addmethod(&WIDENS_METHOD_NAME(ast_void));
    this->addmethod(&WIDENS_METHOD_NAME(ast_integer));
    this->addmethod(&WIDENS_METHOD_NAME(ast_real));
    this->addmethod(&WIDENS_METHOD_NAME(ast_array));
    this->addmethod(&WIDENS_METHOD_NAME(ast_pointer));
    this->addmethod(&WIDENS_METHOD_NAME(ast_function));
    this->addmethod(&WIDENS_METHOD_NAME(ast_record));
}

#define WIDENS_RETURN_FALSE                             return false;
#define WIDENS_RETURN_NOWORK                            return true;
#define WIDENS_RETURN_WORK(c, w)                        c += w; return true;

/* ------------------------- *
 * Rules for widen void type *
 * ------------------------- */

WIDENS_FUNC(ast_void, tt, fe, cost) {
    ast_type* ft = fe->type;
    if(is_void_type(ft)) {
        WIDENS_RETURN_NOWORK
    }
    else {
        WIDENS_RETURN_FALSE
    }
}

/* ---------------------------- *
 * Rules for widen integer type *
 * ---------------------------- */

WIDENS_FUNC(ast_integer, tt, fe, cost) {
    ast_type* ft = fe->type;

    // from another integer type
    // -------------------------
    if(is_integer_type(ft)) {
        uint32_t    tw = tt->bitwidth;
        bool        tu = tt->is_unsigned;
        uint32_t    fw = ft->as<ast_integer_type>()->bitwidth;
        bool        fu = ft->as<ast_integer_type>()->is_unsigned;
        if(tw < fw) {
            WIDENS_RETURN_FALSE
        }
        else if(tw > fw) {
            if(tu == fu) { WIDENS_RETURN_WORK(cost, 1) }
            else         { WIDENS_RETURN_WORK(cost, 2) }
        }
        else {
            if(tu == fu) { WIDENS_RETURN_NOWORK        }
            else         { WIDENS_RETURN_WORK(cost, 1) }
        }
    }
    WIDENS_RETURN_FALSE
}

/* ------------------------- *
 * Rules for widen real type *
 * ------------------------- */

WIDENS_FUNC(ast_real, tt, fe, cost) {
    ast_type* ft = fe->type;

    // from another real type
    // ----------------------
    if(is_real_type(ft)) {
        uint32_t    tw = tt->bitwidth;
        uint32_t    fw = tt->bitwidth;
        if(tw == fw)        { WIDENS_RETURN_NOWORK        }
        else if(fw < tw)    { WIDENS_RETURN_WORK(cost, 1) }
        else                { WIDENS_RETURN_FALSE         }
    }

    // from an integer type
    // --------------------
    if(is_integer_type(ft)) {
        WIDENS_RETURN_WORK(cost, 2);
    }
    WIDENS_RETURN_FALSE
}

/* -------------------------- *
 * Rules for widen array type *
 * -------------------------- */

WIDENS_FUNC(ast_array, tt, fe, cost) {
    WIDENS_RETURN_FALSE
}

/* ---------------------------- *
 * Rules for widen pointer type *
 * ---------------------------- */

// TODO: auto conversion from func -> *func

WIDENS_FUNC(ast_pointer, tt, fe, cost) {
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

/* ----------------------------- *
 * Rules for widen function type *
 * ----------------------------- */

WIDENS_FUNC(ast_function, tt, fe, cost) {
    if(_builder.sametype(tt, fe->type)) {
        WIDENS_RETURN_NOWORK
    }
    WIDENS_RETURN_FALSE
}

/* --------------------------- *
 * Rules for widen record type *
 * --------------------------- */

WIDENS_FUNC(ast_record, tt, fe, cost) {
    if(_builder.sametype(tt, fe->type)) {
        WIDENS_RETURN_NOWORK
    }
    WIDENS_RETURN_FALSE
}

}

