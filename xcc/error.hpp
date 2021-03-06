/*
 * exceptions.hpp
 *
 *  Created on: Jan 19, 2018
 *      Author: derick
 */

#ifndef XCC_ERROR_HPP_
#define XCC_ERROR_HPP_

#include <stdexcept>

#define __STR(x)                                            #x
#define __WHERE(f, l)                                       f ", " __STR(l) ": "
#define __UNHANDLED_TREE_TYPE_MSG(f, l, t, m)               std::string(__WHERE(f, l) "Unhandled tree type ")       + t->get_tree_type_name() + " in " m
#define __UNHANDLED_AST_TYPE_MSG(f, l, t, m)                std::string(__WHERE(f, l) "Unhandled ast type ")        + t->get_tree_type_name() + " in " m
#define __UNHANDLED_OPERATOR_MSG(f, l, p, m)                std::string(__WHERE(f, l) "Unhandled operator ")        + std::to_string(p)       + " in " m
#define __UNSUPORTED_TREE_TYPE_MSG(f, l, t, m)              std::string(__WHERE(f, l) "Unsuported tree type ")      + t->get_tree_type_name() + " in " m
#define __UNREACHABLE_MSG(f, l, m)                          std::string(__WHERE(f, l) "Unreachable function call")  + " in " + m

#define __throw_unhandled_tree_type(f, l, t, m)             throw std::runtime_error(__UNHANDLED_TREE_TYPE_MSG(f, l, t, m))
#define __throw_unhandled_ast_type(f, l, t, m)              throw std::runtime_error(__UNHANDLED_AST_TYPE_MSG(f, l, t, m))
#define __throw_unhandled_operator(f, l, p, m)              throw std::runtime_error(__UNHANDLED_OPERATOR_MSG(f, l, p, m))
#define __throw_unsuported_tree_type(f, l, t, m)            throw std::runtime_error(__UNSUPORTED_TREE_TYPE_MSG(f, l, t, m))
#define __throw_unreachable(f, l, m)                        throw std::runtime_error(__UNREACHABLE_MSG(f, l, m))

//TODO: some sort of error logging mechanism


#endif /* XCC_ERROR_HPP_ */
