/*
 * xi_builder_synthtype.cpp
 *
 *  Created on: Feb 15, 2017
 *      Author: derick
 */


#include "xi_builder.hpp"
#include "xi_lower.hpp"

namespace xcc {


static ast_expr* lower_unary(xi_builder* builder, xi_operator op, ast_expr* expr) {
    switch(op) {
    case xi_operator::sub:                  return builder->make_op_expr(ast_op::sub,           expr);
    case xi_operator::add:                  return expr;
    case xi_operator::lnot:                 return builder->make_op_expr(ast_op::logical_not,   expr);
    case xi_operator::bnot:                 return builder->make_op_expr(ast_op::binary_not,    expr);
    }

    assert(false);
    return nullptr;
}

static ast_expr* lower_binary(xi_builder* builder, xi_operator op, ast_expr* lhs, ast_expr* rhs) {
    switch(op) {
    case xi_operator::add:                  return builder->make_op_expr(ast_op::add,           lhs, rhs);
    case xi_operator::sub:                  return builder->make_op_expr(ast_op::sub,           lhs, rhs);
    case xi_operator::mul:                  return builder->make_op_expr(ast_op::mul,           lhs, rhs);
    case xi_operator::div:                  return builder->make_op_expr(ast_op::div,           lhs, rhs);
    case xi_operator::mod:                  return builder->make_op_expr(ast_op::mod,           lhs, rhs);
    case xi_operator::shl:                  return builder->make_op_expr(ast_op::shl,           lhs, rhs);
    case xi_operator::shr:                  return builder->make_op_expr(ast_op::shr,           lhs, rhs);
    case xi_operator::eq:                   return builder->make_op_expr(ast_op::eq,            lhs, rhs);
    case xi_operator::ne:                   return builder->make_op_expr(ast_op::ne,            lhs, rhs);
    case xi_operator::lt:                   return builder->make_op_expr(ast_op::lt,            lhs, rhs);
    case xi_operator::le:                   return builder->make_op_expr(ast_op::le,            lhs, rhs);
    case xi_operator::gt:                   return builder->make_op_expr(ast_op::gt,            lhs, rhs);
    case xi_operator::ge:                   return builder->make_op_expr(ast_op::ge,            lhs, rhs);
    case xi_operator::land:                 return builder->make_op_expr(ast_op::logical_and,   lhs, rhs);
    case xi_operator::lor:                  return builder->make_op_expr(ast_op::logical_or,    lhs, rhs);
    case xi_operator::band:                 return builder->make_op_expr(ast_op::binary_and,    lhs, rhs);
    case xi_operator::bor:                  return builder->make_op_expr(ast_op::binary_or,     lhs, rhs);
    case xi_operator::bxor:                 return builder->make_op_expr(ast_op::binary_xor,    lhs, rhs);
    }

    assert(false);
    return nullptr;
}

ast_expr* xi_lower_walker::lower_op_expr(xi_op_expr* e) {
    xi_operator     op                      = e->op;
    list<ast_expr>* operands                = e->operands;

    //TODO: look for overloads ( resolved already ?? )

    if(operands->size() == 1) {
        return lower_unary(&this->_xi_builder, op, (*operands)[0]);
    }
    else if(operands->size() == 2) {
        return lower_binary(&this->_xi_builder, op, (*operands)[0], (*operands)[1]);
    }

    assert(false);
    return nullptr;
}

ast_expr* xi_lower_walker::lower_zero_initializer_expr(xi_zero_initializer_expr* e) {
    //TODO: this should only be for struct types without default constructors
    //auto decl = e->declaration->as<xi_struct_decl>();
    return this->_xi_builder.make_zero(e->type);
}

ast_expr* xi_lower_walker::lower_cast_expr(ast_cast* e) {
    return this->_ast_builder.make_lower_cast_expr(e->type, e->expr);
}

ast_expr* xi_lower_walker::lower_invoke_expr(ast_invoke* e) {
    return this->_ast_builder.make_lower_call_expr(e->funcexpr, e->arguments);
}

}

