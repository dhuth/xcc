/*
 * xi_printer.cpp
 *
 *  Created on: Feb 27, 2017
 *      Author: derick
 */


#include <xi_old/xi_internal.hpp>

namespace xcc {
std::string to_string(xcc::xi_operator op) {
    switch(op) {
    case xcc::xi_operator::add:                      return "xi+";
    case xcc::xi_operator::sub:                      return "xi-";
    case xcc::xi_operator::mul:                      return "xi*";
    case xcc::xi_operator::div:                      return "xi/";
    case xcc::xi_operator::mod:                      return "xi%";
    case xcc::xi_operator::shl:                      return "xi<<";
    case xcc::xi_operator::shr:                      return "xi>>";
    case xcc::xi_operator::lnot:                     return "xi!";
    case xcc::xi_operator::land:                     return "xi&&";
    case xcc::xi_operator::lor:                      return "xi||";
    case xcc::xi_operator::bnot:                     return "xi~";
    case xcc::xi_operator::band:                     return "xi&";
    case xcc::xi_operator::bor:                      return "xi|";
    case xcc::xi_operator::bxor:                     return "xi^";
    case xcc::xi_operator::eq:                       return "xi==";
    case xcc::xi_operator::ne:                       return "xi!=";
    case xcc::xi_operator::lt:                       return "xi<";
    case xcc::xi_operator::le:                       return "xi<=";
    case xcc::xi_operator::gt:                       return "xi>";
    case xcc::xi_operator::ge:                       return "xi>=";
    case xcc::xi_operator::assign:                   return "xi=";
    case xcc::xi_operator::assign_add:               return "xi+=";
    case xcc::xi_operator::assign_sub:               return "xi-=";
    case xcc::xi_operator::assign_mul:               return "xi*=";
    case xcc::xi_operator::assign_div:               return "xi/=";
    case xcc::xi_operator::assign_mod:               return "xi%=";
    case xcc::xi_operator::assign_shl:               return "xi<<";
    case xcc::xi_operator::assign_shr:               return "xi>>";
    }
    assert(false);
    return "?";
}


static void print_xi_function(xi_function_decl* func, std::ostream& s) {
    ast_printer::print(s, "define %0 %1(%{2:, })%3", func->return_type, func->name, func->parameters, func->body);
}

static void print_xi_parameter(xi_parameter_decl* param, std::ostream& s) {
    ast_printer::print(s, "%0 %1", param->type, param->name);
}

static void print_xi_op_expr(xi_op_expr* expr, std::ostream& s) {
    auto op = to_string(expr->op);
    ast_printer::print(s, "([type=%0] %1 %{2:, })", expr->type, op, expr->operands);
}

static void print_xi_assign_stmt(xi_assign_stmt* astmt, std::ostream& s) {
    auto op = to_string(astmt->op);
    ast_printer::print(s, "(let[%0] %1, %2)", op, astmt->lhs, astmt->rhs);
}


void xi_init_printer() {
    ast_printer::add(&print_xi_function);
    ast_printer::add(&print_xi_parameter);
    ast_printer::add(&print_xi_op_expr);
    ast_printer::add(&print_xi_assign_stmt);
}


}
