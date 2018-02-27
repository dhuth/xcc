/*
 * ast_widen.hpp
 *
 *  Created on: Feb 27, 2018
 *      Author: derick
 */

#ifndef XCC_AST_TYPE_CONV_HPP_
#define XCC_AST_TYPE_CONV_HPP_

#include "ast.hpp"
#include "ast_builder.hpp"

namespace xcc {

struct ast_widen_func : public dispatch_visitor<ast_expr*, __Widen_arg_types> {
public:

    explicit ast_widen_func(const ast_builder_impl_t&) noexcept;

private:
#define __Decl_func(t)      ast_expr* to_##t##_type(t##_type*, __Widen_param_arg_types)
    __Decl_func(ast_void);
    __Decl_func(ast_integer);
    __Decl_func(ast_real);
    __Decl_func(ast_array);
    __Decl_func(ast_pointer);
    __Decl_func(ast_function);
    __Decl_func(ast_record);
#undef  __Decl_func
};


struct ast_widens_func : public dispatch_visitor<bool, __Widens_arg_types> {
public:

    explicit ast_widens_func(const ast_builder_impl_t&) noexcept;

private:
#define __Decl_func(t)      bool to_##t##_type(t##_type*, __Widens_param_arg_types)
    __Decl_func(ast_void);
    __Decl_func(ast_integer);
    __Decl_func(ast_real);
    __Decl_func(ast_array);
    __Decl_func(ast_pointer);
    __Decl_func(ast_function);
    __Decl_func(ast_record);
#undef  __Decl_func
};

}




#endif /* XCC_AST_TYPE_CONV_HPP_ */
