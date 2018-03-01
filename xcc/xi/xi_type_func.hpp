/*
 * xi_widen.hpp
 *
 *  Created on: Feb 27, 2018
 *      Author: derick
 */

#ifndef XCC_XI_XI_TYPE_FUNC_HPP_
#define XCC_XI_XI_TYPE_FUNC_HPP_

#include "ast_type_func.hpp"
#include "xi_tree.hpp"

namespace xcc {


struct xi_sametype_func : public ast_sametype_func {
public:

    xi_sametype_func(const ast_builder_impl_t& b) noexcept;

private:
#define __Decl_func(t) bool SAMETYPE_FUNC_NAME(t)(TT_NAME(t)*, __Sametype_param_arg_types)
    __Decl_func(xi_auto);
    __Decl_func(xi_const);
    __Decl_func(xi_reference);
    __Decl_func(xi_tuple);
    __Decl_func(xi_decl);
#undef  __Decl_func
};


struct xi_widens_func : public ast_sametype_func {
public:

    xi_widens_func(const ast_builder_impl_t& b) noexcept;

private:
#define __Decl_func(t) bool WIDENS_FUNC_NAME(t)(TT_NAME(t)*, __Widens_param_arg_types)
    __Decl_func(xi_auto);
    __Decl_func(xi_const);
    __Decl_func(xi_reference);
    __Decl_func(xi_tuple);
    __Decl_func(xi_decl);
    __Decl_func(ast_pointer);
#undef  __Decl_func
};


struct xi_cast_func : public ast_sametype_func {
public:

    xi_cast_func(const ast_builder_impl_t& b) noexcept;

private:
#define __Decl_func(t) bool CAST_FUNC_NAME(t)(TT_NAME(t)*, __Cast_param_arg_types)
    __Decl_func(xi_auto);
    __Decl_func(xi_const);
    __Decl_func(xi_reference);
    __Decl_func(xi_tuple);
    __Decl_func(xi_decl);
    __Decl_func(ast_pointer);
#undef  __Decl_func
};


}



#endif /* XCC_XI_XI_TYPE_FUNC_HPP_ */
