/*
 * xi_tc_stmt.cpp
 *
 *  Created on: Feb 6, 2018
 *      Author: derick
 */

#include "xi_semantic.hpp"

namespace xcc {

ast_stmt* xi_tc_walker::tc_block_stmt(ast_block_stmt* block, xi_builder& b) {
    for(auto itr = block->decls->begin(); itr != block->decls->end(); itr++) {
        auto ld = *itr;
        if(ld->init_value != nullptr) {
            ast_expr* ival  = tc_single_expr(ld->init_value, b);
            ast_type* dtype = ld->type;
            return tc_maybe_cast(dtype, ival, b);
        }
    }
    return block;
}

ast_stmt* xi_tc_walker::tc_return_stmt(ast_return_stmt* s, xi_builder& b) {
    if(s->expr != nullptr) {
        ast_type* rtype = b.get_return_type();
        ast_expr* rexpr = tc_single_expr(s->expr, b);
        return tc_maybe_cast(rtype, rexpr, b);
    }
    return s;
}

}

