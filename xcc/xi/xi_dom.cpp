/*
 * xi_semantic.cpp
 *
 *  Created on: Nov 22, 2017
 *      Author: derick
 */


#include "xi_dom.hpp"
#include "error.hpp"
#include <map>

namespace xcc {

static void merge_namespaces_in(xi_namespace_decl*) noexcept;

/**
 * Helper function for merge_namespaces
 * Move declarations in each namespace in list with the same name as ns, into ns.
 * @param ns        The destination namespace
 * @param rest      An iterator in list, just after the destination namespace
 * @param list      The list of declarations to search for similarly named namespaces
 */
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


/**
 * Merge all namespaces in this list of declarations
 * @param decls
 */
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

/**
 * Merge all namespaces inside of this namespace
 * @param ns
 */
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


typedef std::map<ast_decl*,ptr<ast_decl>>                   decl_swap_map_t;

static bool same_type(ast_type*, ast_type*, xi_builder&);
static bool same_decl(ast_decl*, ast_decl*, xi_builder&);

static bool same_type(ast_type* lhs, ast_type* rhs, xi_builder& b) {
    //TODO: handle aliases
    return b.sametype(lhs, rhs);
}

static bool same_rt_and_parameters(xi_function_base* lhs, xi_function_base* rhs, xi_builder& b) {
    //TODO: generic parameters and specializations
    //TODO: variadic functions

    // same return type
    if(!same_type(lhs->return_type, rhs->return_type, b)) return false;
    // same number of parameters
    if(lhs->parameters->size() != rhs->parameters->size()) return false;
    // same parameter types
    auto lhs_iter = lhs->parameters.begin();
    auto rhs_iter = rhs->parameters.begin();
    while(lhs_iter != lhs->parameters.end()) {

        if(!same_type((*lhs_iter)->type, (*rhs_iter)->type, b)) {
            return false;
        }

        lhs_iter++;
        rhs_iter++;
    }
    return true;
}

static bool same_struct(xi_struct_decl* lhs, xi_struct_decl* rhs, xi_builder& b) {
    assert(lhs->name == rhs->name);
    //TODO: generics and specializations
    return true;
}

static inline bool same_function(xi_function_decl* lhs, xi_function_decl* rhs, xi_builder& b) {
    assert(lhs->name == rhs->name);
    return same_rt_and_parameters(lhs, rhs, b);
}

static inline bool same_operator_function(xi_operator_function_decl* lhs, xi_operator_function_decl* rhs, xi_builder& b) {
    assert(lhs->name == rhs->name);
    return same_rt_and_parameters(lhs, rhs, b);
}

static inline bool same_method(xi_method_decl* lhs, xi_method_decl* rhs, xi_builder& b) {
    assert(lhs->name == rhs->name);
    return same_rt_and_parameters(lhs, rhs, b);
}

static inline bool same_operator_method(xi_operator_method_decl* lhs, xi_operator_method_decl* rhs, xi_builder& b) {
    assert(lhs->name == rhs->name);
    return same_rt_and_parameters(lhs, rhs, b);
}


//TODO: promote to xi_builder method
static bool same_decl(ast_decl* lhs, ast_decl* rhs, xi_builder& b) {
    if(lhs == rhs)                                      return true;
    if(lhs->get_tree_type() != rhs->get_tree_type())    return false;   //TODO: might not be right
    if(lhs->name            != rhs->name)               return false;

    switch(lhs->get_tree_type()) {
    case tree_type_id::xi_struct_decl:
        return same_struct(
                lhs->as<xi_struct_decl>(),
                rhs->as<xi_struct_decl>(), b);
    case tree_type_id::xi_function_decl:
        return same_function(
                lhs->as<xi_function_decl>(),
                rhs->as<xi_function_decl>(), b);
    case tree_type_id::xi_operator_function_decl:
        return same_operator_function(
                lhs->as<xi_operator_function_decl>(),
                rhs->as<xi_operator_function_decl>(), b);
    case tree_type_id::xi_method_decl:
        return same_method(
                lhs->as<xi_method_decl>(),
                rhs->as<xi_method_decl>(), b);
    case tree_type_id::xi_operator_method_decl:
        return same_operator_method(
                lhs->as<xi_operator_method_decl>(),
                rhs->as<xi_operator_method_decl>(), b);
    case tree_type_id::ast_variable_decl:
        return true;
    default:
        __throw_unhandled_tree_type(__FILE__, __LINE__, lhs, "same_decl");
    }
}

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

static ast_decl* merge_declarations_with(ast_decl* decl, list<ast_decl>::iterator iter, ptr<list<ast_decl>> decl_list, xi_builder& b, decl_swap_map_t& swapmap) {
    while(iter < decl_list->end()) {
        auto decl_other = *iter;
        if(same_decl(decl, decl_other, b)) {
            decl = merge_decl(decl, decl_other, b);
            decl_list->remove(iter);
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
    list<ast_decl>::iterator    decl_iter   = decl_list->begin();
    while(decl_iter < decl_list->end()) {
        auto old_decl = *decl_iter;
        auto new_decl = merge_declarations_with(old_decl, decl_iter + 1, decl_list, b, swapmap);

        if(old_decl != new_decl) {
            decl_list->remove(decl_iter);
            decl_list->insert(decl_iter, new_decl);

            swapmap[old_decl] = new_decl;
        }

        decl_iter++;
    }
}


static dom_qname_resolver       type_name_resolver;

bool xi_builder::dom_pass(/*options & error log info*/) noexcept {
    // merge namespaces
    // ----------------
    merge_namespaces_in(this->global_namespace->as<xi_namespace_decl>());


    // resolve top level qnames
    // ------------------------
    type_name_resolver(this->global_namespace, *this /* error log info */);


    // merge decls
    // -----------
    decl_swap_map_t         swapmap;
    xi_postorder_walker<>   merge_walker;
    merge_decls_in_namespace(this->global_namespace->as<xi_namespace_decl>(), *this, swapmap);
    for(auto miter: swapmap) {
        merge_walker.set(miter.first, miter.second);
    }
    merge_walker.visit(this->global_namespace, *this);

    // assign parent types to members
    // ------------------------------

    // TODO: set member parent types
    return true;
}

}