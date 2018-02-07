/*
 * xi_tc_stmt.cpp
 *
 *  Created on: Feb 6, 2018
 *      Author: derick
 */

#include "xi_semantic.hpp"

namespace xcc {

ast_stmt* xi_tc_walker::tc_return_stmt(ast_return_stmt* s, xi_builder& b) {
    if(s->expr != nullptr) {
        auto rtype = b.get_return_type();
        int cost = 0;
        if(b.widens(s->expr->type, rtype, cost)) {
            s->expr = copyloc(b.widen(rtype, s->expr), (ast_expr*) s->expr);
        }
        else if(b.coercable(rtype, s->expr)) {
            s->expr = copyloc(b.coerce(rtype, s->expr), (ast_expr*) s->expr);
        }
    }
    return s;
}

}

