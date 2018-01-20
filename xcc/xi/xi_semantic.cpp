/*
 * xi_semantic.cpp
 *
 *  Created on: Nov 22, 2017
 *      Author: derick
 */

#include "xi_builder.hpp"

namespace xcc {

static void merge_namespaces_in(ast_namespace_decl*) noexcept;

static void merge_namespaces_into(ast_namespace_decl* ns, list<ast_decl>::iterator rest, list<ast_decl>* list) {
    auto itr = rest;
    while(itr != list->end()) {
        auto decl = *itr;
        if(decl->is<ast_namespace_decl>() && (decl->name == ns->name)) {
            list->remove(itr);
            for(auto d: decl->as<ast_namespace_decl>()->declarations) {
                ns->declarations->append(d);
            }
        }
        else {
            itr++;
        }
    }
    merge_namespaces_in(ns);
}


static void merge_namespaces(list<ast_decl>* decls) noexcept {
    list<ast_decl>::iterator itr = decls->begin();

    while(itr != decls->end()) {
        if((*itr)->is<ast_namespace_decl>()) {
            auto ns = (*itr)->as<ast_namespace_decl>();
            itr++;
            merge_namespaces_into(ns, itr, decls);
        }
        else {
            itr++;
        }
    }
}

static void merge_namespaces_in(ast_namespace_decl* ns) noexcept {
    merge_namespaces(ns->declarations);
}

void xi_builder::semantic_check(/*error log info*/) noexcept {
    // merge namespaces
    merge_namespaces_in(this->global_namespace);

    // TODO: typecheck and resolve names
}

}
