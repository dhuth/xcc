/*
 * ast_widen.hpp
 *
 *  Created on: Feb 27, 2018
 *      Author: derick
 */

#ifndef XCC_AST_TYPE_FUNC_HPP_
#define XCC_AST_TYPE_FUNC_HPP_

#include "ast_builder.hpp"

namespace xcc {

// Helper macros
#define TT_NAME(t)                      t##_type
#define TT_FUNC_NAME(p, t)              p##_##t##_type

#define SAMETYPE_FUNC_NAME(t)           TT_FUNC_NAME(same_as, t)
#define WIDENS_FUNC_NAME(t)             TT_FUNC_NAME(to, t)
#define COERCABLE_FUNC_NAME(t)          TT_FUNC_NAME(to, t)
#define MAXTYPE_FUNC_NAME(t)            TT_FUNC_NAME(max, t)
#define CAST_FUNC_NAME(t)               TT_FUNC_NAME(to, t)


/**
 * The base sametype function
 */
struct ast_sametype_func : public dispatch_visitor<bool, __Sametype_param_arg_types> {
public:

    explicit ast_sametype_func(const ast_builder_impl_t& b) noexcept;

protected:

    bool sametype_list(ref<list<ast_type>>, ref<list<ast_type>>) noexcept;

private:
#define __Decl_func(t)      bool SAMETYPE_FUNC_NAME(t)(TT_NAME(t)*, __Sametype_param_arg_types)
    __Decl_func(ast_void);
    __Decl_func(ast_integer);
    __Decl_func(ast_real);
    __Decl_func(ast_array);
    __Decl_func(ast_pointer);
    __Decl_func(ast_function);
    __Decl_func(ast_record);
#undef  __Decl_func

protected:

    ast_builder_impl_t&                                     _builder;
};


/**
 * The base widens function
 */
struct ast_widens_func : public dispatch_visitor<bool, __Widens_param_arg_types> {
public:

    explicit ast_widens_func(const ast_builder_impl_t& b) noexcept;

private:
#define __Decl_func(t)      bool WIDENS_FUNC_NAME(t)(TT_NAME(t)*, __Widens_param_arg_types)
    __Decl_func(ast_void);
    __Decl_func(ast_integer);
    __Decl_func(ast_real);
    __Decl_func(ast_array);
    __Decl_func(ast_pointer);
    __Decl_func(ast_function);
    __Decl_func(ast_record);
#undef  __Decl_func

protected:
    ast_builder_impl_t&                                     _builder;
};


/**
 * The base coercable function
 */
struct ast_coercable_func : public dispatch_visitor<bool, __Coercable_param_arg_types> {
public:

    explicit ast_coercable_func(const ast_builder_impl_t& b) noexcept;

private:
#define __Decl_func(t)      bool COERCABLE_FUNC_NAME(t)(TT_NAME(t)*, __Coercable_param_arg_types)
    __Decl_func(ast_void);
    __Decl_func(ast_integer);
    __Decl_func(ast_real);
    __Decl_func(ast_array);
    __Decl_func(ast_pointer);
    __Decl_func(ast_function);
    __Decl_func(ast_record);
#undef __Decl_func

protected:
    ast_builder_impl_t&                                     _builder;
};


/**
 * The base maxtype function
 */
struct ast_maxtype_func : public dispatch_visitor<ast_type*, __Maxtype_param_arg_types> {
public:

    explicit ast_maxtype_func(const ast_builder_impl_t& b) noexcept;

private:
#define __Decl_func(t)      ast_type* MAXTYPE_FUNC_NAME(t)(TT_NAME(t)*, __Maxtype_param_arg_types)
    __Decl_func(ast_integer);
    __Decl_func(ast_real);
    __Decl_func(ast_pointer);
#undef __Decl_func

protected:
    ast_builder_impl_t&                                     _builder;
};


/**
 * The base cast function
 */
struct ast_cast_func : public dispatch_visitor<ast_expr*, __Cast_param_arg_types> {
public:

    explicit ast_cast_func(const ast_builder_impl_t& b) noexcept;

private:
#define __Decl_func(t)      ast_expr* CAST_FUNC_NAME(t)(TT_NAME(t)*, __Cast_param_arg_types)
    __Decl_func(ast_integer);
    __Decl_func(ast_real);
    __Decl_func(ast_pointer);
#undef __Decl_func

protected:
    ast_builder_impl_t&                                     _builder;
};


// Helper macros
#define SAMETYPE_METHOD_NAME(t)             ast_sametype_func   ::  SAMETYPE_FUNC_NAME(t)
#define WIDENS_METHOD_NAME(t)               ast_widens_func     ::  WIDENS_FUNC_NAME(t)
#define COERCABLE_METHOD_NAME(t)            ast_coercable_func  ::  COERCABLE_FUNC_NAME(t)
#define MAXTYPE_METHOD_NAME(t)              ast_maxtype_func    ::  MAXTYPE_FUNC_NAME(t)
#define CAST_METHOD_NAME(t)                 ast_cast_func       ::  CAST_FUNC_NAME(t)

#define SAMETYPE_FUNC(t, tt, rh)            bool        SAMETYPE_METHOD_NAME(t)     (TT_NAME(t)* tt, __Sametype_param_args(rh))
#define WIDENS_FUNC(t, tt, fe, c)           bool        WIDENS_METHOD_NAME(t)       (TT_NAME(t)* tt, __Widens_param_args(fe, c))
#define COERCABLE_FUNC(t, tt, fe, c)        bool        COERCABLE_METHOD_NAME(t)    (TT_NAME(t)* tt, __Coercable_param_args(fe, c))
#define MAXTYPE_FUNC(t, tt, rh)             ast_type*   MAXTYPE_METHOD_NAME(t)      (TT_NAME(t)* tt, __Maxtype_param_args(rh))
#define CAST_FUNC(t, tt, fe)                ast_expr*   CAST_METHOD_NAME(t)         (TT_NAME(t)* tt, __Cast_param_args(fe))

}




#endif /* XCC_AST_TYPE_FUNC_HPP_ */
