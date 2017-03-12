/*
 * xi_pass.hpp
 *
 *  Created on: Mar 7, 2017
 *      Author: derick
 */

#ifndef XI_XI_PASS_HPP_
#define XI_XI_PASS_HPP_


#include "xi_tree.hpp"

namespace xcc {

struct xi_builder;

typedef xcc::dispatch_visitor<void>                                     xi_pass_base_t;
typedef xcc::dispatch_postorder_tree_walker<ast_tree>                   xi_postorder_pass_base_t;
typedef xcc::dispatch_preorder_tree_walker<ast_tree>                    xi_preorder_pass_base_t;

struct xi_postorder_pass : public xi_postorder_pass_base_t {
public:

    inline xi_postorder_pass(xi_builder& builder): builder(builder) { }
    virtual ~xi_postorder_pass() = default;

    virtual void begin(tree_type_id, ast_tree*);
    virtual void end(tree_type_id, ast_tree*);

protected:

    xi_builder&                                                         builder;

};

struct xi_preorder_pass : public xi_preorder_pass_base_t {
public:

    inline xi_preorder_pass(xi_builder& builder): builder(builder) { }
    virtual ~xi_preorder_pass() = default;

    virtual void begin(tree_type_id, ast_tree*);
    virtual void end(tree_type_id, ast_tree*);

protected:

    xi_builder&                                                         builder;

};

}


#endif /* XI_XI_PASS_HPP_ */
