/*
 * xi_builder_lower_stmt.cpp
 *
 *  Created on: Feb 15, 2017
 *      Author: derick
 */


#include "xi_builder.hpp"
#include "xi_lower.hpp"

namespace xcc {

void xi_lower_walker::begin_block(ast_block_stmt* stmt) {
    this->_xi_builder.push_block(stmt);
}

ast_stmt* xi_lower_walker::lower_block_stmt(ast_block_stmt* stmt) {
    return stmt;
}

ast_stmt* xi_lower_walker::lower_assign_stmt(xi_assign_stmt* stmt) {
    if(stmt->op == xi_operator::assign) {
        return this->_ast_builder.make_lower_assign_stmt(
                this->lower_expr(stmt->lhs),
                this->lower_expr(stmt->rhs));
    }

    ast_op newop = ast_op::none;
    switch((xi_operator) stmt->op) {
    case xi_operator::assign_add:   newop = ast_op::add;            break;
    case xi_operator::assign_sub:   newop = ast_op::sub;            break;
    case xi_operator::assign_mul:   newop = ast_op::mul;            break;
    case xi_operator::assign_div:   newop = ast_op::div;            break;
    case xi_operator::assign_mod:   newop = ast_op::mod;            break;
    case xi_operator::assign_shl:   newop = ast_op::shl;            break;
    case xi_operator::assign_shr:   newop = ast_op::shr;            break;
    case xi_operator::assign_land:  newop = ast_op::logical_and;    break;
    case xi_operator::assign_lor:   newop = ast_op::logical_or;     break;
    case xi_operator::assign_band:  newop = ast_op::binary_and;     break;
    case xi_operator::assign_bor:   newop = ast_op::binary_or;      break;
    }

    assert(newop != ast_op::none);

    return this->_ast_builder.make_lower_assign_stmt(
            this->lower_expr(stmt->lhs),
            this->_ast_builder.make_op_expr(newop,
                    this->lower_expr(stmt->lhs),
                    this->lower_expr(stmt->rhs)));
}

void xi_lower_walker::end_block(ast_block_stmt* stmt) {
    this->_xi_builder.pop();
}

}

