/*
 * xi_widen.hpp
 *
 *  Created on: Feb 27, 2018
 *      Author: derick
 */

#ifndef XCC_XI_XI_TYPE_FUNC_HPP_
#define XCC_XI_XI_TYPE_FUNC_HPP_

#include "ast_type_func.hpp"

namespace xcc {

struct xi_builder;

#define XI_SAMETYPE_FUNC_NAME(t)                            xi_same_as##t##_type
#define XI_WIDENS_FUNC_NAME(t)                              xito_##t##_type
#define XI_CAST_FUNC_NAME(t)                                xito_##t##_type

/**
 * The xi language sametype function
 */
struct xi_sametype_func : public ast_sametype_func {
public:

    xi_sametype_func(const ast_builder_impl_t& b) noexcept;

private:
#define __Decl_func(t) bool XI_SAMETYPE_FUNC_NAME(t)(TT_NAME(t)*, __Sametype_param_arg_types)
    __Decl_func(xi_auto);
    __Decl_func(xi_const);
    __Decl_func(xi_reference);
    __Decl_func(xi_tuple);
    __Decl_func(xi_decl);
#undef  __Decl_func

    xi_builder&                                             _xi_builder;
};


/**
 * The xi language widen function
 */
struct xi_widens_func : public ast_widens_func {
public:

    xi_widens_func(const ast_builder_impl_t& b) noexcept;

private:
#define __Decl_func(t) bool XI_WIDENS_FUNC_NAME(t)(TT_NAME(t)*, __Widens_param_arg_types)
    __Decl_func(xi_const);
    __Decl_func(xi_reference);
    __Decl_func(xi_tuple);
    __Decl_func(xi_decl);
    __Decl_func(ast_pointer);
#undef  __Decl_func

    xi_builder&                                             _xi_builder;
};


/**
 * The xi language cast function
 */
struct xi_cast_func : public ast_cast_func {
public:

    xi_cast_func(const ast_builder_impl_t& b) noexcept;

private:
#define __Decl_func(t) ast_expr* XI_CAST_FUNC_NAME(t)(TT_NAME(t)*, __Cast_param_arg_types)
    //__Decl_func(xi_auto);
    __Decl_func(xi_const);
    __Decl_func(xi_reference);
    __Decl_func(xi_tuple);
    __Decl_func(xi_decl);
    __Decl_func(ast_pointer);
#undef  __Decl_func

    xi_builder&                                             _xi_builder;
};

// Helper macros
#define XI_SAMETYPE_METHOD_NAME(t)              xi_sametype_func    :: XI_SAMETYPE_FUNC_NAME(t)
#define XI_WIDENS_METHOD_NAME(t)                xi_widens_func      :: XI_WIDENS_FUNC_NAME(t)
#define XI_CAST_METHOD_NAME(t)                  xi_cast_func        :: XI_CAST_FUNC_NAME(t)

#define XI_SAMETYPE_FUNC(t, tt, rh)             bool        XI_SAMETYPE_METHOD_NAME(t) (TT_NAME(t)* tt, __Sametype_param_args(rh))
#define XI_WIDENS_FUNC(t, tt, fe, c)            bool        XI_WIDENS_METHOD_NAME(t)   (TT_NAME(t)* tt, __Widens_param_args(fe, c))
#define XI_CAST_FUNC(t, tt, fe)                 ast_expr*   XI_CAST_METHOD_NAME(t)     (TT_NAME(t)* tt, __Cast_param_args(fe))

}



#endif /* XCC_XI_XI_TYPE_FUNC_HPP_ */
