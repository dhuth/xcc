/*
 * xi_builder.hpp
 *
 *  Created on: Aug 20, 2017
 *      Author: derick
 */

#ifndef XCC_XI_XI_BUILDER_HPP_
#define XCC_XI_XI_BUILDER_HPP_


#include "ast_builder.hpp"
#include "ast_type.hpp"
#include "xi_tree.hpp"
#include "xi_struct.hpp"
#include "xi_name_mangler.hpp"
#include <stack>

namespace xcc {

struct xi_type_provider;


struct xi_builder : public ast_builder<xi_name_mangler,
                                       xi_type_provider> {
public:

    explicit xi_builder(translation_unit&) noexcept;
    ~xi_builder() noexcept;

    xi_const_type*                              get_const_type(ast_type* type)                                                                                noexcept;
    xi_auto_type*                               get_auto_type()                                                                                         const noexcept;
    xi_reference_type*                          get_reference_type(ast_type* type)                                                                            noexcept;
    xi_decl_type*                               get_object_type(ast_decl* decl)                                                                               noexcept;
    xi_tuple_type*                              get_tuple_type(list<ast_type>* types)                                                                         noexcept;
    xi_id_type*                                 get_id_type(xi_qname*)                                                                                  const noexcept;

    ast_type*                                   get_declaration_type(ast_decl*)                                                                               noexcept override;


    /* --------------------------------- *
     * Parser Make Declaration Functions *
     * --------------------------------- */

    ast_namespace_decl*                         make_namespace_decl(const char*, list<ast_decl>*)                                                       const noexcept override;
    xi_function_decl*                           make_xi_function_decl(const char*, ast_type*, list<xi_parameter_decl>*, ast_stmt*, bool)                const noexcept;
    xi_operator_function_decl*                  make_xi_operator_function_decl(xi_operator, ast_type*, list<xi_parameter_decl>*, ast_stmt*, bool)       const noexcept;
    xi_method_decl*                             make_xi_method_decl(const char*, ast_type*, list<xi_parameter_decl>*, ast_stmt*, bool)                  const noexcept;
    xi_operator_method_decl*                    make_xi_operator_method_decl(xi_operator, ast_type*, list<xi_parameter_decl>*, ast_stmt*, bool)         const noexcept;
    xi_parameter_decl*                          make_xi_parameter_decl(const char*, ast_type*)                                                          const noexcept;
    xi_struct_decl*                             make_xi_struct_decl(const char*, list<xi_member_decl>*)                                                 const noexcept;
    xi_field_decl*                              make_xi_field_decl(const char*, ast_type*)                                                              const noexcept;


    /* -------------------------------- *
     * Parser Make Expression Functions *
     * -------------------------------- */

    ast_expr*                                   make_xi_id_expr(xi_qname*)                                                                              const noexcept;
    ast_expr*                                   make_xi_member_id_expr(ast_expr*, xi_qname*)                                                            const noexcept;
    ast_expr*                                   make_xi_deref_member_id_expr(ast_expr*, xi_qname*)                                                      const noexcept;
    ast_expr*                                   make_xi_tuple_expr(list<ast_expr>*)                                                                     const noexcept;
    ast_expr*                                   make_xi_op(xi_op_expr::xi_operator op, ast_expr*)                                                       const noexcept;
    ast_expr*                                   make_xi_op(xi_op_expr::xi_operator op, ast_expr*, ast_expr*)                                            const noexcept;
    ast_expr*                                   make_xi_op(xi_op_expr::xi_operator op, list<ast_expr>*)                                                 const noexcept;
    ast_expr*                                   make_xi_invoke(ast_expr*, list<ast_expr>*)                                                              const noexcept;
    ast_expr*                                   make_xi_index(ast_expr*, list<ast_expr>*)                                                               const noexcept;


    /* ------------------------------- *
     * Parser Make Statement Functions *
     * ------------------------------- */

    ast_stmt*                                   make_xi_if_stmt(ast_expr*, ast_stmt*, ast_stmt*)                                                        const noexcept;
    ast_stmt*                                   make_xi_while_stmt(ast_expr*, ast_stmt*)                                                                const noexcept;
    ast_stmt*                                   make_xi_return_stmt(ast_expr*)                                                                          const noexcept;


    /* --------------------------------------- *
     * Semantic Pass Make Expression Functions *
     * --------------------------------------- */

    ast_expr*                                   make_xi_member_expr(ast_expr*, xi_member_decl*)                                                               noexcept;
    ast_expr*                                   make_xi_deref_expr(ast_expr*)                                                                           const noexcept;
    ast_expr*                                   make_xi_call_expr(ast_decl*, list<ast_expr>* args)                                                            noexcept;


    /* ----------------- *
     * Semantic Analysis *
     * ----------------- */

    // Defined in xi_walker.cpp
    ast_type*                                   get_return_type()                                                                                       const noexcept;
    ast_namespace_decl*                         get_nesting_namespace()                                                                                 const noexcept;
    ast_type*                                   get_nesting_type()                                                                                      const noexcept;

    void                                        push_xi_namespace(xi_namespace_decl*)                                                                         noexcept;
    void                                        push_xi_function(xi_function_decl*)                                                                           noexcept;
    void                                        push_xi_struct(xi_struct_decl*)                                                                               noexcept;
    void                                        push_xi_method(xi_method_decl*)                                                                               noexcept;
    void                                        push_xi_block(ast_block_stmt*)                                                                                noexcept;

    void                                        leave_xi_namespace()                                                                                          noexcept;
    void                                        leave_xi_function()                                                                                           noexcept;
    void                                        leave_xi_struct()                                                                                             noexcept;
    void                                        leave_xi_method()                                                                                             noexcept;
    void                                        leave_xi_block()                                                                                              noexcept;

    // Defined in xi_lookup.cpp
    ast_decl*                                   find_declaration(ref<xi_qname>)                                                                         const noexcept;
    ptr<list<ast_decl>>                         find_all_declarations(ref<xi_qname>)                                                                    const noexcept;
    xi_member_decl*                             find_member(ast_type*, ref<xi_qname>)                                                                   const noexcept;
    ptr<list<xi_member_decl>>                   find_all_members(ast_type*, ref<xi_qname>)                                                              const noexcept;


    // Defined in xi_decl.cpp
    bool                                        samedecl(ast_decl*, ast_decl*)                                                                          const noexcept;


    /* ---------------------- *
     * Implementation details *
     * ---------------------- */

    // Defined in xi_struct.cpp
    void                                        implement_type(ast_type*)                                                                                     noexcept;
    void                                        implement_decl(ast_decl*)                                                                                     noexcept;


    /* --------------------- *
     * Major compiler passes *
     * --------------------- */

    bool                                        read_metadata_pass(ircode_context& /* input modules, error log info */)                                       noexcept;
    bool                                        dom_pass(/* error log info*/)                                                                                 noexcept;
    bool                                        write_metadata_pass(ircode_context& /* error log info */)                                                     noexcept;
    bool                                        semantic_pass(/* error log info*/)                                                                            noexcept;
    bool                                        mangle_names_pass()                                                                                           noexcept;
    bool                                        lower_pass(ircode_context&/* error log info*/)                                                                noexcept;


    /* --------- *
     * Utilities *
     * --------- */

    template<typename T>
    inline treemap<ast_type, T> create_typemap() const noexcept {
        return this->get_type_provider()->create_typemap<T>();
    }

    template<typename Tdecl, typename T>
    inline treemap<Tdecl, T> create_declmap() const noexcept {
        return treemap<Tdecl, T>([=](Tdecl* r, Tdecl* l)->bool {
            return this->samedecl(r, l);
        });
    }

private:

    ptr<xi_auto_type>                                       _the_auto_type;

    std::stack<ptr<ast_namespace_decl>>                     _nesting_namespaces;
    std::stack<ptr<ast_type>>                               _nesting_types;
    std::stack<ptr<ast_decl>>                               _nesting_closure_decls;

    treemap<xi_struct_decl, xi_structimpl_info>             _decl_implementations;

};


}


#endif /* XCC_XI_XI_BUILDER_HPP_ */
