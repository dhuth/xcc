/*
 * ast_widen_fwd.hpp
 *
 *  Created on: Feb 27, 2018
 *      Author: derick
 */

#ifndef XCC_AST_TYPE_CONV_FWD_HPP_
#define XCC_AST_TYPE_CONV_FWD_HPP_

#include "ast.hpp"

namespace xcc {

struct ast_widen_func;
struct ast_widens_func;
struct ast_coercable_func;
struct ast_coerce_func;
struct ast_max_func;
struct ast_cast_func;

#define __Widen_param_args(fe)                              ast_expr* fe
#define __Widens_param_args(fe, c)                          ast_expr* fe, int& c
#define __Coerce_param_args(...)                            __Widen_param_args(__VA_ARGS__)
#define __Coercable_param_args(...)                         __Widens_param_args(__VA_ARGS__)
#define __Max_param_args(rh)                                ast_type* rh
#define __Cast_param_args(e)                                ast_expr* e

#define __Widen_args(tt, fe)                                ast_type* tt, __Widen_param_args(fe)
#define __Widens_args(tt, fe, c)                            ast_type* tt, __Widens_param_args(fe, c)
#define __Coerce_args(tt, ...)                              ast_type* tt, __Coerce_param_args(__VA_ARGS__)
#define __Coercable_args(tt, ...)                           ast_type* tt, __Coercable_param_args(__VA_ARGS__)
#define __Max_args(lh, rh)                                  ast_type* lh, __Max_param_args(rh)
#define __Cast_args(tt, fe)                                 ast_type* tt, __Cast_param_args(fe)

#define __Widens_param_arg_types                            __Widens_param_args(,)
#define __Widen_param_arg_types                             __Widen_param_args()
#define __Coercable_param_arg_types                         __Coercable_param_args(,)
#define __Coerce_param_arg_types                            __Coerce_param_args()
#define __Max_param_arg_types                               __Max_param_args()
#define __Cast_param_arg_types                              __Cast_param_args()

#define __Widens_arg_types                                  __Widens_args(,,)
#define __Widen_arg_types                                   __Widen_args(,)
#define __Coercable_arg_types                               __Coercable_args(,,)
#define __Coerce_arg_types                                  __Coerce_args(,)
#define __Max_arg_types                                     __Max_args(,)
#define __Cast_arg_types                                    __Cast_args(,)

}



#endif /* XCC_AST_TYPE_CONV_FWD_HPP_ */
