/*
 * xi_tc_op.cpp
 *
 *  Created on: Jan 30, 2018
 *      Author: derick
 */

#include "xi_semantic.hpp"
#include "error.hpp"

namespace xcc {

ast_expr* xi_tc_walker::tc_op_expr(xi_op_expr* e, xi_builder& b) {
    //TODO: search for an overload

    if(e->operands->size() == 2) {
        // make lower binary op

        auto lhs = tc_single_expr((ast_expr*) e->operands[0], b);
        auto rhs = tc_single_expr((ast_expr*) e->operands[1], b);

        switch((xi_operator) e->op) {
        case xi_operator::__and__:      return b.make_op_expr(ast_op::land,     lhs, rhs);
        case xi_operator::__or__:       return b.make_op_expr(ast_op::lor,      lhs, rhs);
        case xi_operator::__xor__:      return b.make_op_expr(ast_op::lxor,     lhs, rhs);
        case xi_operator::__add__:      return b.make_op_expr(ast_op::add,      lhs, rhs);
        case xi_operator::__sub__:      return b.make_op_expr(ast_op::sub,      lhs, rhs);
        case xi_operator::__mul__:      return b.make_op_expr(ast_op::mul,      lhs, rhs);
        case xi_operator::__div__:      return b.make_op_expr(ast_op::div,      lhs, rhs);
        case xi_operator::__mod__:      return b.make_op_expr(ast_op::mod,      lhs, rhs);
        case xi_operator::__sl__:       return b.make_op_expr(ast_op::shl,      lhs, rhs);
        case xi_operator::__sr__:       return b.make_op_expr(ast_op::shr,      lhs, rhs);
        case xi_operator::__band__:     return b.make_op_expr(ast_op::band,     lhs, rhs);
        case xi_operator::__bor__:      return b.make_op_expr(ast_op::bor,      lhs, rhs);
        case xi_operator::__bxor__:     return b.make_op_expr(ast_op::bxor,     lhs, rhs);
        case xi_operator::__lt__:       return b.make_op_expr(ast_op::lt,       lhs, rhs);
        case xi_operator::__le__:       return b.make_op_expr(ast_op::le,       lhs, rhs);
        case xi_operator::__gt__:       return b.make_op_expr(ast_op::gt,       lhs, rhs);
        case xi_operator::__ge__:       return b.make_op_expr(ast_op::ge,       lhs, rhs);
        case xi_operator::__eq__:       return b.make_op_expr(ast_op::eq,       lhs, rhs);
        case xi_operator::__ne__:       return b.make_op_expr(ast_op::ne,       lhs, rhs);

        case xi_operator::__assign__:   return b.make_assign_expr(lhs, rhs);

        default:
            __throw_unhandled_operator(__FILE__, __LINE__, (xi_operator) e->op, "xi_tc_walker::tc_op_expr");
        }
    }
    else if(e->operands->size() == 1) {
        // make lower unary op

        auto ue = tc_single_expr((ast_expr*) e->operands[0], b);

        switch((xi_operator) e->op) {
        case xi_operator::__neg__:      return b.make_op_expr(ast_op::neg, ue);
        default:
            __throw_unhandled_operator(__FILE__, __LINE__, (xi_operator) e->op, "xi_tc_walker::tc_op_expr");
        }
    }
    __throw_unhandled_operator(__FILE__, __LINE__, (xi_operator) e->op, "xi_tc_walker::tc_op_expr");
}
}


