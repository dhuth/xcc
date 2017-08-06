/*
 * xi_pass.hpp
 *
 *  Created on: Mar 7, 2017
 *      Author: derick
 */

#ifndef XI_XI_PASS_HPP_
#define XI_XI_PASS_HPP_

#include <stack>
#include "xi_tree.hpp"

namespace xcc {

struct xi_builder;

typedef xcc::dispatch_visitor<void>                                     xi_pass_base_t;
typedef xcc::dispatch_postorder_tree_walker<ast_tree>                   xi_postorder_pass_base_t;
typedef xcc::dispatch_preorder_tree_walker<ast_tree>                    xi_preorder_pass_base_t;

template<typename TBaseType>
struct __xi_pass : public TBaseType {
public:

    inline __xi_pass(xi_builder& builder) : builder(builder) {
        this->namespace_stack.push(nullptr);
        this->current_namespace = nullptr;
    }
    virtual ~__xi_pass() = default;

    void push_namespace(ast_namespace_decl* decl) {
        this->namespace_stack.push(decl);
        this->current_namespace = decl;
    }

    void pop_namespace() {
        this->namespace_stack.pop();
        this->current_namespace = this->namespace_stack.top();
    }

protected:

    typedef __xi_pass<TBaseType>                                        base_t;
    xi_builder&                                                         builder;
    std::stack<ptr<ast_namespace_decl>>                                 namespace_stack;
    ptr<ast_namespace_decl>                                             current_namespace;

};

struct xi_postorder_pass : public __xi_pass<xi_postorder_pass_base_t> {
public:

    inline xi_postorder_pass(xi_builder& builder): base_t(builder) { }
    virtual ~xi_postorder_pass() = default;

    virtual void begin(tree_type_id id, ast_tree* t) override final;
    virtual void end(tree_type_id, ast_tree* t)      override final;

};

struct xi_preorder_pass : public __xi_pass<xi_preorder_pass_base_t> {
public:

    inline xi_preorder_pass(xi_builder& builder): base_t(builder) { }
    virtual ~xi_preorder_pass() = default;

    virtual void begin(tree_type_id id, ast_tree* t) override final;
    virtual void end(tree_type_id, ast_tree* t)      override final;

};

}


#endif /* XI_XI_PASS_HPP_ */
