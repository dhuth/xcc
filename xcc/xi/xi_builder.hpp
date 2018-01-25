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

struct xi_builder;

struct xi_builder : public ast_builder<ast_default_name_mangler,
                                       xi_type_comparer,
                                       xi_type_hasher> {
public:

    inline xi_builder(translation_unit& tu) noexcept
            : ast_builder<ast_default_name_mangler, xi_type_comparer, xi_type_hasher>(tu),
              _the_auto_type(new xi_auto_type()),
              _const_types(*this->_type_hasher_ptr, *this->_type_comparer_ptr),
              _reference_types(*this->_type_hasher_ptr, *this->_type_comparer_ptr),
              _object_types(*this->_type_hasher_ptr, *this->_type_comparer_ptr),
              _tuple_types(*this->_type_hasher_ptr, *this->_type_comparer_ptr){
        // do nothing ...
    }

    ast_type*                                   get_const_type(ast_type* type)                                                                                noexcept;
    ast_type*                                   get_auto_type()                                                                                         const noexcept;
    ast_type*                                   get_reference_type(ast_type* type)                                                                            noexcept;
    ast_type*                                   get_object_type(ast_decl* decl)                                                                               noexcept;
    ast_type*                                   get_tuple_type(list<ast_type>* types)                                                                         noexcept;
    ast_type*                                   get_id_type(xi_qname*)                                                                                const noexcept;

    virtual ast_type*                           get_declaration_type(ast_decl*)                                                                               noexcept;

    xi_function_decl*                           make_xi_function_decl(const char*, ast_type*, list<xi_parameter_decl>*, ast_stmt*)                      const noexcept;
    xi_function_decl*                           make_xi_function_fwd_decl(const char*, ast_type*, list<xi_parameter_decl>*)                             const noexcept;
    xi_parameter_decl*                          make_xi_parameter_decl(const char*, ast_type*)                                                          const noexcept;
    xi_struct_decl*                             make_xi_struct_decl(const char*, list<xi_member_decl>*)                                                 const noexcept;
    xi_struct_decl*                             make_xi_struct_fwd_decl(const char*)                                                                    const noexcept;
    xi_field_decl*                              make_xi_field_decl(const char*, ast_type*)                                                              const noexcept;

    ast_expr*                                   make_xi_id_expr(xi_qname*)                                                                              const noexcept;
    ast_expr*                                   make_xi_member_id_expr(ast_expr*, xi_qname*)                                                            const noexcept;
    ast_expr*                                   make_xi_deref_member_id_expr(ast_expr*, xi_qname*)                                                      const noexcept;
    ast_expr*                                   make_xi_tuple_expr(list<ast_expr>*)                                                                     const noexcept;
    ast_expr*                                   make_xi_op(xi_op_expr::xi_operator op, ast_expr*)                                                       const noexcept;
    ast_expr*                                   make_xi_op(xi_op_expr::xi_operator op, ast_expr*, ast_expr*)                                            const noexcept;
    ast_expr*                                   make_xi_op(xi_op_expr::xi_operator op, list<ast_expr>*)                                                 const noexcept;
    ast_expr*                                   make_xi_member_expr(ast_expr*, xi_member_decl*)                                                         const noexcept;

    ast_stmt*                                   make_xi_if_stmt(ast_expr*, ast_stmt*, ast_stmt*)                                                        const noexcept;
    ast_stmt*                                   make_xi_while_stmt(ast_expr*, ast_stmt*)                                                                const noexcept;
    ast_stmt*                                   make_xi_return_stmt(ast_expr*)                                                                          const noexcept;

    ast_decl*                                   find_declaration(xi_qname*)                                                                             const noexcept;
    ptr<list<ast_decl>>                         find_all_declarations(xi_qname*)                                                                        const noexcept;

    void                                        push_xi_using(xi_using_decl*)                                                                                 noexcept;
    void                                        push_xi_function(xi_function_decl*)                                                                           noexcept;
    void                                        push_xi_struct(xi_struct_decl*)                                                                               noexcept;
    void                                        push_xi_method(xi_method_decl*)                                                                               noexcept;

    //ast_expr*                                   const_eval(ast_expr*)                                                                                   const noexcept;
    void                                        write_metadata(ircode_context&)                                                                               noexcept;
    void                                        semantic_check(/* error log info*/)                                                                           noexcept;
    void                                        lower(/* error log info*/)                                                                                    noexcept;

    ast_expr*                                   find_best_overload(xi_op_expr::xi_operator, list<ast_expr>*)                                            const noexcept;

private:

    ptr<xi_auto_type>                           _the_auto_type;
    ast_typeset                                 _const_types;
    ast_typeset                                 _reference_types;
    ast_typeset                                 _object_types;
    ast_typeset                                 _tuple_types;

};

void maybe_push_context(tree_t*, xi_builder& b);
void maybe_pop_context(tree_t*, xi_builder& b);

template<typename... TParameters>
struct xi_postorder_walker : public dispatch_postorder_tree_walker<tree_t, xi_builder&, TParameters...> {
public:

    void begin(tree_type_id, tree_t* t, xi_builder& b, TParameters...) override {
        maybe_push_context(t, b);
    }
    void end(tree_type_id, tree_t* t, xi_builder& b, TParameters...) override {
        maybe_pop_context(t, b);
    }
};

template<typename... TParameters>
struct xi_preorder_walker : public dispatch_postorder_tree_walker<tree_t, xi_builder&, TParameters...> {
public:

    void begin(tree_type_id, tree_t* t, xi_builder& b, TParameters...) override {
        maybe_push_context(t, b);
    }
    void end(tree_type_id, tree_t* t, xi_builder& b, TParameters...) override {
        maybe_pop_context(t, b);
    }
};


}


#endif /* XCC_XI_XI_BUILDER_HPP_ */
