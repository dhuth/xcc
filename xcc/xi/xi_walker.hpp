/*
 * xi_walker.hpp
 *
 *  Created on: Feb 5, 2018
 *      Author: derick
 */

#ifndef XCC_XI_XI_WALKER_HPP_
#define XCC_XI_XI_WALKER_HPP_

#include "ast.hpp"
#include <set>

namespace xcc {

struct xi_builder;

void maybe_push_context(ast_tree*, xi_builder& b);
void maybe_pop_context(ast_tree*, xi_builder& b);

template<typename... TParameters>
struct xi_postorder_walker : public dispatch_postorder_tree_walker<ast_tree, TParameters..., xi_builder&> {
public:

    inline ast_tree* operator()(ast_tree* t, TParameters... p, xi_builder& b) noexcept {
        return this->visit(t, p..., b);
    }

    void begin(tree_type_id, ast_tree* t, TParameters... p, xi_builder& b) override {
        maybe_push_context(t, p..., b);
    }
    void end(tree_type_id, ast_tree* t, TParameters... p, xi_builder& b) override {
        maybe_pop_context(t, p..., b);
    }
};

template<typename... TParameters>
struct xi_preorder_walker : public dispatch_preorder_tree_walker<ast_tree, TParameters..., xi_builder&> {
public:

    inline ast_tree* operator()(ast_tree* t, TParameters... p, xi_builder& b) noexcept {
        return this->visit(t, p..., b);
    }

    void begin(tree_type_id, ast_tree* t, TParameters... p, xi_builder& b) override {
        maybe_push_context(t, p..., b);
    }
    void end(tree_type_id, ast_tree* t, TParameters... p, xi_builder& b) override {
        maybe_pop_context(t, p..., b);
    }
};

template<typename TReturn, typename... TParameters>
using xi_visitor = dispatch_visitor<TReturn, TParameters..., xi_builder&>;

template<typename TReturn, typename... TParameters>
using xi_const_visitor = dispatch_visitor<TReturn, TParameters..., const xi_builder&>;

template<typename T> ptr<tree_list<T>> gather_nodes(ast_namespace_decl*);

template<typename T>
struct __gather_tt_walker : public dispatch_preorder_tree_walker<ast_tree, std::set<T*>&> {
public:

    explicit inline __gather_tt_walker() {
        this->add(&__gather_tt_walker<T>::visit_node);
    }

private:

    friend
    ptr<tree_list<T>> gather_nodes<T>(ast_namespace_decl*);

    void visit_node(ast_tree* t, std::set<T*>& s) {
        if(t->is<T>()) {
            s.insert(t->as<T>());
        }
    }

    static __gather_tt_walker<T>                            instance;
};

template<typename T>
inline ptr<tree_list<T>> gather_nodes(ast_namespace_decl* ns) {
    __gather_tt_walker<T> instance;
    std::set<T*> st;
    ptr<list<T>> olist = box(new list<T>());
    instance.visit(ns, st);
    for(auto d: st) {
        olist->push_back(d);
    }
    return olist;
}

}



#endif /* XCC_XI_XI_WALKER_HPP_ */
