/*
 * xi_widen.hpp
 *
 *  Created on: Feb 27, 2018
 *      Author: derick
 */

#ifndef XCC_XI_XI_WIDEN_HPP_
#define XCC_XI_XI_WIDEN_HPP_

#include "xi_builder.hpp"
#include "xi_walker.hpp"

namespace xcc {

struct xi_widen_func : public xi_const_visitor<ast_expr*, ast_expr*> {
public:

    explicit xi_widen_func() noexcept;

private:

    ast_expr*   to_ast_integer_type(ast_integer_type*,      ast_expr*, const xi_builder&);
    ast_expr*   to_ast_real_type(ast_real_type*,            ast_expr*, const xi_builder&);
    ast_expr*   to_xi_const_type(xi_const_type*,            ast_expr*, const xi_builder&);
    ast_expr*   to_ast_pointer_type(ast_pointer_type*,      ast_expr*, const xi_builder&);
    ast_expr*   to_xi_reference_type(xi_reference_type*,    ast_expr*, const xi_builder&);
    ast_expr*   to_xi_decl_type(xi_decl_type*,              ast_expr*, const xi_builder&);

};

struct xi_widens_func : public xi_const_visitor<bool, ast_type*, int&> {
public:

    explicit xi_widens_func() noexcept;

private:

    bool        to_ast_integer_type(ast_integer_type*,      ast_type*, int&, const xi_builder&);
    bool        to_ast_real_type(ast_real_type*,            ast_type*, int&, const xi_builder&);
    bool        to_xi_const_type(xi_const_type*,            ast_type*, int&, const xi_builder&);
    bool        to_ast_pointer_type(ast_pointer_type*,      ast_type*, int&, const xi_builder&);
    bool        to_xi_reference_type(xi_reference_type*,    ast_type*, int&, const xi_builder&);
    bool        to_xi_decl_type(xi_decl_type*,              ast_type*, int&, const xi_builder&);
};


}



#endif /* XCC_XI_XI_WIDEN_HPP_ */
