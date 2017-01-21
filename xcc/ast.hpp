/*
 * ast.hpp
 *
 *  Created on: Jan 20, 2017
 *      Author: derick
 */

#ifndef AST_HPP_
#define AST_HPP_

#include "tree.hpp"

namespace xcc {

struct ast_tree : public extend_tree<tree_type_id::ast_tree> {
public:

    inline ast_tree(tree_type_id id) noexcept : base_type(id) {
        //...
    }

};

struct ast_type : public extend_tree<tree_type_id::ast_type, ast_tree> {
public:

    inline ast_type(tree_type_id id) noexcept : base_type(id) {
        //...
    }

};

struct ast_decl : public extend_tree<tree_type_id::ast_decl, ast_tree> {
public:

    inline ast_decl(tree_type_id id) noexcept : base_type(id) {
        //...
    }

};

struct ast_expr : public extend_tree<tree_type_id::ast_expr, ast_tree> {
public:

    inline ast_expr(tree_type_id id) noexcept : base_type(id) {
        //...
    }

};

struct ast_stmt : public extend_tree<tree_type_id::ast_stmt, ast_tree> {
public:

    inline ast_stmt(tree_type_id id) noexcept : base_type(id) {
        //...
    }

};


struct ast_void_type final : public extend_tree<tree_type_id::ast_void_type, ast_type> {
public:

    inline ast_void_type() : base_type() {
        //...
    }

};


struct ast_integer_type final : public extend_tree<tree_type_id::ast_integer_type, ast_type> {

    inline ast_integer_type(uint32_t bitwidth, bool is_unsigned) noexcept
            : base_type(),
              bitwidth(this, bitwidth),
              is_unsigned(this, is_unsigned) {
        //...
    }

    property<uint32_t>                                          bitwidth;
    property<bool>                                              is_unsigned;

};

struct ast_real_type final : public extend_tree<tree_type_id::ast_real_type, ast_type> {
public:

    inline ast_real_type(uint32_t bitwidth) noexcept
            : base_type(),
              bitwidth(this, bitwidth) {
        //...
    }

    property<uint32_t>                                          bitwidth;

};

struct ast_array_type final : public extend_tree<tree_type_id::ast_array_type, ast_type> {
public:

    inline ast_array_type(ast_type* element_type, uint64_t size)
            : base_type(),
              element_type(this, element_type),
              size(this, size) {
        //...
    }

    property<ast_type>                                          element_type;
    property<uint64_t>                                          size;

};

struct ast_pointer_type final : public extend_tree<tree_type_id::ast_pointer_type, ast_type> {
public:

    inline ast_pointer_type(ast_type* element_type)
            : base_type(),
              element_type(this, element_type) {
        //...
    }

    property<ast_type>                                          element_type;

};

struct ast_function_type final : public extend_tree<tree_type_id::ast_function_type, ast_type> {
public:

    inline ast_function_type(ast_type* return_type, list<ast_type>* parameter_types)
            : base_type(),
              return_type(this, return_type),
              parameter_types(this, parameter_types) {
        //...
    }

    property<ast_type>                                          return_type;
    property<list<ast_type>>                                    parameter_types;

};



}




#endif /* AST_HPP_ */
