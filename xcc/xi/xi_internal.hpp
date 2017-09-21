#ifndef XI_INTERNAL_HPP_
#define XI_INTERNAL_HPP_

#include "xi_tree.hpp"
#include "xi_builder.hpp"

typedef xcc::xi_op_expr::xi_operator                                operator_t;
typedef xcc::ast_type                                               type_t;
typedef xcc::ast_expr                                               expr_t;
typedef xcc::ast_decl                                               decl_t;
typedef xcc::ast_stmt                                               stmt_t;

typedef xcc::list<std::string>                                      id_list_t;
typedef xcc::list<xcc::ast_expr>                                    expr_list_t;
typedef xcc::list<xcc::ast_decl>                                    decl_list_t;
typedef xcc::list<xcc::ast_stmt>                                    stmt_list_t;
typedef xcc::list<xcc::ast_type>                                    type_list_t;

typedef xcc::xi_builder                                             xi_builder_t;

#endif
