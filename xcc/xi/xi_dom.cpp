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
#include <vector>

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
    ptr<list<ast_decl>> dlist = b.find_all_declarations(t->name);
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


static void __merge_decls_in_namespace(xi_namespace_decl* ns, xi_builder& b, decl_swap_map_t&, const std::vector<tree_type_id>&);

template<typename           T,
         enable_if_forwardable_t<T, int> = 0>
static inline T* merge_forwardable(T* lhs, T* rhs, xi_builder&) {
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

template<typename       T,
         enable_if_forwardable_t<T, int> = 0>
static inline bool __is_forward(T* d) noexcept {
    return d->is_forward_decl;
}

static inline bool is_forward(ast_decl* d) noexcept {
    switch(d->get_tree_type()) {
    case tree_type_id::xi_struct_decl:
        return __is_forward(d->as<xi_struct_decl>());
    case tree_type_id::xi_function_decl:
    case tree_type_id::xi_operator_function_decl:
        return __is_forward(d->as<xi_function_decl>());
    case tree_type_id::xi_method_decl:
    case tree_type_id::xi_operator_method_decl:
        return __is_forward(d->as<xi_method_decl>());

    default:
        __throw_unhandled_tree_type(__FILE__, __LINE__, d, "is_forward");
    }
}

template<typename       T,
         enable_if_forwardable_t<T, int> = 0>
static inline void __setfwd(T* fwd, T* decl) noexcept {
    fwd->definition = decl;
}

static inline void setfwd(ast_decl* fwd, ast_decl* d) noexcept {
    switch(d->get_tree_type()) {
    case tree_type_id::xi_struct_decl:
        __setfwd(fwd->as<xi_struct_decl>(), d->as<xi_struct_decl>());
        break;
    case tree_type_id::xi_function_decl:
    case tree_type_id::xi_operator_function_decl:
        __setfwd(fwd->as<xi_function_decl>(), d->as<xi_function_decl>());
        break;
    case tree_type_id::xi_method_decl:
    case tree_type_id::xi_operator_method_decl:
        __setfwd(fwd->as<xi_method_decl>(), d->as<xi_method_decl>());
        break;

    default:
        __throw_unhandled_tree_type(__FILE__, __LINE__, d, "is_forward");
    }
}

static void merge_declarations_with(
        ast_decl* decl,
        list<ast_decl>::iterator_t iter_start,
        ptr<list<ast_decl>> decl_list,
        xi_builder& b,
        decl_swap_map_t& swapmap,
        const std::vector<tree_type_id>& treetypes) {

    ast_decl*               merged_decl = decl;
    ptr<list<ast_decl>>     fwdlist     = new list<ast_decl>();

    auto iter = iter_start;
    while(iter < decl_list->end()) {
        auto decl_other = *iter;

        if(b.samedecl(merged_decl, decl_other)) {
            if(is_forward(decl_other)) {
                merged_decl = merge_decl(merged_decl, decl_other, b);
                fwdlist->push_back(decl_other);

                decl_list->erase(iter);
                continue;
            }
        }
        iter++;
    }

    for(auto fiter = fwdlist->begin(); fiter < fwdlist->end(); fiter++) {
        setfwd(*fiter, merged_decl);
        swapmap[*fiter] = merged_decl;
    }
}

static inline bool __is_in(const std::vector<tree_type_id>& v, tree_type_id t) noexcept {
    return (std::find(std::begin(v), std::end(v), t) != std::end(v));
}

static void __merge_decls_in_namespace(
        xi_namespace_decl* ns,
        xi_builder& b,
        decl_swap_map_t& swapmap,
        const std::vector<tree_type_id>& treetypes) {
    ptr<list<ast_decl>>         decl_list   = ns->declarations;
    list<ast_decl>::iterator_t  decl_iter   = decl_list->begin();
    while(decl_iter < decl_list->end()) {
        auto decl = *decl_iter;
        if(__is_in(treetypes, decl->get_tree_type())) {
            merge_declarations_with(decl, decl_iter, decl_list, b, swapmap, treetypes);
        }
        else if(decl->is<xi_namespace_decl>()) {
            __merge_decls_in_namespace(decl->as<xi_namespace_decl>(), b, swapmap, treetypes);
        }

        decl_iter++;
    }
}

template<tree_type_id... _TreeTypes>
static inline void merge_decls_in_namespace(xi_namespace_decl* ns, xi_builder& b) {
    std::vector<tree_type_id> treetypes = { _TreeTypes... };
    decl_swap_map_t         swapmap;
    dom_swap_decl_walker    swap_decl_walker;

    __merge_decls_in_namespace(ns, b, swapmap, treetypes);
    for(auto miter: swapmap) {
        swap_decl_walker.set(miter.first, miter.second);
    }
    swap_decl_walker.visit(ns, b);
}


/* ===================== *
 * = Swap Declarations = *
 * ===================== */

void dom_swap_decl_walker::visit_xi_decl_type(xi_decl_type* dt, xi_builder& b) {
    dt->declaration = this->visit(dt->declaration, b)->as<ast_decl>();
}


#define MERGE_PASS_1_TYPES\
    tree_type_id::xi_struct_decl
#define MERGE_PASS_2_TYPES\
    tree_type_id::xi_function_decl,\
    tree_type_id::xi_operator_function_decl,\
    tree_type_id::xi_method_decl,\
    tree_type_id::xi_operator_method_decl

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


    // merge pass 1 (types)
    // --------------------
    merge_decls_in_namespace<MERGE_PASS_1_TYPES>(this->global_namespace->as<xi_namespace_decl>(), *this);


    // global struct implementation
    // ----------------------------------
    // TODO: ...


    // merge pass 2 (functions & methods)
    // ----------------------------------
    merge_decls_in_namespace<MERGE_PASS_2_TYPES>(this->global_namespace->as<xi_namespace_decl>(), *this);
    return true;
}

}
