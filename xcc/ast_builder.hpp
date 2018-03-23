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
#include "ast_type_fwd.hpp"

#include <map>
#include <unordered_map>
#include <functional>

namespace xcc {

struct translation_unit;    // frontend.hpp
struct ircode_context;      // ircodegen.hpp
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
     * @param ts                the underlying typeset
     */
    __ast_builder_impl(
            translation_unit& tu,
            ast_name_mangler* mangler,
            ast_type_provider* tp) noexcept;
    virtual ~__ast_builder_impl() noexcept;

    /**
     * Get the universal void type node
     * @return
     */
    virtual ast_void_type*                      get_void_type()                                                     const noexcept;
    /**
     * Get an integer type node
     * @param bitwidth
     * @param is_unsigned
     * @return
     */
    virtual ast_integer_type*                   get_integer_type(uint32_t bitwidth, bool is_unsigned)               const noexcept;
    /**
     * Get a boolean type node
     * @return
     */
    virtual ast_integer_type*                   get_bool_type()                                                     const noexcept;
    /**
     * Get a floating point type node
     * @param bitwidth
     * @return
     */
    virtual ast_real_type*                      get_real_type(uint32_t bitwidth)                                    const noexcept;
    /**
     * Get a pointer type node
     * @param eltype
     * @return
     */
    virtual ast_pointer_type*                   get_pointer_type(ast_type* eltype)                                        noexcept;
    /**
     * Get an array type node
     * @param artype
     * @param size
     * @return
     */
    virtual ast_array_type*                     get_array_type(ast_type* artype, uint32_t size)                           noexcept;
    /**
     * Get a function type node
     * @param
     * @param
     * @return
     */
    virtual ast_function_type*                  get_function_type(ast_type*, ptr<list<ast_type>>, bool)                   noexcept;
    /**
     * Get a record type node
     * @param
     * @return
     */
    virtual ast_record_type*                    get_record_type(ptr<list<ast_type>>)                                      noexcept;
    /**
     * Get the language default string type (ast is an i8 array)
     * @return
     */
    virtual ast_type*                           get_string_type(const std::string& s)                                     noexcept;
    /**
     * Get the underlying size type
     * @return
     */
    virtual ast_type*                           get_size_type()                                                     const noexcept;

    /**
     * Get the type of a declaration
     * TODO: better explanation
     * @param
     * @return The type of a declaration
     */
    virtual ast_type*                           get_declaration_type(ast_decl*)                                           noexcept;


    /* ================= *
     * Make Declarations *
     * ================= */

    virtual ast_namespace_decl*                 make_namespace_decl(const char*, list<ast_decl>*)                   const noexcept;
    virtual ast_decl*                           make_local_decl(const char*, ast_type*, ast_expr*)                  const noexcept;
    virtual ast_temp_decl*                      make_temp_decl(ast_expr*)                                           const noexcept;


    /* =============================== *
     * Make Constant Value Expressions *
     * =============================== */

    virtual ast_expr*                           make_integer(const char* txt, uint8_t radix)                        const noexcept;
    virtual ast_expr*                           make_real(const char* txt)                                          const noexcept;
    virtual ast_expr*                           make_string(const char* txt, size_t len)                                  noexcept;
    virtual ast_expr*                           make_true()                                                         const noexcept;
    virtual ast_expr*                           make_false()                                                        const noexcept;
    virtual ast_expr*                           make_zero(ast_type* tp)                                             const noexcept;
    virtual ast_expr*                           make_null()                                                               noexcept;


    /* ================ *
     * Make Expressions *
     * ================ */

    virtual ast_expr*                           make_op_expr(ast_op, ast_expr*)                                           noexcept;
    virtual ast_expr*                           make_op_expr(ast_op, ast_expr*, ast_expr*)                                noexcept;
            ast_expr*                           make_lower_op_expr(ast_op, ast_expr*, ast_expr*)                          noexcept;
    virtual ast_expr*                           make_cast_expr(ast_type*, ast_expr*)                                const noexcept;
    virtual ast_declref*                        make_declref_expr(ast_decl*)                                              noexcept;
    virtual ast_expr*                           make_memberref_expr(ast_expr*, uint32_t)                                  noexcept;
    virtual ast_expr*                           make_deref_expr(ast_expr*)                                          const noexcept;
    virtual ast_expr*                           make_addressof_expr(ast_expr*)                                            noexcept;
            ast_expr*                           make_lower_addressof_expr(ast_expr*)                                      noexcept;
    virtual ast_expr*                           make_index_expr(ast_expr*, ast_expr*)                               const noexcept;
    virtual ast_expr*                           make_call_expr(ast_expr*, list<ast_expr>*)                          const noexcept;
            ast_expr*                           make_lower_call_expr(ast_expr*, list<ast_expr>*)                    const noexcept;
            ast_expr*                           make_assign_expr(ast_expr*, ast_expr*)                                    noexcept;


    /* =============== *
     * Make Statements *
     * =============== */

    virtual ast_stmt*                           make_nop_stmt()                                                     const noexcept;
    virtual ast_stmt*                           make_expr_stmt(ast_expr*)                                           const noexcept;
    virtual ast_stmt*                           make_assign_stmt(ast_expr* lhs, ast_expr* rhs)                      const noexcept;
            ast_stmt*                           make_lower_assign_stmt(ast_expr*, ast_expr*)                        const noexcept;
    virtual ast_stmt*                           make_block_stmt(list<ast_stmt>*)                                    const noexcept;
    virtual ast_stmt*                           make_block_stmt(ast_local_decl*, list<ast_stmt>*)                   const noexcept;
    virtual ast_stmt*                           make_block_stmt(list<ast_local_decl>*, list<ast_stmt>*)             const noexcept;
    virtual ast_stmt*                           make_if_stmt(ast_expr*,ast_stmt*,ast_stmt*)                         const noexcept;
    virtual ast_stmt*                           make_while_stmt(ast_expr*,ast_stmt*)                                const noexcept;
    virtual ast_stmt*                           make_for_stmt(ast_stmt*,ast_expr*,ast_stmt*,ast_stmt*)              const noexcept;
    virtual ast_stmt*                           make_return_stmt(ast_type*, ast_expr*)                              const noexcept;
    virtual ast_stmt*                           make_break_stmt()                                                   const noexcept;
    virtual ast_stmt*                           make_continue_stmt()                                                const noexcept;


    /* ================= *
     * Semantic Analysis *
     * ================= */

            bool                                sametype(__Sametype_arg_types)                                      const noexcept;
            bool                                widens(__Widens_arg_types)                                          const noexcept;
            bool                                widens(__Widens_arg_types_nc)                                       const noexcept;
            //bool                                coercable(__Coercable_arg_types)                                    const noexcept;
            //bool                                coercable(__Coercable_arg_types_nc)                                 const noexcept;
            ast_type*                           maxtype(__Maxtype_arg_types)                                        const noexcept;
            ast_expr*                           cast(__Cast_arg_types)                                              const noexcept;


    /* ================================= *
     * Low level stuff & code generation *
     * ================================= */

            ast_name_mangler&                   get_mangled_name;

protected:

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

    translation_unit&                                                   tu;                                         //! The translation unit
    ptr<ast_namespace_decl>                                             global_namespace;                           //! The unnamed global namespace

public:

    inline ptr<ast_namespace_decl> get_global_namespace() const {
        return this->global_namespace;
    }

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
     * Leave the current context
     */
    inline void pop_context() noexcept {
        this->context = this->context->pop_context();
    }

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
     * Insert a declaration at the global scope
     * @param
     */
            void                                insert_at_global_scope(ast_decl*)                                         noexcept;

private:

    //TODO: break up into create_default_integer / real / boolean / string ...
    virtual void                                create_default_types()                                                    noexcept;

    /* ============ *
     * Common types *
     * ============ */

    ptr<ast_void_type>                                                  _the_void_type;                             //! The void type
    ptr<ast_pointer_type>                                               _the_void_ptr_type;                         //! The void pointer type
    ptr<ast_integer_type>                                               _the_boolean_type;                          //! The default boolean type;
    ptr<ast_stmt>                                                       _the_nop_stmt;                              //! The nop stmt
    ptr<ast_stmt>                                                       _the_break_stmt;                            //! The break stmt;
    ptr<ast_stmt>                                                       _the_continue_stmt;                         //! The continue stmt

    std::map<uint32_t, ptr<ast_integer_type>>                           _unsigned_integer_types;                    //! Unsigned integer types by bitwidth
    std::map<uint32_t, ptr<ast_integer_type>>                           _signed_integer_types;                      //! Signed integer types by bitwidth
    std::map<uint32_t, ptr<ast_real_type>>                              _real_types;                                //! Floating point types by bitwidth

    /* ============= *
     * Common values *
     * ============= */

    //TODO: add null
    ptr<ast_expr>                                                       _true_value;                                //! The default true value
    ptr<ast_expr>                                                       _false_value;                               //! The default false value

    /* ========================== *
     * Type info function objects *
     * ========================== */

    ast_sametype_func*                                                  _the_sametype_func;
    ast_widens_func*                                                    _the_widens_func;
    //ast_coercable_func*                                                 _the_coercable_func;
    ast_maxtype_func*                                                   _the_maxtype_func;
    ast_cast_func*                                                      _the_cast_func;
    ast_typeset*                                                        _the_typeset;

    template<typename, typename>
    friend struct ast_builder;

    //TODO: rethink this
    ptr<ast_name_mangler>                                               _mangler_ptr;                              //! A pointer to the name mangling function
    ptr<ast_type_provider>                                              _type_provider_ptr;                        //! A pointer to type info provider

    int                                                                 _next_local_id;

};


/**
 * The base class for abstract syntax builders
 */
template<typename _Mangler              = ast_default_name_mangler,
         typename _TypeProvider         = ast_type_provider>
struct ast_builder : public __ast_builder_impl {
public:

    /**
     *
     * @param tu    the translation unit where top level nodes will be placed
     */
    ast_builder(translation_unit& tu) noexcept
            : __ast_builder_impl(
                    tu,
                    new _Mangler(),
                    new _TypeProvider()) {
        // do nothing
    }
    virtual ~ast_builder() noexcept {
        // do nothing
    }

protected:

    inline ast_typeset* get_typeset() const noexcept { return _the_typeset; }
    inline ast_type_provider* get_type_provider() const noexcept { return _type_provider_ptr; }

};


}



#endif /* AST_BUILDER_HPP_ */
