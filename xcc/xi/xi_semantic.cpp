/*
 * xi_semantic.cpp
 *
 *  Created on: Jan 30, 2018
 *      Author: derick
 */

#include "xi_semantic.hpp"

namespace xcc {

xi_tc_walker::xi_tc_walker() noexcept {

    // function declarations
    this->add(&xi_tc_walker::visit_xi_function_decl);
    this->add(&xi_tc_walker::visit_xi_method_decl);
    this->add(&xi_tc_walker::visit_xi_operator_function_decl);
    this->add(&xi_tc_walker::visit_xi_operator_method_decl);

    // expressions
    this->add(&xi_tc_walker::tc_id_expr);
    this->add(&xi_tc_walker::tc_member_id_expr);
    this->add(&xi_tc_walker::tc_op_expr);
}

void xi_tc_walker::visit_xi_function_decl(xi_function_decl* fdecl, xi_builder& b) {
    //...
}
void xi_tc_walker::visit_xi_method_decl(xi_method_decl* fdecl, xi_builder& b) {
    //...
}
void xi_tc_walker::visit_xi_operator_function_decl(xi_operator_function_decl* fdecl, xi_builder& b) {
    //...
}
void xi_tc_walker::visit_xi_operator_method_decl(xi_operator_method_decl* fdecl, xi_builder& b) {
    //...
}



static ast_expr* __widen_reference_expr(ast_type* tp_to, ast_expr* e, const xi_builder& b) {
    return b.widen(tp_to, b.make_xi_deref_expr(e));
}

bool xi_builder::widens(ast_type* tp_from, ast_type* tp_to, int& cost) const {
    assert(tp_from != nullptr);
    assert(tp_from != nullptr);

    if(this->sametype(tp_from, tp_to)) return true;
    //TODO: is there an explicit conversion function (?)

    switch(tp_from->get_tree_type()) {
    case tree_type_id::xi_reference_type:           return this->widens(tp_from->as<xi_reference_type>()->type, tp_to, ++cost);
    default:
        return ast_builder::widens(tp_from, tp_to, cost);
    }
}

ast_expr* xi_builder::widen(ast_type* tp_to, ast_expr* e) const {
    ast_type* tp_from = e->type;

    assert(tp_from != nullptr);

    if(this->sametype(tp_from, tp_to)) return e;
    // TODO: is there an explicit conversion function (?)

    switch(tp_from->get_tree_type()) {
    case tree_type_id::xi_reference_type:           return __widen_reference_expr(tp_to, e, *this);
    default:
        return ast_builder::widen(tp_to, e);
    }
}

bool xi_builder::semantic_pass() noexcept {
    xi_tc_walker tcwalker;

    tcwalker.visit(this->global_namespace, *this);

    return true;
}

}

