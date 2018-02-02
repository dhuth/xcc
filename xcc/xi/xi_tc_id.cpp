/*
 * xi_tc_id.cpp
 *
 *  Created on: Jan 30, 2018
 *      Author: derick
 */

#include "xi_semantic.hpp"
#include "xi_context.hpp"
#include "error.hpp"

namespace xcc {

static ast_expr* __tc_single_expr(xi_id_expr* e, xi_builder& b) {
    auto decl = b.find_declaration(e->name);
    if(decl == nullptr) {
        //TODO: error
        assert(false);
        return nullptr;
    }
    else {
        return copyloc(b.make_declref_expr(decl), e);
    }
}

static ast_expr* __tc_single_expr(xi_member_id_expr* e, xi_builder& b) {
    ast_expr* objexpr           = tc_single_expr(e->expr, b);
    ast_type* objtype           = objexpr->type;

    auto member = b.find_member(objtype, e->name);
    //TODO: check member found
    auto rexpr = copyloc(b.make_xi_member_expr(objexpr, member), e);
    return rexpr;
}

ast_expr* tc_single_expr(ast_expr* e, xi_builder& b) {
    if(e->is<xi_id_expr>()) {
        return __tc_single_expr(e->as<xi_id_expr>(), b);
    }
    else if(e->is<xi_member_id_expr>()) {
        return __tc_single_expr(e->as<xi_member_id_expr>(), b);
    }
    return e;
}

ast_expr* xi_tc_walker::tc_id_expr(xi_id_expr* e, xi_builder& b) {
    return e;
}

ast_expr* xi_tc_walker::tc_member_id_expr(xi_member_id_expr* e, xi_builder&) {
    return e;
}

}

