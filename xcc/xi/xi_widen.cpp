/*
 * xi_widen.cpp
 *
 *  Created on: Feb 27, 2018
 *      Author: derick
 */

#include "xi_semantic.hpp"
#include "xi_widen.hpp"

namespace xcc {

#define WIDENS_FUNC(n)          bool xi_widens_func::to_##n##_type(n##_type* to_type, ast_type* from_type, int& cost, const xi_builder& b)
#define WIDEN_FUNC(n)           ast_expr* xi_widen_func::to_##n##_type(n##_type* to_type, ast_expr* expr, const xi_builder& b)


#define IS_CONST(t)             (t->is<xi_const_type>())
#define AS_CONST(t)             (t->as<xi_const_type>())
#define CONST_ELTYPE(ct)        ((ast_type*) AS_CONST(ct)->type)

#define IS_REF(t)               (t->is<xi_reference_type>())
#define AS_REF(t)               (t->as<xi_reference_type>())
#define REF_ELTYPE(rt)          ((ast_type*) AS_REF(rt)->element_type)

#define IS_PTR(t)               (t->is<ast_pointer_type>())
#define AS_PTR(t)               (t->as<ast_pointer_type>())
#define PTR_ELTYPE(pt)          ((ast_type*) AS_PTR(pt)->element_type)

xi_widen_func::xi_widen_func() noexcept {
    this->addmethod(&xi_widen_func::to_const_type);
    this->addmethod(&xi_widen_func::to_pointer_type);
    this->addmethod(&xi_widen_func::to_reference_type);
    this->addmethod(&xi_widen_func::to_decl_type);
}

xi_widens_func::xi_widens_func() noexcept {
    this->addmethod(&xi_widens_func::to_const_type);
    this->addmethod(&xi_widens_func::to_pointer_type);
    this->addmethod(&xi_widens_func::to_reference_type);
    this->addmethod(&xi_widens_func::to_decl_type);
}


/* ============================= *
 * Rules for widen to const type *
 * ============================= */

// Rule A:      const a -> const b          iff widens a -> b
// Rule B:            a -> const b          iff widens a -> b

WIDENS_FUNC(xi_const) {
    auto        tet = CONST_ELTYPE(to_type);
    if(IS_CONST(from_type)) {
        auto    fet = CONST_ELTYPE(from_type);

        cost += 1;
        return  b.widens(fet, tet, cost);
    }
    else {
        cost += 1;
        return b.widens(from_type, tet, cost);
    }
}

WIDEN_FUNC(xi_const) {
    //...
}

/* ============================ *
 * Rules for widen pointer type *
 * ============================ */

// Rule A:           *a -> *void            always
// Rule B:           *a -> *b               iff widens a -> b

WIDENS_FUNC(ast_pointer) {
    if(IS_PTR(to_type)) {
        auto tet = PTR_ELTYPE(to_type);
        if(tet->is<ast_void_type>()) {
            cost += 1;
            return true;
        }
        else {
            cost += 1;
            return b.widens(PTR_ELTYPE(from_type), tet, cost);
        }
    }
    return false;
}

WIDEN_FUNC(ast_pointer) {
    //...
}

/* ============================== *
 * Rules for widen reference type *
 * ============================== */

//                      ->
// Rule A:            a -> &a               always*
// Rule B:           &a -> &b               iff widens a -> b

WIDENS_FUNC(xi_reference) {
    //...
}

WIDEN_FUNC(xi_reference) {
    //...
}

/* =================== *
 * Rules for decl type *
 * =================== */

WIDENS_FUNC(xi_decl) {
    //...
}

WIDEN_FUNC(xi_decl) {
    //...
}

/* ================================= *
 * xi_builder functions for widening *
 * ================================= */

// TODO: needs some major rework...
bool xi_builder::widens(ast_type* tp_from, ast_type* tp_to, int& cost) const {
    assert(tp_from != nullptr);
    assert(tp_to   != nullptr);

    return _the_widens_func->visit(tp_to, tp_from, cost, *this);
}

bool xi_builder::widens(ast_type* tp_from, ast_type* tp_to) const {
    int cost = 0;
    return this->widens(tp_from, tp_to);
}

bool xi_builder::ast_widens(ast_type* tp_from, ast_type* tp_to, int& cost) const {
    return ast_builder::widens(tp_from, tp_to, cost);
}

ast_expr* xi_builder::widen(ast_type* tp_to, ast_expr* e) const {
    assert(tp_to    != nullptr);
    assert(e        != nullptr);
    assert(e->type  != nullptr);

    return _the_widen_func->visit(tp_to, e, *this);
}

ast_expr* xi_builder::ast_widen(ast_type* tp_to, ast_expr* e) const {
    return ast_builder::widen(tp_to, e);
}

}


