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

            //TODO: more general type inference
            if(is_auto_type(dtype)) {
                ld->init_value = ival;
                ld->type       = ival->type;
            }
            else {
                ld->init_value = b.cast(dtype, ival);
            }
        }
    }
    return block;
}

ast_stmt* xi_tc_walker::tc_return_stmt(ast_return_stmt* s, xi_builder& b) {
    if(s->expr != nullptr) {
        ast_type* rtype = b.get_return_type();
        ast_expr* rexpr = tc_single_expr(s->expr, b);

        //TODO: may infer return type
        s->expr = b.cast(rtype, rexpr);
    }
    return s;
}

}

