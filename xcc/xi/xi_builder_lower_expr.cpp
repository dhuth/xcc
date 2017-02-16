/*
 * xi_builder_synthtype.cpp
 *
 *  Created on: Feb 15, 2017
 *      Author: derick
 */


#include "xi_builder.hpp"

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


ast_expr* xi_builder::lower(ast_expr* e) {
    switch(e->get_tree_type()) {
    case tree_type_id::xi_expr_stmt_expr:
        //TODO:...
        break;
    case tree_type_id::xi_op_expr:
        {
            auto op         = e->as<xi_op_expr>()->op;
            auto operands   = map<ast_expr,ast_expr>(e->as<xi_op_expr>()->operands, [&](ast_expr* expr) -> ast_expr* { return this->lower(expr); });

            //TODO: look for overloads (Maybe already calling overload function by now?)

            if(operands->size() == 1) {
                return lower_unary(this, op, (*operands)[0]);
            }
            else if(operands->size() == 2) {
                return lower_binary(this, op, (*operands)[0], (*operands)[1]);
            }
            assert(false);
            break;
        }
    case tree_type_id::ast_declref:
        {
            auto dexpr = e->as<ast_declref>();
            dexpr->declaration  = this->lower(dexpr->declaration);
            dexpr->type         = this->get_declaration_type(dexpr->declaration);
            return dexpr;
        }
    case tree_type_id::ast_cast:
        {
            auto cexpr = e->as<ast_cast>();
            cexpr->expr = this->lower(cexpr->expr);
            cexpr->type = this->lower(cexpr->type);
            return cexpr;
        }
    case tree_type_id::ast_binary_op:
    case tree_type_id::ast_unary_op:
        assert(false);
        break;
    }
    return e;
}

}

