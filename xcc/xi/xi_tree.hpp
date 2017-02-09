/*
 * xi_tree.hpp
 *
 *  Created on: Feb 6, 2017
 *      Author: derick
 */

#ifndef XI_XI_TREE_HPP_
#define XI_XI_TREE_HPP_

#include "ast.hpp"

namespace xcc {

enum class xi_operator : uint32_t {
    none,
    dot,
    add,
    sub,
    mul,
    div,
    mod,
    shl,
    shr,
    eq,
    ne,
    lt,
    le,
    gt,
    ge,
    assign,
    assign_add,
    assign_sub,
    assign_mul,
    assign_div,
    assign_mod,
    assign_shl,
    assign_shr
};

struct xi_const_type : public extend_tree<tree_type_id::xi_const_type, ast_type> {
public:

    inline xi_const_type(ast_type* type)
            : base_type(),
              type(this, type) {
        //...
    }

    property<ast_type>                              type;

};

struct xi_op_expr : public extend_tree<tree_type_id::xi_op_expr, ast_expr> {
public:

    inline xi_op_expr(ast_type* type, xi_operator op, list<ast_expr>* operands)
            : base_type(type),
              op(this, op),
              operands(this, operands){
        //...
    }

    property<xi_operator>                           op;
    property<list<ast_expr>>                        operands;
};



}



#endif /* XI_XI_TREE_HPP_ */
