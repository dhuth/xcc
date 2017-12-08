/*
 * xi_semantic.cpp
 *
 *  Created on: Nov 22, 2017
 *      Author: derick
 */

#include "xi_builder.hpp"


namespace xcc {

static ast_expr* rn_id_expr(xi_id_expr* idexpr, xi_builder& b) {
    auto decls = b.find_all_declarations(idexpr->name->c_str());
    if(decls->size() == 0) {
        //TODO: error
        return nullptr;
    }
    else if(decls->size() == 1) {
        return copyloc(b.make_declref_expr(first(decls)), idexpr);
    }
    else {
        auto exprs = map(decls, [&](ast_decl* d)-> ast_expr* {
            return copyloc(b.make_declref_expr(d), idexpr);
        });
        return new xi_group_expr(exprs);
    }
}

static inline ast_expr* single_expr(ast_expr* e) {
    if(e->is<xi_group_expr>()) {
        return first(e->as<xi_group_expr>()->expressions);
    }
    return e;
}

static inline ast_type* single_type(ast_type* t) {
    if(t->is<xi_group_type>()) {
        return first(t->as<xi_group_type>()->types);
    }
    return t;
}

ptr<list<ast_expr>> xi_builder::find_all_member_exprs(ast_type* type, ast_expr* expr, const char* name) const noexcept {
    // TODO: static members

    switch(type->get_tree_type()) {
    case tree_type_id::ast_void_type:
    case tree_type_id::xi_auto_type:
    case tree_type_id::ast_record_type:
    case tree_type_id::ast_pointer_type:
    case tree_type_id::ast_array_type:
    case tree_type_id::ast_function_type:
        // TODO: error
        break;
    case tree_type_id::ast_integer_type:
    case tree_type_id::ast_real_type:
        // TODO: primitive extensions
        break;
    case tree_type_id::xi_const_type:
        // TODO: ???
        break;
    case tree_type_id::xi_group_type:
        // TODO: ???
        break;
    case tree_type_id::xi_id_type:
        // TODO: internal error -- should not happen
        assert(false);
        break;
    case tree_type_id::xi_object_type: {
        auto objdecl    = (xi_decl*) type->as<xi_object_type>()->declaration;
        auto typedecl   = objdecl->as<xi_type_decl>();

        auto mlist = filter(typedecl->members, [&](xi_member_decl* m)->bool {
            return (std::string) m->name == std::string(name);
        });
        return map(mlist, [&](xi_member_decl* m) -> ast_expr* {
            return new xi_member_expr(type, expr, m);
        });
    }
    case tree_type_id::xi_reference_type:
        return this->find_all_member_exprs(type->as<xi_reference_type>()->type, expr, name);
    case tree_type_id::xi_tuple_type:
        // TODO: ???
        break;
    }

    return box(new list<xi_member_decl>());
}

static ast_expr* rn_member_id_expr(xi_member_id_expr* e, xi_builder& b) {
    auto exprs = b.find_all_member_exprs(e->expr->type, e->expr, e->name->c_str());
    if(exprs->size() == 0) {
        // TODO: error
        return nullptr;
    }
    else if(exprs->size() == 1) {
        return copyloc(first(exprs), e);
    }
    else {
        for(auto me: exprs) {
            copyloc(me, e);
        }
        return new copyloc(new xi_group_expr(exprs), e);
    }
}

static ast_expr* ck_tuple_expr(xi_tuple_expr* expr, xi_builder& b) {
    auto types = map(expr->expressions, [&](ast_expr* e)->ast_type* {
        return e->type;
    });
    expr->type = b.get_tuple_type(types);
    return expr;
}

static ast_expr* ck_op_expr(xi_op_expr* expr, xi_builder& b) {
    typedef xi_op_expr::xi_operator op_t;

    auto op         = (op_t)            expr->op;
    auto exprs      = (list<ast_expr>*) expr->operands;
    auto expr_first =                   first(exprs);
    auto expr_rest  =                   box(rest(exprs));

    auto overload_exprs = b.find_all_overload_exprs(expr_first, op, expr_rest);
    if(overload_exprs->size() > 0) {
        return first(overload_exprs);
    }

    // TODO: ...
    return expr;
}


struct xi_semantic_checker : public xi_postorder_walker {
public:

    inline xi_semantic_checker() {
        this->add(rn_id_expr);
        this->add(rn_member_id_expr);

        this->add(ck_tuple_expr);
        this->add(ck_op_expr);
    }
};


void xi_builder::semantic_check() noexcept {
}


}
