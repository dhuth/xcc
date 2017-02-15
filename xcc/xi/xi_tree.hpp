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

struct xi_parameter_decl : public extend_tree<tree_type_id::xi_parameter_decl, ast_decl> {
public:

    enum class direction : uint32_t {
        byval,
        byref,
        in,
        out,
    };

    inline xi_parameter_decl(const char* name, ast_type* type, direction dir = direction::byval)
            : base_type(name),
              type(this, type),
              parameter_direction(this, dir),
              generated_parameter(this, nullptr) {
        //...
    }

    property<ast_type>                              type;
    property<direction>                             parameter_direction;
    property<ast_parameter_decl>                    generated_parameter;

};

struct xi_function_decl : public extend_tree<tree_type_id::xi_function_decl, ast_decl> {
public:

    inline xi_function_decl(const char* name, ast_type* rtype, list<xi_parameter_decl>* parameters)
            : base_type(name),
              return_type(this, rtype),
              parameters(this, parameters),
              body(this, new ast_block_stmt(new list<ast_local_decl>(), new list<ast_stmt>())),
              is_extern(this, false),
              is_extern_visible(this, true),
              generated_function(this, nullptr) {
        //...
    }

    property<ast_type>                              return_type;
    property<list<xi_parameter_decl>>               parameters;
    property<ast_block_stmt>                        body;
    property<bool>                                  is_extern;
    property<bool>                                  is_extern_visible;
    property<ast_function_decl>                     generated_function;
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