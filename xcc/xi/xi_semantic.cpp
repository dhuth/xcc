/*
 * xi_semantic.cpp
 *
 *  Created on: Jan 30, 2018
 *      Author: derick
 */

#include "xi_semantic.hpp"
#include "xi_builder.hpp"

namespace xcc {

xi_tc_walker::xi_tc_walker() noexcept {

    // function declarations
    //this->add(&xi_tc_walker::visit_xi_function_decl);
    //this->add(&xi_tc_walker::visit_xi_method_decl);
    //this->add(&xi_tc_walker::visit_xi_operator_function_decl);
    //this->add(&xi_tc_walker::visit_xi_operator_method_decl);

    // expressions
    this->add(&xi_tc_walker::tc_id_expr);
    this->add(&xi_tc_walker::tc_member_id_expr);
    this->add(&xi_tc_walker::tc_op_expr);
    this->add(&xi_tc_walker::tc_invoke_expr);

    // statements
    this->add(&xi_tc_walker::tc_block_stmt);
    this->add(&xi_tc_walker::tc_return_stmt);
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

bool xi_builder::semantic_pass() noexcept {
    // type check
    // ----------
    xi_tc_walker tcwalker;
    tcwalker.visit(this->global_namespace, *this);

    // TODO: conditional operators and short circuiting
    // ------------------------------------------------

    return true;
}

}

