/*
 * xi_tc_expr.cpp
 *
 *  Created on: Jan 24, 2018
 *      Author: derick
 */

#include "xi_semantic.hpp"
#include "error.hpp"

namespace xcc {

xi_expr_tcvisitor::xi_expr_tcvisitor(xi_decl_tcwalker& decl_walker) noexcept
        : decl_walker(decl_walker) {
    this->addmethod(&xi_expr_tcvisitor::tc_xi_id_expr);
    this->addmethod(&xi_expr_tcvisitor::tc_xi_member_id_expr);
}


static ptr<list<ast_decl>> __choose_decls(ptr<list<ast_decl>> ilist) {
    auto first_decl = first(ilist);

    if(first_decl->is<xi_function_decl>() || first_decl->is<xi_method_decl>()) {
        //TODO: maybe a callable check instead of function or method
        return filter(ilist, [&](ast_decl* d)->bool{
            return d->is<xi_function_decl>() || d->is<xi_method_decl>();
        });
    }
    else {
        return box(new list<ast_decl>(first_decl));
    }
}


ast_expr* xi_expr_tcvisitor::tc_xi_id_expr(xi_id_expr* expr, xi_builder& builder) {
    auto decls = builder.find_all_declarations(expr->name);

    if(decls->size() == 0) {
        //TODO: error here
    }
    else if(decls->size() == 1) {
        return copyloc(builder.make_declref_expr(first(decls)), expr);
    }
    else {
        auto mult_decls = __choose_decls(decls);

        if(mult_decls->size() > 1) {
            return new xi_group_expr(map(mult_decls, [&](ast_decl* d)->ast_expr* {
                return copyloc(builder.make_declref_expr(d), expr);
            }));
        }
        else {
            return copyloc(builder.make_declref_expr(first(mult_decls)), expr);
        }
    }
}


static ast_type* __remove_reference_type(ast_type* t) {
    if(t->is<xi_reference_type>()) {
        return t->as<xi_reference_type>()->type;
    }
    //TODO: handle constant reference
    return t;
}


static ptr<list<ast_decl>> __get_members_byname(ast_type*,     std::string);
static ptr<list<ast_decl>> __get_members_byname(xi_type_decl*, std::string);


static inline xi_type_decl* __get_object_type_decl(ast_type* tp) {
    return tp->as<xi_object_type>()->declaration->as<xi_type_decl>();
}


static ptr<list<ast_decl>> __get_members_byname(xi_type_decl* tp, std::string name) {
    ptr<list<ast_decl>> olist = box(new list<ast_decl>());

    switch(tp->get_tree_type()) {

    case tree_type_id::xi_struct_decl:

        for(auto m: tp->members) {
            if(m->name == name) {
                olist->append(m);
            }
        }
        for(auto bt: tp->as<xi_struct_decl>()->base_types) {
            auto bt_members = __get_members_byname(bt, name);
            for(auto m: bt_members) {
                olist->append(m);
            }
        }
        break;

    default:
        __throw_unhandled_tree_type(__FILE__, __LINE__, tp, "__get_members_byname(xi_type_decl)");
    }

    return olist;
}


static ptr<list<ast_decl>> __get_members_byname(ast_type* objtype, std::string name) {
    switch(__remove_reference_type(objtype)->get_tree_type()) {
    case tree_type_id::xi_object_type:
        return __get_members_byname(__get_object_type_decl(__remove_reference_type(objtype)), name);
    default:
        __throw_unhandled_tree_type(__FILE__, __LINE__, __remove_reference_type(objtype), "__get_members_byname(ast_expr)");
    }
}

ast_expr* xi_expr_tcvisitor::tc_xi_member_id_expr(xi_member_id_expr* mexpr, xi_builder& builder) {
    //TODO: assert objexpr has a non-null type
    //TODO: assert name has only one string

    ast_expr* objexpr = this->visit(mexpr->expr, builder);
    ast_type* objtype = objexpr->type;

    auto decls = __choose_decls(__get_members_byname(objtype, first(mexpr->name->names)));

    if(decls->size() == 0) {
        //TODO: error
    }
    else if(decls->size() == 1) {
        return copyloc(builder.make_declref_expr(first(decls)), mexpr);
    }
    else {
        return new xi_group_expr(map(decls, [&](ast_decl* d)->ast_expr* {
            return copyloc(builder.make_declref_expr(d), mexpr);
        }));
    }
}

ast_expr* xi_expr_tcvisitor::tc_xi_tuple_expr(xi_tuple_expr* texpr, xi_builder& builder) {
    auto visited_exprs = map(texpr->expressions, [&](ast_expr* e)->ast_expr* {
        return copyloc(this->visit(e, builder), e);
    });
    auto tp = builder.get_tuple_type(map(visited_exprs, [&](ast_expr* e)->ast_type*{
        return e->type;
    }));
    return copyloc(new xi_tuple_expr(tp, visited_exprs), texpr);
}
}


