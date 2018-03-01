/*
 * xi_semantic.hpp
 *
 *  Created on: Jan 30, 2018
 *      Author: derick
 */

#ifndef XCC_XI_XI_SEMANTIC_HPP_
#define XCC_XI_XI_SEMANTIC_HPP_

#include "xi_builder.hpp"
#include "xi_walker.hpp"

#include <functional>

namespace xcc {

struct xi_tc_walker : public xi_postorder_walker<> {
public:

    explicit xi_tc_walker() noexcept;

private:

    void                visit_xi_function_decl(xi_function_decl*,                       xi_builder&);
    void                visit_xi_method_decl(xi_method_decl*,                           xi_builder&);
    void                visit_xi_operator_function_decl(xi_operator_function_decl*,     xi_builder&);
    void                visit_xi_operator_method_decl(xi_operator_method_decl*,         xi_builder&);

    ast_expr*           tc_id_expr(xi_id_expr*,                                         xi_builder&);
    ast_expr*           tc_member_id_expr(xi_member_id_expr*,                           xi_builder&);
    ast_expr*           tc_op_expr(xi_op_expr*,                                         xi_builder&);
    ast_expr*           tc_invoke_expr(xi_invoke_expr*,                                 xi_builder&);

    ast_stmt*           tc_block_stmt(ast_block_stmt*,                                  xi_builder&);
    ast_stmt*           tc_return_stmt(ast_return_stmt*,                                xi_builder&);

};

ast_expr* tc_single_expr(ast_expr*, xi_builder&);
ast_expr* tc_maybe_cast(ast_type*, ast_expr*, xi_builder&);

}


#endif /* XCC_XI_XI_SEMANTIC_HPP_ */
