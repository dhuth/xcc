/*
 * xi_semantic.hpp
 *
 *  Created on: Jan 24, 2018
 *      Author: derick
 */

#ifndef XCC_XI_XI_SEMANTIC_HPP_
#define XCC_XI_XI_SEMANTIC_HPP_

#include "xi_builder.hpp"

namespace xcc {

struct xi_expr_tcvisitor;
struct xi_decl_tcwalker;


struct xi_expr_tcvisitor : public dispatch_visitor<ast_expr*, xi_builder&> {
public:

    explicit xi_expr_tcvisitor(xi_decl_tcwalker&) noexcept;
    ~xi_expr_tcvisitor() = default;

private:

    ast_expr* tc_xi_id_expr(xi_id_expr*,                xi_builder&);
    ast_expr* tc_xi_member_id_expr(xi_member_id_expr*,  xi_builder&);
    ast_expr* tc_xi_tuple_expr(xi_tuple_expr*,          xi_builder&);

    xi_decl_tcwalker&                                       decl_walker;

};

struct xi_decl_tcwalker : public xi_postorder_walker<xi_expr_tcvisitor&> {
public:

    explicit xi_decl_tcwalker() noexcept;
    ~xi_decl_tcwalker() = default;

private:

};

}

#endif /* XCC_XI_XI_SEMANTIC_HPP_ */
