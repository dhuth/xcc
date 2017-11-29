/*
 * xi_semantic.cpp
 *
 *  Created on: Nov 22, 2017
 *      Author: derick
 */

#include "xi_builder.hpp"


namespace xcc {

static inline ast_expr* single_expr(ast_expr* e) {
    if(e->get_tree_type() == tree_type_id::xi_group_expr) {
        return e->as<xi_group_expr>()->expressions[0];
    }
    return e;
}

static inline ast_type* single_type(ast_type* t) {
    if(t->get_tree_type() == tree_type_id::xi_group_type) {
        return t->as<xi_group_type>()->types[0];
    }
    return t;
}

static inline bool is_ref_of(ast_type* t, ast_type** et) {
    if(t->is<xi_reference_type>()) {
        *et = t->as<xi_reference_type>()->type;
        return true;
    }
    return false;
}

// ----------------------------------- //
// * -> Resolve names -> Get type -> * //
// ----------------------------------- //

static ast_expr* rn_id_expr(xi_id_expr* e, xi_builder& b) {
    auto decls = b.find_all_declarations(e->name->c_str());
    if(decls->size() == 0) {
        //TODO: throw an error
        return nullptr;
    }
    else if(decls->size() == 1) {
        auto decl = (*decls)[0];
        return new ast_declref(b.get_declaration_type(decl), decl);
    }
    else {
        auto typed_decls = xcc::map<ast_expr, ast_decl>(decls,[&](ast_decl* d) -> ast_expr* {
            return copyloc(new ast_declref(b.get_declaration_type(d), d), e);
        });
        return new xi_group_expr(typed_decls);
    }
}

static ast_type* rn_id_type(xi_id_type* t, xi_builder& b) {
    auto decls = b.find_all_declarations(t->name->c_str());

    if(decls->size() == 0) {
        //TODO: throw an error
        return nullptr;
    }
    else if(decls->size() == 1) {
        auto decl = (*decls)[0];
        //TODO: generic parameterization maybe?
        return b.get_object_type(decl->as<xi_decl>());
    }
    else {
        //TODO: generic parameterization maybe?
        auto decl = (*decls)[0];
        return b.get_object_type(decl->as<xi_decl>());
    }
}

static ast_expr* rn_member_id_expr(xi_member_id_expr* e, xi_builder& b) {
    auto expr = single_expr(e->expr);

    // limit type
    ast_type* et        = expr->type;
    bool      isref     = is_ref_of(expr->type, &et);
    if(!(et->is<xi_object_type>() || et->is<xi_reference_type>())) {
        // TODO: error
        return nullptr;
    }

    // get member info
    auto mlist = box(new list<xi_member_decl>());
    for(auto m: et->as<xi_object_type>()->declaration->as<xi_type_decl>()->members) {
        if((std::string)m->name == (std::string)e->name) {
            mlist->append(m);
        }
    }

    if(mlist->size() == 0) {
        // TODO: error
        return nullptr;
    }
    else if(mlist->size() == 1) {
        auto res = copyloc(new xi_member_expr(et, expr, (*mlist)[0]), e);
        res->type = b.get_declaration_type((*mlist)[0]);
        return res;
    }
    else {
        auto typed_refs = map<ast_expr, xi_member_decl>(mlist, [&](xi_member_decl* d) -> ast_expr* {
            auto res = new xi_member_expr(et, expr, d);
            res->type = b.get_declaration_type(d);
            return res;
        });
        return copyloc(new xi_group_expr(typed_refs), e);
    }
}


static ast_type* tc_pointer_type(ast_pointer_type* t, xi_builder& b) {
    return b.get_pointer_type(t->element_type);
}

static ast_type* tc_reference_type(xi_reference_type* t, xi_builder& b) {
    return b.get_reference_type(t->type);
}

static ast_type* tc_const_type(xi_const_type* t, xi_builder& b) {
    return b.get_const_type(t->type);
}

static ast_type* tc_tuple_type(xi_tuple_type* t, xi_builder& b) {
    auto types = map<ast_type, ast_type>(t->types, [&](ast_type* t) -> ast_type* { return single_type(t); });
    return b.get_tuple_type(types);
}


static ast_stmt* tc_return_stmt(ast_return_stmt* r, xi_builder& b) {
    auto rtype = b.get_return_type();
    auto rexpr = single_expr(r->expr);
    if(rtype != nullptr && rtype->get_tree_type() != tree_type_id::ast_void_type) {
        r->expr = copyloc(b.make_cast_expr(rtype, rexpr), rexpr);
    }
    return r;
}

static ast_stmt* tc_if_stmt(ast_if_stmt* stmt, xi_builder& b) {
    stmt->condition = copyloc(b.make_cast_expr(b.get_bool_type(), stmt->condition), (ast_expr*) stmt->condition);
    return stmt;
}

static ast_stmt* tc_while_stmt(ast_while_stmt* stmt, xi_builder& b) {
    stmt->condition = copyloc(b.make_cast_expr(b.get_bool_type(), stmt->condition), (ast_expr*) stmt->condition);
}


static ast_expr* tc_op_expr(xi_op_expr* e, xi_builder& b) {
    // TODO: a lot of stuff

    //auto overload_expr = b.find_best_overload(e->op, e->operands);
    //if(overload_expr != nullptr) {
    //    return overload_expr;
    //}

    return e;
}

static ast_expr* tc_tuple_expr(xi_tuple_expr* e, xi_builder& b) {
    auto exprs = map<ast_expr, ast_expr>(e->expressions, [&](ast_expr* e) -> ast_expr* { return single_expr(e); });
    auto types = map<ast_type, ast_expr>(exprs,          [&](ast_expr* e) -> ast_type* { return single_type(e->type); });
    e->expressions = exprs;
    e->type        = b.get_tuple_type(types);
    return e;
}


struct xi_semantic_checker : public xi_postorder_walker {
public:

    inline xi_semantic_checker() noexcept
            : dispatch_postorder_tree_walker<ast_tree, xi_builder&>() {
        // resolve names
        this->add(&rn_id_expr);
        this->add(&rn_id_type);
        this->add(&rn_member_id_expr);

        // type check
        this->add(&tc_pointer_type);
        this->add(&tc_reference_type);
        this->add(&tc_const_type);
        this->add(&tc_tuple_type);

        this->add(&tc_if_stmt);
        this->add(&tc_while_stmt);
        this->add(&tc_return_stmt);

        this->add(&tc_op_expr);
        this->add(&tc_tuple_expr);
    }
};

void xi_builder::semantic_check() noexcept {
    xi_semantic_checker checker;
    checker.visit(this->global_namespace, *this);
}


}
