/*
 * xi_context.cpp
 *
 *  Created on: Nov 22, 2017
 *      Author: derick
 */

#include "xi_context.hpp"
#include "error.hpp"

namespace xcc {


static bool find_all_rest_members(xi_struct_decl*, ptr<list<ast_decl>>, const char*, bool);

static bool find_all_members(xi_type_decl* tp, ptr<list<ast_decl>> olist, const char* name, bool first_only = false) {
    for(auto m: tp->members) {
        if(m->name == (std::string) name) {
            olist->push_back(m);
            if(first_only) {
                return true;
            }
        }
    }
    switch(tp->get_tree_type()) {
    case tree_type_id::xi_struct_decl:          return find_all_rest_members(tp->as<xi_struct_decl>(), olist, name, first_only);
    default:
        __throw_unhandled_tree_type(__FILE__, __LINE__, tp, "find_all_members");
    }
}

static bool find_all_rest_members(xi_struct_decl* sdecl, ptr<list<ast_decl>> olist, const char* name, bool first_only) {
    //TODO: visibility
    for(auto bt: sdecl->base_types) {
        if(find_all_members(bt->as<xi_decl_type>()->declaration, olist, name, first_only) && first_only) {
            return true;
        }
    }
    return false;
}

static inline ast_decl* find_first_member(xi_type_decl* tp, const char* name) {
    ptr<list<ast_decl>> olist = new list<ast_decl>();
    find_all_members(tp, olist, name, true);
    return first(olist);
}

ptr<ast_decl> xi_function_context::find_first_impl(const char* name) {
    // search parameters
    return first_from_list(this->func->parameters, name);
}

void xi_function_context::find_all_impl(ptr<list<ast_decl>> lout, const char* name) {
    // search parameters
    all_from_list(this->func->parameters, lout, name);
}


ptr<ast_decl> xi_method_context::find_first_impl(const char* name) {
    return first_from_list(this->methoddecl->parameters, name);
}

void xi_method_context::find_all_impl(ptr<list<ast_decl>> lout, const char* name) {
    all_from_list(this->methoddecl->parameters, lout, name);
}


ptr<ast_decl> xi_type_context::find_first_impl(const char* name) {
    return first_from_list(this->typedecl->members, name);
}

void xi_type_context::find_all_impl(ptr<list<ast_decl>> lout, const char* name) {
    // search all members
    all_from_list(this->typedecl->members, lout, name);
}


ptr<ast_decl> xi_struct_context::find_first_impl(const char* name) {
    // search all of my members
    return find_first_member(this->structdecl, name);
}

void xi_struct_context::find_all_impl(ptr<list<ast_decl>> lout, const char* name) {
    // search all of my members
    find_all_members(this->structdecl, lout, name);
}
}


