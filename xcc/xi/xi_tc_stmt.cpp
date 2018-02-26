/*
 * xi_tc_stmt.cpp
 *
 *  Created on: Feb 6, 2018
 *      Author: derick
 */

#include "xi_semantic.hpp"

namespace xcc {

ast_stmt* xi_tc_walker::tc_block_stmt(ast_block_stmt* block, xi_builder& b) {
    for(ast_local_decl* ld: block->decls) {
        if(ld->init_value != nullptr) {
            ast_expr* ival  = tc_single_expr(ld->init_value, b);
            ast_type* itype = ival->type;
            ast_type* dtype = ld->type;
            int i = 0;
            if(b.widens(itype, dtype, i)) {
                ld->init_value = b.widen(dtype, ival);
            }
            else if(b.coercable(dtype, ival)) {
                ld->init_value = b.coerce(dtype, ival);
            }
        }
    }
    return block;
}

ast_stmt* xi_tc_walker::tc_return_stmt(ast_return_stmt* s, xi_builder& b) {
    if(s->expr != nullptr) {
        ast_type* rtype = b.get_return_type();
        ast_expr* rexpr = tc_single_expr(s->expr, b);
        int cost = 0;
        if(b.widens(rexpr->type, rtype, cost)) {
            s->expr = copyloc(b.widen(rtype, rexpr), (ast_expr*) s->expr);
        }
        else if(b.coercable(rtype, rexpr)) {
            s->expr = copyloc(b.coerce(rtype, rexpr), (ast_expr*) s->expr);
        }
    }
    return s;
}

}

