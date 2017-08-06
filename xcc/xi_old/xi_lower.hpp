/*
 * xi_lower.hpp
 *
 *  Created on: Feb 22, 2017
 *      Author: derick
 */

#ifndef XI_XI_LOWER_HPP_
#define XI_XI_LOWER_HPP_

#include "xi_builder.hpp"

namespace xcc {

struct xi_lower_walker : public dispatch_postorder_tree_walker<ast_tree> {
public:

    inline xi_lower_walker(xi_builder& builder)
            : _xi_builder(builder),
              _ast_builder(builder) {
        this->add(&xi_lower_walker::lower_const_type);
        this->add(&xi_lower_walker::lower_object_type);
        this->add(&xi_lower_walker::lower_array_type);
        this->add(&xi_lower_walker::lower_ref_type);

        this->add(&xi_lower_walker::lower_op_expr);
        this->add(&xi_lower_walker::lower_zero_initializer_expr);
        this->add(&xi_lower_walker::lower_cast_expr);
        this->add(&xi_lower_walker::lower_invoke_expr);
        this->add(&xi_lower_walker::lower_index_expr);
        this->add(&xi_lower_walker::lower_bound_memberref_expr);

        this->add(&xi_lower_walker::lower_block_stmt);
        this->add(&xi_lower_walker::lower_assign_stmt);
    }

    virtual ~xi_lower_walker() = default;

    inline ast_decl*                                lower_decl(ast_decl* x) { return this->visit(x)->as<ast_decl>(); }
    inline ast_stmt*                                lower_stmt(ast_stmt* x) { return this->visit(x)->as<ast_stmt>(); }
    inline ast_expr*                                lower_expr(ast_expr* x) { return this->visit(x)->as<ast_expr>(); }
    inline ast_type*                                lower_type(ast_type* x) { return this->visit(x)->as<ast_type>(); }

protected:

    void                                            begin(tree_type_id, ast_tree*) override final;
    void                                            end(tree_type_id, ast_tree*) override final;

private:

    ast_type*                                       lower_const_type(xi_const_type*);
    ast_type*                                       lower_object_type(xi_object_type*);
    ast_type*                                       lower_array_type(xi_array_type*);
    ast_type*                                       lower_ref_type(xi_ref_type*);

    ast_expr*                                       lower_op_expr(xi_op_expr*);
    ast_expr*                                       lower_zero_initializer_expr(xi_zero_initializer_expr*);
    ast_expr*                                       lower_cast_expr(ast_cast*);
    ast_expr*                                       lower_invoke_expr(ast_invoke*);
    ast_expr*                                       lower_index_expr(xi_index_expr*);
    ast_expr*                                       lower_bound_memberref_expr(xi_bound_memberref_expr*);

    void                                            begin_block(ast_block_stmt*);
    void                                            end_block(ast_block_stmt*);

    ast_stmt*                                       lower_block_stmt(ast_block_stmt*);
    ast_stmt*                                       lower_assign_stmt(xi_assign_stmt*);

    xi_builder&                                                         _xi_builder;
    xi_builder::base_builder_t&                                         _ast_builder;
};

}


#endif /* XI_XI_LOWER_HPP_ */
