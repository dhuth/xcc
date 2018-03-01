/*
 * ast_widen_fwd.hpp
 *
 *  Created on: Feb 27, 2018
 *      Author: derick
 */

#ifndef XCC_AST_TYPE_FWD_HPP_
#define XCC_AST_TYPE_FWD_HPP_

#include "ast.hpp"

namespace xcc {

// defined in ast_type.hpp
struct ast_type_provider;

// defined in ast_type_func.hpp
struct ast_widens_func;
struct ast_coercable_func;
struct ast_maxtype_func;
struct ast_sametype_func;
struct ast_cast_func;

// defined in ast_type_set.hpp
struct ast_typeset;
struct ast_typehash_func;

#define __Sametype_param_args(rh)                           ast_type* rh
#define __Widens_param_args(fe, c)                          ast_expr* fe, int& c
#define __Widens_param_args_nc(fe)                          ast_expr* fe
#define __Coercable_param_args(...)                         __Widens_param_args(__VA_ARGS__)
#define __Coercable_param_args_nc(...)                      __Widens_param_args_nc(__VA_ARGS__)
#define __Maxtype_param_args(rh)                            ast_type* rh
#define __Cast_param_args(e)                                ast_expr* e

#define __Sametype_args(lh, rh)                             ast_type* lh, __Sametype_param_args(rh)
#define __Widens_args(tt, fe, c)                            ast_type* tt, __Widens_param_args(fe, c)
#define __Widens_args_nc(tt, fe)                            ast_type* tt, __Widens_param_args_nc(fe)
#define __Coercable_args(tt, ...)                           ast_type* tt, __Coercable_param_args(__VA_ARGS__)
#define __Coercable_args_nc(tt, ...)                        ast_type* tt, __Coercable_param_args_nc(__VA_ARGS__)
#define __Maxtype_args(lh, rh)                              ast_type* lh, __Maxtype_param_args(rh)
#define __Cast_args(tt, fe)                                 ast_type* tt, __Cast_param_args(fe)

#define __Sametype_param_arg_types                          __Sametype_param_args()
#define __Widens_param_arg_types                            __Widens_param_args(,)
#define __Coercable_param_arg_types                         __Coercable_param_args(,)
#define __Maxtype_param_arg_types                           __Maxtype_param_args()
#define __Cast_param_arg_types                              __Cast_param_args()

#define __Sametype_arg_types                                __Sametype_args(,)
#define __Widens_arg_types                                  __Widens_args(,,)
#define __Widens_arg_types_nc                               __Widens_args_nc(,)
#define __Coercable_arg_types                               __Coercable_args(,,)
#define __Coercable_arg_types_nc                            __Coercable_args_nc(,)
#define __Maxtype_arg_types                                 __Maxtype_args(,)
#define __Cast_arg_types                                    __Cast_args(,)

}



#endif /* XCC_AST_TYPE_FWD_HPP_ */
