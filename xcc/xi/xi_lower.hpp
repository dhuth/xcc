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

struct xi_lower_walker : public dispatch_tree_postorder_walker<ast_tree> {
public:

    inline xi_lower_walker(xi_builder& builder)
            : _xi_builder(builder),
              _ast_builder(builder) {
        this->add(&xi_lower_walker::lower_op_expr);
        this->add(&xi_lower_walker::lower_cast_expr);
        this->add(&xi_lower_walker::lower_invoke_expr);

        this->add(&xi_lower_walker::lower_block_stmt);
        this->add(&xi_lower_walker::lower_assign_stmt);
    }

    virtual ~xi_lower_walker() = default;

    inline ast_decl*                                lower_decl(ast_decl* x) { return this->visit(x)->as<ast_decl>(); }
    inline ast_stmt*                                lower_stmt(ast_stmt* x) { return this->visit(x)->as<ast_stmt>(); }
    inline ast_expr*                                lower_expr(ast_expr* x) { return this->visit(x)->as<ast_expr>(); }
    inline ast_type*                                lower_type(ast_type* x) { return this->visit(x)->as<ast_type>(); }

protected:

    bool                                            begin(tree_type_id, base_tree_type*) override final;
    bool                                            end(tree_type_id, base_tree_type*) override final;

private:

    ast_expr*                                       lower_op_expr(xi_op_expr*);
    ast_expr*                                       lower_cast_expr(ast_cast*);
    ast_expr*                                       lower_invoke_expr(ast_invoke*);

    void                                            begin_block(ast_block_stmt*);
    void                                            end_block(ast_block_stmt*);

    ast_stmt*                                       lower_block_stmt(ast_block_stmt*);
    ast_stmt*                                       lower_assign_stmt(xi_assign_stmt*);

    xi_builder&                                                         _xi_builder;
    ast_builder<>&                                                      _ast_builder;
};

}


#endif /* XI_XI_LOWER_HPP_ */
