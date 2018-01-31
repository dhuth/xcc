/*
 * xi_semantic.cpp
 *
 *  Created on: Nov 22, 2017
 *      Author: derick
 */


#include <iostream>
#include "xi_dom.hpp"
#include "error.hpp"

namespace xcc {

static void merge_namespaces_in(xi_namespace_decl*) noexcept;


static void merge_namespaces_into(xi_namespace_decl* ns, list<ast_decl>::iterator rest, list<ast_decl>* list) {
    auto itr = rest;
    while(itr != list->end()) {
        auto decl = *itr;
        if(decl->is<xi_namespace_decl>() && (decl->name == ns->name)) {
            list->remove(itr);
            for(auto d: decl->as<xi_namespace_decl>()->declarations) {
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
        if((*itr)->is<xi_namespace_decl>()) {
            auto ns = (*itr)->as<xi_namespace_decl>();
            itr++;
            merge_namespaces_into(ns, itr, decls);
        }
        else {
            itr++;
        }
    }
}


static void merge_namespaces_in(xi_namespace_decl* ns) noexcept {
    merge_namespaces(ns->declarations);
}


ast_type* dom_qname_resolver::resolve_id_type(xi_id_type* t, xi_builder& b) {
    auto dlist = b.find_all_declarations(t->name);
    if(dlist->size() == 0) {
        //TODO: throw an error or log an error or something
        std::cout << "unknown name " << first(t->name->names) << " at line " << t->source_location->first.line_number << "\n";

        assert(false);
        return nullptr;
    }
    else {
        auto decl = first(dlist);
        switch(decl->get_tree_type()) {
        case tree_type_id::xi_struct_decl:      return b.get_object_type(decl);
        default:
            __throw_unhandled_tree_type(__FILE__, __LINE__, decl, "dom_qname_resolver::resolve_id_type()");
        }
    }
}


bool xi_builder::dom_pass(/*options & error log info*/) noexcept {
    // merge namespaces
    merge_namespaces_in(this->global_namespace->as<xi_namespace_decl>());

    // resolve top level qnames
    dom_qname_resolver().visit(this->global_namespace, *this /* error log info */);

    // merge type decls
    // TODO: ...

    // merge function decls
    // TODO: ...

    // merge method decls
    // TODO: ...
    return true;
}

}
