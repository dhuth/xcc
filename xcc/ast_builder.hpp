/*
 * ast_builder.hpp
 *
 *  Created on: Jan 22, 2017
 *      Author: derick
 */

#ifndef AST_BUILDER_HPP_
#define AST_BUILDER_HPP_

#include "ast.hpp"
#include "ast_context.hpp"

#include <map>
#include <unordered_map>
#include <functional>

namespace xcc {

struct translation_unit;
struct ircode_context;
struct __ast_builder_impl;

/// A convenient way to refer to ast_builder implementation without template parameters
typedef __ast_builder_impl                                  ast_builder_impl_t;

struct ast_name_mangler : public dispatch_visitor<std::string> {
    virtual std::string         operator()(ast_tree* t) = 0;
    virtual std::string         operator()(std::string prefix, ast_tree* t) = 0;
};


/**
 * The default name mangling function for ast_trees.
 * Follows c++ conventions
 */
struct ast_default_name_mangler : public ast_name_mangler {
public:

    inline ast_default_name_mangler() noexcept {
        this->addmethod(&ast_default_name_mangler::mangle_variable);
        this->addmethod(&ast_default_name_mangler::mangle_parameter);
        this->addmethod(&ast_default_name_mangler::mangle_function);

        this->addmethod(&ast_default_name_mangler::mangle_void_type);
        this->addmethod(&ast_default_name_mangler::mangle_integer_type);
        this->addmethod(&ast_default_name_mangler::mangle_real_type);
        this->addmethod(&ast_default_name_mangler::mangle_array_type);
        this->addmethod(&ast_default_name_mangler::mangle_pointer_type);
        this->addmethod(&ast_default_name_mangler::mangle_function_type);
        this->addmethod(&ast_default_name_mangler::mangle_record_type);
    }
    virtual ~ast_default_name_mangler() = default;

    virtual std::string         operator()(ast_tree* t);
    virtual std::string         operator()(std::string prefix, ast_tree* t);

private:

    std::string                 mangle_void_type(ast_void_type*);
    std::string                 mangle_integer_type(ast_integer_type*);
    std::string                 mangle_real_type(ast_real_type*);
    std::string                 mangle_array_type(ast_array_type*);
    std::string                 mangle_pointer_type(ast_pointer_type*);
    std::string                 mangle_function_type(ast_function_type*);
    std::string                 mangle_record_type(ast_record_type*);

    std::string                 mangle_variable(ast_variable_decl*);
    std::string                 mangle_parameter(ast_parameter_decl*);
    std::string                 mangle_function(ast_function_decl*);

};


/**
 * The base implementation for an ast_builder
 */
struct __ast_builder_impl {
public:

    /**
     * __ast_builder_impl constructor
     * @param tu                the translation unit where top level nodes will be placed
     * @param mangler           the name mangler function
     * @param type_comparer     the type comparer used in typesets
     * @param type_hasher       the type hasher used in typesets
     */
    __ast_builder_impl(
            translation_unit& tu,
            ast_name_mangler* mangler,
            ast_type_comparer* type_comparer,
            ast_type_hasher* type_hasher) noexcept;
    virtual ~__ast_builder_impl() noexcept;


            ast_void_type*                      get_void_type()                                                     const noexcept;
            ast_integer_type*                   get_integer_type(uint32_t bitwidth, bool is_unsigned)               const noexcept;
            ast_integer_type*                   get_bool_type()                                                     const noexcept;
            ast_real_type*                      get_real_type(uint32_t bitwidth)                                    const noexcept;
            ast_pointer_type*                   get_pointer_type(ast_type* eltype)                                        noexcept;
            ast_array_type*                     get_array_type(ast_type* artype, uint32_t size)                           noexcept;
            ast_function_type*                  get_function_type(ast_type*, ptr<list<ast_type>>)                         noexcept;
            ast_record_type*                    get_record_type(ptr<list<ast_type>>)                                      noexcept;

    /**
     * Get the type of a declaration
     * TODO: better explanation
     * @param
     * @return The type of a declaration
     */
    virtual ast_type*                           get_declaration_type(ast_decl*)                                           noexcept;

    // Declarations
    virtual ast_decl*                           make_namespace_decl(const char*, list<ast_decl>*)                   const noexcept;
    virtual ast_decl*                           make_local_decl(const char*, ast_type*, ast_expr*)                  const noexcept;

    // Constant values
    virtual ast_expr*                           make_integer(const char* txt, uint8_t radix)                        const noexcept;
    virtual ast_expr*                           make_real(const char* txt)                                          const noexcept;

    virtual ast_expr*                           make_true()                                                         const noexcept;
    virtual ast_expr*                           make_false()                                                        const noexcept;
    virtual ast_expr*                           make_zero(ast_type* tp)                                             const noexcept;

    // Expressions
    virtual ast_expr*                           make_op_expr(ast_op, ast_expr*);
    virtual ast_expr*                           make_op_expr(ast_op, ast_expr*, ast_expr*);
            ast_expr*                           make_lower_op_expr(ast_op, ast_expr*, ast_expr*);
    virtual ast_expr*                           make_cast_expr(ast_type*, ast_expr*)                                const;
            ast_expr*                           make_lower_cast_expr(ast_type*, ast_expr*)                          const;
    virtual ast_expr*                           make_declref_expr(ast_decl*);
    virtual ast_expr*                           make_memberref_expr(ast_expr*, uint32_t);
    virtual ast_expr*                           make_deref_expr(ast_expr*)                                          const;
    virtual ast_expr*                           make_addressof_expr(ast_expr*);
            ast_expr*                           make_lower_addressof_expr(ast_expr*);
    virtual ast_expr*                           make_index_expr(ast_expr*, ast_expr*)                               const;
    virtual ast_expr*                           make_call_expr(ast_expr*, list<ast_expr>*)                          const;
            ast_expr*                           make_lower_call_expr(ast_expr*, list<ast_expr>*)                    const;
    virtual ast_expr*                           make_stmt_expr(list<ast_stmt>* stmts, ast_expr*)                    const noexcept;

    // Statments
    virtual ast_stmt*                           make_nop_stmt()                                                     const noexcept;
    virtual ast_stmt*                           make_expr_stmt(ast_expr*)                                           const noexcept;
    virtual ast_stmt*                           make_assign_stmt(ast_expr* lhs, ast_expr* rhs)                            noexcept;
            ast_stmt*                           make_lower_assign_stmt(ast_expr*, ast_expr*)                              noexcept;
    virtual ast_stmt*                           make_block_stmt(list<ast_stmt>*)                                    const noexcept;
    virtual ast_stmt*                           make_block_stmt(ast_local_decl*, list<ast_stmt>*)                   const noexcept;
    virtual ast_stmt*                           make_block_stmt(list<ast_local_decl>*, list<ast_stmt>*)             const noexcept;
    virtual ast_stmt*                           make_if_stmt(ast_expr*,ast_stmt*,ast_stmt*)                         const noexcept;
    virtual ast_stmt*                           make_while_stmt(ast_expr*,ast_stmt*)                                const noexcept;
    virtual ast_stmt*                           make_for_stmt(ast_stmt*,ast_expr*,ast_stmt*,ast_stmt*)              const noexcept;
    virtual ast_stmt*                           make_return_stmt(ast_type*, ast_expr*)                              const noexcept;
    virtual ast_stmt*                           make_break_stmt()                                                   const noexcept;
    virtual ast_stmt*                           make_continue_stmt()                                                const noexcept;

    // Anylasis
            ast_name_mangler&                   get_mangled_name;
    virtual bool                                sametype(ast_type*, ast_type*)                                      const;
    virtual ast_type*                           maxtype(ast_type*, ast_type*)                                       const;
    virtual bool                                widens(ast_type*, ast_type*)                                        const;
    virtual ast_expr*                           widen(ast_type*, ast_expr*)                                         const;

protected:

    virtual void                                create_default_types()                                                    noexcept;


    // Building context
    ptr<ast_context>                                                    context;                                    //! Current context

    /**
     * Push a new context as a child to the current one
     * @param args          Arguments to the new context's constructor
     */
    template<typename TContext, typename... TArgs>
    inline void push_context(TArgs&&... args) noexcept {
        this->context = this->context->push_context<TContext>(std::forward<TArgs>(args)...);
    }

    /**
     * Leave the current context and return to its' parent
     * @return              the original context
     */
    inline ptr<ast_context> pop_context() noexcept {
        auto popped_context = this->context;
        this->context = this->context->parent;
        return popped_context;
    }

    translation_unit&                                                   tu;                                         //! The translation unit
    ptr<ast_namespace_decl>                                             global_namespace;                           //! The unnamed global namespace

    ast_type_comparer*                                                  _type_comparer_ptr;                         //! A pointer to the type comparer function
    ast_type_hasher*                                                    _type_hasher_ptr;                           //! A pointer to the type hasher function

public:

    /**
     * Step into a namespace scope
     * @param
     */
    virtual void                                push_namespace(ast_namespace_decl*)                                       noexcept;

    /**
     * Step into a function scope
     * @param
     */
    virtual void                                push_function(ast_function_decl*)                                         noexcept;

    /**
     * Step into a block scope
     * @param
     */
    virtual void                                push_block(ast_block_stmt*)                                               noexcept;

    /**
     * Get the first declaration with the given name
     * @param
     * @return
     */
            ast_decl*                           find_declaration(const char*)                                             noexcept;
    /**
     * Get the first declaration with the given name in a particular context
     * @param
     * @param
     * @return
     */
    virtual ast_decl*                           find_declaration(ast_context*, const char*)                         const noexcept;
    /**
     * Find all declarations with the given name, starting with the closest
     * @param
     * @return
     */
            ptr<list<ast_decl>>                 find_all_declarations(const char*)                                        noexcept;
    /**
     * Find all declarations with the given name in a particular context, starting with the closest
     * @param
     * @param
     * @return
     */
    virtual ptr<list<ast_decl>>                 find_all_declarations(ast_context*, const char*)                    const noexcept;

    /**
     * Step out of the current context and back into its parent
     */
    virtual void                                pop()                                                                     noexcept;

    /**
     * Insert a declaration at the global scope
     * @param
     */
            void                                insert_at_global_scope(ast_decl*)                                         noexcept;

private:

            ast_expr*                           cast_to(ast_integer_type*, ast_expr*)                               const;
            ast_expr*                           cast_to(ast_real_type*,    ast_expr*)                               const;
            ast_expr*                           cast_to(ast_pointer_type*, ast_expr*)                               const;
            ast_expr*                           cast_to(ast_record_type*,  ast_expr*)                               const;

    ptr<ast_void_type>                                                  _the_void_type;                             //! The void type
    ptr<ast_pointer_type>                                               _the_void_ptr_type;                         //! The void pointer type
    ptr<ast_integer_type>                                               _the_boolean_type;                          //! The default boolean type;
    ptr<ast_stmt>                                                       _the_nop_stmt;                              //! The nop stmt
    ptr<ast_stmt>                                                       _the_break_stmt;                            //! The break stmt;
    ptr<ast_stmt>                                                       _the_continue_stmt;                         //! The continue stmt

    ptr<ast_expr>                                                       _true_value;                                //! The default true value
    ptr<ast_expr>                                                       _false_value;                               //! The default false value

    std::map<uint32_t, ptr<ast_integer_type>>                           _unsigned_integer_types;                    //! Unsigned integer types by bitwidth
    std::map<uint32_t, ptr<ast_integer_type>>                           _signed_integer_types;                      //! Signed integer types by bitwidth
    std::map<uint32_t, ptr<ast_real_type>>                              _real_types;                                //! Floating point types by bitwidth

    ast_typeset                                                         _pointer_types;                             //! The pointer type set
    ast_typeset                                                         _function_types;                            //! The function type set
    ast_typeset                                                         _array_types;                               //! The array type set
    ast_typeset                                                         _record_types;                              //! The record type set

    ast_name_mangler*                                                   _mangler_ptr;                               //! A pointer to the name mangling function
};


/**
 * The base class for abstract syntax builders
 */
template<typename TMangler              = ast_default_name_mangler,
         typename TTypeComparer         = ast_type_comparer,
         typename TTypeHasher           = ast_type_hasher,
         typename std::enable_if<std::is_base_of<ast_name_mangler, TMangler>::value, int>::type = 0>
struct ast_builder : public __ast_builder_impl {
public:

    /**
     *
     * @param tu    the translation unit where top level nodes will be placed
     */
    ast_builder(translation_unit& tu) noexcept
            : __ast_builder_impl(
                    tu,
                    new TMangler(),
                    new TTypeComparer(),
                    new TTypeHasher()) { }
    virtual ~ast_builder() noexcept = default;

};


}



#endif /* AST_BUILDER_HPP_ */
