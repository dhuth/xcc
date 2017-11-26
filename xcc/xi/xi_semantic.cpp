/*
 * xi_semantic.cpp
 *
 *  Created on: Nov 22, 2017
 *      Author: derick
 */

#include "xi_builder.hpp"


namespace xcc {

static inline ast_expr* single_expr(ast_expr* e) {
    if(e->get_tree_type() == tree_type_id::xi_declref_group_expr) {
        return e->as<xi_declref_group_expr>()->declrefs[0];
    }
    return e;
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

ast_expr* rn_id_expr(xi_id_expr* e, xi_builder& b) {
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
        auto typed_decls = xcc::map<ast_declref, ast_decl>(decls,[&](ast_decl* d) -> ast_declref* {
            return copyloc(new ast_declref(b.get_declaration_type(d), d), e);
        });
        return new xi_declref_group_expr(typed_decls);
    }
}

ast_type* rn_id_type(xi_id_type* t, xi_builder& b) {
    auto decls = b.find_all_declarations(t->name->c_str());

    if(decls->size() == 0) {
        //TODO: throw an error
        return nullptr;
    }
    else if(decls->size() == 1) {
        auto decl = (*decls)[0];
        return b.get_object_type(decl->as<xi_decl>());
    }
    else {
        //TODO: generic parameterization maybe?
        auto decl = (*decls)[0];
        return b.get_object_type(decl->as<xi_decl>());
    }
}

ast_expr* rn_member_id_expr(xi_member_id_expr* e, xi_builder& b) {
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
    for(auto m: et->as<xi_type_decl>()->members) {
        if((std::string)m->name == (std::string)e->name) {
            mlist->append(m);
        }
    }

    if(mlist->size() == 0) {
        // TODO: error
        return nullptr;
    }
    else if(mlist->size() == 1) {
        auto res = new xi_member_expr(et, expr, (*mlist)[0]);
        res->type = b.get_declaration_type((*mlist)[0]);
        return res;
    }
    else {
        //TODO: return expression group
    }

    // TODO: ...
    return e;
}

ast_expr* rn_deref_member_id_expr(xi_deref_member_id_expr* e, xi_builder& b) {
    auto expr = single_expr(e->expr);
    // TODO: limit type of expr
    // TODO: get member info
    // TODO: ...
    return e;
}

ast_stmt* tc_return_stmt(ast_return_stmt* r, xi_builder& b) {
    auto rtype = b.get_return_type();
    auto rexpr = single_expr(r->expr);
    if(rtype != nullptr && rtype->get_tree_type() != tree_type_id::ast_void_type) {
        r->expr = copyloc(b.make_cast_expr(rtype, rexpr), rexpr);
    }
    return r;
}

ast_expr* tc_op_expr(xi_op_expr* e, xi_builder& b) {
    // TODO: a lot of stuff
    return e;
}

struct xi_semantic_checker : public dispatch_postorder_tree_walker<ast_tree, xi_builder&> {
public:

    inline xi_semantic_checker() noexcept
            : dispatch_postorder_tree_walker<ast_tree, xi_builder&>() {
        // resolve names
        this->add(&rn_id_expr);
        this->add(&rn_member_id_expr);
        this->add(&rn_deref_member_id_expr);

        // type check
        this->add(&tc_return_stmt);
        this->add(&tc_op_expr);
    }

    void begin(tree_type_id id, ast_tree* t, xi_builder& b) {
        switch(id) {
        case tree_type_id::ast_namespace_decl:
            b.push_namespace(t->as<ast_namespace_decl>());
            break;
        case tree_type_id::ast_block_stmt:
            b.push_block(t->as<ast_block_stmt>());
            break;
        case tree_type_id::xi_function_decl:
            b.push_function(t->as<xi_function_decl>());
            break;
        }
    }

    void end(tree_type_id id, ast_tree* t, xi_builder& b) {
        switch(id) {
        case tree_type_id::ast_namespace_decl:
        case tree_type_id::ast_block_stmt:
        case tree_type_id::xi_function_decl:
            b.pop();
            break;
        }
    }
};

void xi_builder::semantic_check() {
    xi_semantic_checker checker;
    for(auto decl : this->global_namespace->declarations) {
        checker.visit(decl, *this);
    }
}


}
