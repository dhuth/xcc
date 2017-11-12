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

struct xi_builder : public ast_builder<ast_default_name_mangler,
                                       xi_type_comparer,
                                       xi_type_hasher> {
public:

    inline xi_builder(translation_unit& tu) noexcept
            : ast_builder<ast_default_name_mangler, xi_type_comparer, xi_type_hasher>(tu),
              _the_auto_type(new xi_auto_type()),
              _const_types(0, *this->_type_hasher_ptr, *this->_type_comparer_ptr),
              _reference_types(0, *this->_type_hasher_ptr, *this->_type_comparer_ptr),
              _object_types(0, *this->_type_hasher_ptr, *this->_type_comparer_ptr),
              _tuple_types(0, *this->_type_hasher_ptr, *this->_type_comparer_ptr){
        // do nothing ...
    }

    ast_type*                                   get_const_type(ast_type* type)                                                                                noexcept;
    ast_type*                                   get_auto_type()                                                                                         const noexcept;
    ast_type*                                   get_reference_type(ast_type* type)                                                                            noexcept;
    ast_type*                                   get_object_type(xi_decl* decl)                                                                                noexcept;
    ast_type*                                   get_tuple_type(list<ast_type>* types)                                                                         noexcept;

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

    ptr<xi_auto_type>                           _the_auto_type;
    ast_typeset                                 _const_types;
    ast_typeset                                 _reference_types;
    ast_typeset                                 _object_types;
    ast_typeset                                 _tuple_types;

};



}


#endif /* XCC_XI_XI_BUILDER_HPP_ */
