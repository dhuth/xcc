/*
 * xi_tc_invoke.cpp
 *
 *  Created on: Jan 31, 2018
 *      Author: derick
 */

#include "xi_semantic.hpp"
#include "error.hpp"

namespace xcc {


ast_expr* xi_tc_walker::tc_invoke_expr(xi_invoke_expr* e, xi_builder& b) {
    ast_expr*               fexpr   = e->funcexpr;
    list<ast_expr>*         args    = e->args;
}

}



