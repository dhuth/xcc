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
    ast_type*                                   get_id_type(const char*)                                                                                const noexcept;

    xi_function_decl*                           make_function_decl(const char*, ast_type*, list<xi_parameter_decl>*, ast_stmt*)                         const noexcept;
    xi_parameter_decl*                          make_parameter_decl(const char*, ast_type*)                                                             const noexcept;

    ast_expr*                                   make_id_expr(const char*)                                                                               const noexcept;
    ast_expr*                                   make_member_id_expr(ast_expr*, const char*)                                                             const noexcept;
    ast_expr*                                   make_deref_member_id_expr(ast_expr*, const char*)                                                       const noexcept;
    ast_expr*                                   make_tuple_expr(list<ast_expr>*)                                                                        const noexcept;
    ast_expr*                                   make_xi_op(xi_op_expr::xi_operator op, ast_expr*)                                                       const noexcept;
    ast_expr*                                   make_xi_op(xi_op_expr::xi_operator op, ast_expr*, ast_expr*)                                            const noexcept;
    ast_expr*                                   make_xi_op(xi_op_expr::xi_operator op, list<ast_expr>*)                                                 const noexcept;

    ast_stmt*                                   make_return_stmt(ast_expr*)                                                                             const noexcept;

    void                                        push_function(xi_function_decl*)                                                                              noexcept;

    void                                        semantic_check();

private:

    ptr<xi_auto_type>                           _the_auto_type;
    ast_typeset                                 _const_types;
    ast_typeset                                 _reference_types;
    ast_typeset                                 _object_types;
    ast_typeset                                 _tuple_types;

};



}


#endif /* XCC_XI_XI_BUILDER_HPP_ */
