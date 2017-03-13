/*
 * xi_pass_typecheck.hpp
 *
 *  Created on: Mar 11, 2017
 *      Author: derick
 */

#ifndef XI_XI_PASS_TYPECHECK_HPP_
#define XI_XI_PASS_TYPECHECK_HPP_

#include "xi_tree.hpp"
#include "xi_pass.hpp"

namespace xcc {

struct xi_top_down_typecheck_pass : public xi_preorder_pass {
public:

    inline xi_top_down_typecheck_pass(xi_builder& builder)
            : xi_preorder_pass(builder) {
        //...
    }

};

struct xi_bottom_up_typecheck_pass : public xi_postorder_pass {
public:

    inline xi_bottom_up_typecheck_pass(xi_builder& builder)
        : xi_postorder_pass(builder) {

        this->add(&xi_bottom_up_typecheck_pass::check_named_memberref_expr);
        this->add(&xi_bottom_up_typecheck_pass::check_op_expr);
        this->add(&xi_bottom_up_typecheck_pass::check_index_expr);
        this->add(&xi_bottom_up_typecheck_pass::check_invoke_expr);
    }

    virtual void postvisit(tree_type_id, ast_tree*) override final;

    ast_expr* check_named_memberref_expr(xi_named_memberref_expr*);
    ast_expr* check_static_named_memberref_expr(xi_static_named_memberref_expr*);
    ast_expr* check_index_expr(xi_index_expr*);
    ast_expr* check_op_expr(xi_op_expr*);
    ast_expr* check_invoke_expr(ast_invoke*);
};

struct xi_typecheck_pass {
public:

    inline xi_typecheck_pass(xi_builder& builder)
            : _top_down(builder),
              _bottom_up(builder) {
        //...
    }

    inline void visit(ast_namespace_decl* gns) {
        //this->_top_down.visit(gns);
        this->_bottom_up.visit(gns);
    }

private:

    xi_top_down_typecheck_pass                                          _top_down;
    xi_bottom_up_typecheck_pass                                         _bottom_up;

};

}


#endif /* XI_XI_PASS_TYPECHECK_HPP_ */
