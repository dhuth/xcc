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

static ptr<ast_context> push_context(ptr<ast_context> ctx, ast_decl* decl) {
    //TODO: visibility
    switch(decl->get_tree_type()) {
    case tree_type_id::xi_namespace_decl:       return ctx->push_context<ast_namespace_context>(decl->as<ast_namespace_decl>());
    case tree_type_id::xi_struct_decl:          return ctx->push_context<xi_struct_context>(decl->as<xi_struct_decl>());
    default:
        __throw_unhandled_tree_type(__FILE__, __LINE__, decl, "push_context");
    }
}

static ptr<ast_context> push_context(ptr<ast_context> ctx, ast_type* type) {
    //TODO: visibility
    switch(type->get_tree_type()) {
    case tree_type_id::xi_decl_type:            return push_context(ctx, type->as<xi_decl_type>()->declaration);
    case tree_type_id::xi_reference_type:       return push_context(ctx, type->as<xi_reference_type>()->type);
    default:
        __throw_unhandled_tree_type(__FILE__, __LINE__, type, "push_context");
    }
}

static ptr<ast_decl> find_declaration(ptr<ast_context> ctx, list<std::string>::iterator iter_start, list<std::string>::iterator iter_end, bool search_parent) {
    //TODO: visibility
    auto decl = ctx->find((*iter_start).c_str(), search_parent);
    iter_start++;
    if(iter_start == iter_end) {
        return decl;
    }
    else {
        return find_declaration(push_context(ctx, decl), iter_start, iter_end, false);
    }
}

static ptr<list<ast_decl>> find_all_declarations(ptr<ast_context> ctx, list<std::string>::iterator iter_start, list<std::string>::iterator iter_end, bool search_parent) {
    //TODO: visibility
    //TODO: change to breadth first
    ptr<list<ast_decl>> ilist = ctx->findall((*iter_start).c_str(), search_parent);
    iter_start++;
    if(iter_start == iter_end) {
        return ilist;
    }
    else {
        ptr<list<ast_decl>> olist = new list<ast_decl>();
        for(auto d: ilist) {
            auto res = find_all_declarations(push_context(ctx, d), iter_start, iter_end, false);
            for(auto r: res) {
                olist->append(r);
            }
        }
        return olist;
    }
}


ast_decl* xi_builder::find_declaration(xi_qname* qname) const noexcept {
    return xcc::find_declaration(this->context, begin(qname->names), end(qname->names), true);
}

ptr<list<ast_decl>> xi_builder::find_all_declarations(xi_qname* qname) const noexcept {
    return xcc::find_all_declarations(this->context, begin(qname->names), end(qname->names), true);
}

xi_member_decl* xi_builder::find_member(ast_type* tp, xi_qname* name) const noexcept {
    //TODO: visibility
    //TODO: look for extension members (???)
    return xcc::find_declaration(xcc::push_context(this->context, tp), begin(name->names), end(name->names), false)->as<xi_member_decl>();
}

ptr<list<xi_member_decl>> xi_builder::find_all_members(ast_type* tp, xi_qname* name) const noexcept {
    //TODO: visibility
    //TODO: look for extension members (???)
    return map(xcc::find_all_declarations(xcc::push_context(this->context, tp), begin(name->names), end(name->names), false),
            [=](ast_decl* d) -> xi_member_decl* { return d->as<xi_member_decl>(); });
}

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

