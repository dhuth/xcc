/*
 * xi_semantic.cpp
 *
 *  Created on: Nov 22, 2017
 *      Author: derick
 */


#include "xi_dom.hpp"
#include "xi_builder.hpp"
#include "error.hpp"
#include <map>

namespace xcc {


/* ==================== *
 * = Merge Namespaces = *
 * ==================== */

static void merge_namespaces_in(xi_namespace_decl*) noexcept;

/**
 * Helper function for merge_namespaces
 * Move declarations in each namespace in list with the same name as ns, into ns.
 * @param ns        The destination namespace
 * @param rest      An iterator in list, just after the destination namespace
 * @param list      The list of declarations to search for similarly named namespaces
 */
static void merge_namespaces_into(xi_namespace_decl* ns, list<ast_decl>::iterator_t rest, list<ast_decl>* list) {
    auto itr = rest;
    while(itr != list->end()) {
        auto decl = *itr;
        if(decl->is<xi_namespace_decl>() && (decl->name == ns->name)) {
            list->erase(itr);
            for(auto d: decl->as<xi_namespace_decl>()->declarations) {
                ns->declarations->push_back(d);
            }
        }
        else {
            itr++;
        }
    }
    merge_namespaces_in(ns);
}


/**
 * Merge all namespaces in this list of declarations
 * @param decls
 */
static void merge_namespaces(list<ast_decl>* decls) noexcept {
    list<ast_decl>::iterator_t itr = decls->begin();

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

/**
 * Merge all namespaces inside of this namespace
 * @param ns
 */
static void merge_namespaces_in(xi_namespace_decl* ns) noexcept {
    merge_namespaces(ns->declarations);
}


/* ======================= *
 * = Declaration Nesting = *
 * ======================= */

void dom_nesting_walker::visit_xi_namespace_decl(xi_namespace_decl* nsdecl, xi_builder& b) {
    nsdecl->ns          = b.get_nesting_namespace();
}

void dom_nesting_walker::visit_xi_function_decl(xi_function_decl* fdecl, xi_builder& b) {
    fdecl->ns           = b.get_nesting_namespace();
}

void dom_nesting_walker::visit_xi_member_decl(xi_member_decl* mdecl, xi_builder& b) {
    mdecl->ns           = b.get_nesting_namespace();
    mdecl->parent       = b.get_nesting_type();
}


/* =================== *
 * = Name Resolution = *
 * =================== */

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


/* ============================ *
 * = Merging Declarations Set = *
 * ============================ */

typedef std::map<ast_decl*,ptr<ast_decl>>                   decl_swap_map_t;


static void merge_decls_in_namespace(xi_namespace_decl* ns, xi_builder& b, decl_swap_map_t&);

template<typename T>
static inline ast_decl* merge_forwardable(T* lhs, T* rhs, xi_builder&) {
    if(lhs->is_forward_decl) {
        return rhs;
    }
    //TODO: throw redeclaration error
    assert(false);
    return rhs;
}

static ast_decl* merge_decl(ast_decl* lhs, ast_decl* rhs, xi_builder& b) {
    if(lhs == rhs) return lhs;

    switch(lhs->get_tree_type()) {
    case tree_type_id::xi_struct_decl:
        return merge_forwardable(lhs->as<xi_struct_decl>(), rhs->as<xi_struct_decl>(), b);

    case tree_type_id::xi_function_decl:
    case tree_type_id::xi_operator_function_decl:
        return merge_forwardable(lhs->as<xi_function_decl>(), rhs->as<xi_function_decl>(), b);

    case tree_type_id::xi_method_decl:
    case tree_type_id::xi_operator_method_decl:
        return merge_forwardable(lhs->as<xi_method_decl>(), rhs->as<xi_method_decl>(), b);

    default:
        __throw_unhandled_tree_type(__FILE__, __LINE__, lhs, "merge_decl");
    }
}

static ast_decl* merge_declarations_with(ast_decl* decl, list<ast_decl>::iterator_t iter, ptr<list<ast_decl>> decl_list, xi_builder& b, decl_swap_map_t& swapmap) {
    while(iter < decl_list->end()) {
        auto decl_other = *iter;
        if(b.samedecl(decl, decl_other)) {
            decl = merge_decl(decl, decl_other, b);
            decl_list->erase(iter);
        }
        else {
            if(decl->is<xi_namespace_decl>()) {
                merge_decls_in_namespace(decl->as<xi_namespace_decl>(), b, swapmap);
            }
            iter++;
        }
    }
    return decl;
}

static void merge_decls_in_namespace(xi_namespace_decl* ns, xi_builder& b, decl_swap_map_t& swapmap) {
    ptr<list<ast_decl>>         decl_list   = ns->declarations;
    list<ast_decl>::iterator_t  decl_iter   = decl_list->begin();
    while(decl_iter < decl_list->end()) {
        auto old_decl = *decl_iter;
        auto new_decl = merge_declarations_with(old_decl, decl_iter + 1, decl_list, b, swapmap);

        if(old_decl != new_decl) {
            decl_list->erase(decl_iter);
            decl_list->insert(decl_iter, new_decl);

            swapmap[old_decl] = new_decl;
        }

        decl_iter++;
    }
}


/* ===================== *
 * = Swap Declarations = *
 * ===================== */

void dom_swap_decl_walker::visit_xi_decl_type(xi_decl_type* dt, xi_builder& b) {
    dt->declaration = this->visit(dt->declaration, b)->as<ast_decl>();
}



bool xi_builder::dom_pass(/*options & error log info*/) noexcept {
    dom_qname_resolver      type_name_resolver;


    // merge namespaces
    // ----------------
    merge_namespaces_in(this->global_namespace->as<xi_namespace_decl>());


    // nesting
    // -------
    dom_nesting_walker      nesting_walker;
    nesting_walker(this->global_namespace->as<xi_namespace_decl>(), *this);


    // resolve top level qnames
    // ------------------------
    type_name_resolver(this->global_namespace, *this /* error log info */);


    // merge decls
    // -----------
    decl_swap_map_t         swapmap;
    merge_decls_in_namespace(this->global_namespace->as<xi_namespace_decl>(), *this, swapmap);


    // swap decls
    // ----------
    dom_swap_decl_walker    swap_decl_walker;
    for(auto miter: swapmap) {
        swap_decl_walker.set(miter.first, miter.second);
    }
    swap_decl_walker.visit(this->global_namespace, *this);

    return true;
}

}
