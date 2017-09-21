/*
 * xi_builder.hpp
 *
 *  Created on: Aug 20, 2017
 *      Author: derick
 */

#ifndef XCC_XI_XI_BUILDER_HPP_
#define XCC_XI_XI_BUILDER_HPP_


#include "xi_tree.hpp"
#include "ast_builder.hpp"

namespace xcc {

struct xi_builder : public ast_builder<> {
public:

    inline xi_builder(translation_unit& tu) noexcept : ast_builder<>(tu) {
        //...
    }

    xi_reference_type*                          get_reference_type(ast_type* type)                                                                            noexcept;

    xi_function_decl*                           define_global_function(std::string name, list<xi_parameter_decl>* parameters, ast_type* return_type)          noexcept;
    xi_parameter_decl*                          define_parameter(ast_type* type, std::string name)                                                            noexcept;
    xi_parameter_decl*                          define_parameter(ast_type* type)                                                                              noexcept;
    void                                        define_xi_local_inplace(std::string, ast_type*, ast_expr*, const source_span&)                                noexcept;
    void                                        define_xi_local_inplace(list<std::string>*, list<ast_type>*, list<ast_expr>*, const source_span&)             noexcept;

    ast_expr*                                   make_xi_op(xi_op_expr::xi_operator op, ast_expr*, const source_span&)                                   const noexcept;
    ast_expr*                                   make_xi_op(xi_op_expr::xi_operator op, ast_expr*, ast_expr*, const source_span&)                        const noexcept;
    ast_expr*                                   make_xi_op(xi_op_expr::xi_operator op, list<ast_expr>*, const source_span&)                             const noexcept;

    void                                        push_function(xi_function_decl*)                                                                              noexcept;
    void                                        push_function_and_body(xi_function_decl*)                                                                     noexcept;
    void                                        pop_function_and_body()                                                                                       noexcept;

private:


};



}


#endif /* XCC_XI_XI_BUILDER_HPP_ */
