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

typedef dispatch_visitor<ast_expr>                                      ast_folder_t;

struct ast_name_mangler_t : public dispatch_visitor<std::string> {
    virtual std::string         operator()(ast_tree* t) = 0;
    virtual std::string         operator()(std::string prefix, ast_tree* t) = 0;
};

struct ast_default_name_mangler : public ast_name_mangler_t {
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

template<typename TDestTreeType,
         typename TSrcTreeType,
         typename std::enable_if<std::is_base_of<ast_tree, TDestTreeType>::value, int>::type = 0,
         typename std::enable_if<std::is_base_of<ast_tree, TSrcTreeType>::value, int>::type = 0>
TDestTreeType* copyloc(TDestTreeType* t, TSrcTreeType* ft) noexcept { t->source_location = ft->source_location; return t; }

template<typename TDestTreeType,
         typename TSrcMinTreeType,
         typename TSrcMaxTreeType,
         typename std::enable_if<std::is_base_of<ast_tree, TDestTreeType>::value, int>::type = 0,
         typename std::enable_if<std::is_base_of<ast_tree, TSrcMinTreeType>::value, int>::type = 0,
         typename std::enable_if<std::is_base_of<ast_tree, TSrcMaxTreeType>::value, int>::type = 0>
TDestTreeType* copyloc(TDestTreeType *t, TSrcMinTreeType* fmin, TSrcMaxTreeType* fmax) noexcept {
    t->source_location->first   = fmin->source_location->first;
    t->source_location->last    = fmax->source_location->last;
    return t;
}

struct __ast_builder_impl {
public:

    __ast_builder_impl(translation_unit& tu, ast_name_mangler_t* mangler) noexcept;
    virtual ~__ast_builder_impl() noexcept = default;

    template<typename TTreeType, typename std::enable_if<std::is_base_of<ast_tree, TTreeType>::value, int>::type = 0>
    TTreeType* setloc(TTreeType* t, const source_span& loc) { t->source_location = loc; return t; }

    template<typename TTreeType, typename std::enable_if<std::is_base_of<ast_tree, TTreeType>::value, int>::type = 0>
    TTreeType* setloc(TTreeType* t, source_span& minloc, const source_span& maxloc) {
        t->source_location->first = minloc.first;
        t->source_location->last  = maxloc.last;
        return t;
    }
            void                                copyloc(ast_tree* dest, ast_tree* src) const noexcept;
            void                                copyloc(ast_tree* dest, ast_tree* minsrc, ast_tree* maxsrc) const noexcept;

            ast_void_type*                      get_void_type()                                                     const noexcept;
            ast_integer_type*                   get_integer_type(uint32_t bitwidth, bool is_unsigned)               const noexcept;
            ast_integer_type*                   get_bool_type()                                                     const noexcept;
            ast_real_type*                      get_real_type(uint32_t bitwidth)                                    const noexcept;
            ast_pointer_type*                   get_pointer_type(ast_type* eltype)                                        noexcept;
            ast_array_type*                     get_array_type(ast_type* artype, uint32_t size)                           noexcept;
            ast_function_type*                  get_function_type(ast_type*, ptr<list<ast_type>>)                         noexcept;
            ast_record_type*                    get_record_type(ptr<list<ast_type>>)                                      noexcept;
    virtual ast_type*                           get_string_type(uint32_t length)                                          noexcept;
    virtual ast_type*                           get_char_type()                                                           noexcept;

    virtual ast_type*                           get_declaration_type(ast_decl*)                                           noexcept;

    // Declarations
    virtual ast_typedef_decl*                   define_named_type(const char*, ast_type*)                                 noexcept;
    virtual ast_namespace_decl*                 define_namespace(const char*)                                             noexcept;
    virtual ast_variable_decl*                  define_global_variable(ast_type*, const char*)                            noexcept;
    virtual ast_variable_decl*                  define_global_variable(ast_type*, const char*, ast_expr*)                 noexcept;
    virtual ast_local_decl*                     define_local_variable(ast_type*, const char*)                             noexcept;
    virtual ast_local_decl*                     define_local_variable(ast_type*)                                          noexcept;

    // Constant values
    virtual ast_expr*                           make_integer(const char* txt, uint8_t radix)                        const noexcept;
    virtual ast_expr*                           make_real(const char* txt)                                          const noexcept;
    virtual ast_expr*                           make_string(const char* txt, uint32_t from, uint32_t length)              noexcept;

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
    virtual ast_stmt*                           make_block_stmt()                                                   const noexcept;
    virtual ast_stmt*                           make_if_stmt(ast_expr*,ast_stmt*,ast_stmt*)                         const noexcept;
    virtual ast_stmt*                           make_while_stmt(ast_expr*,ast_stmt*)                                const noexcept;
    virtual ast_stmt*                           make_for_stmt(ast_stmt*,ast_expr*,ast_stmt*,ast_stmt*)              const noexcept;
    virtual ast_stmt*                           make_return_stmt(ast_type*, ast_expr*)                              const noexcept;
    virtual ast_stmt*                           make_break_stmt()                                                   const noexcept;
    virtual ast_stmt*                           make_continue_stmt()                                                const noexcept;
            void                                emit(ast_stmt*)                                                           noexcept;

    // Utility
    virtual uint32_t                            foldu32(ast_expr* e);
    virtual uint64_t                            foldu64(ast_expr* e);
    virtual ast_expr*                           fold(ast_expr* e);

    // Anylasis
            ast_name_mangler_t&                 get_mangled_name;
    virtual bool                                sametype(ast_type*, ast_type*)                                      const;
    virtual ast_type*                           maxtype(ast_type*, ast_type*)                                       const;
    virtual bool                                widens(ast_type*, ast_type*)                                        const;
    virtual ast_expr*                           widen(ast_type*, ast_expr*)                                         const;

    virtual bool                                is_ptrof(ast_type*, ast_type*);
    virtual bool                                is_arrayof(ast_type*, ast_type*);

protected:

    virtual void                                create_default_types()                                                    noexcept;


    // Building context
    ptr<ast_context>                                                    context;
    template<typename TContext, typename... TArgs>
    inline void push_context(TArgs... args) noexcept {
        this->context = new TContext(this->context, args...);
    }

    inline ptr<ast_context> pop_context() noexcept {
        auto popped_context = this->context;
        this->context = this->context->parent;
        return popped_context;
    }

    translation_unit&                                                   tu;
    ptr<ast_namespace_decl>                                             global_namespace;

public:

            ptr<ast_context>                    get_context()                                                             noexcept;
            void                                set_context(ptr<ast_context>)                                             noexcept;
            void                                clear_context()                                                           noexcept;

    virtual void                                push_block(ast_block_stmt*)                                               noexcept;
    virtual void                                push_namespace(ast_namespace_decl*)                                       noexcept;
    virtual ast_type*                           get_return_type()                                                         noexcept;

    virtual ast_decl*                           find_declaration(const char*)                                             noexcept;
    virtual ptr<list<ast_decl>>                 find_all_declarations(const char*)                                        noexcept;

    virtual void                                pop()                                                                     noexcept;

private:

            ast_expr*                           cast_to(ast_integer_type*, ast_expr*)                               const;
            ast_expr*                           cast_to(ast_real_type*, ast_expr*)                                  const;
            ast_expr*                           cast_to(ast_pointer_type*, ast_expr*)                               const;
            ast_expr*                           cast_to(ast_record_type*, ast_expr*)                                const;

    struct sametype_predicate {
        __ast_builder_impl&                                             builder;

        inline sametype_predicate(__ast_builder_impl& builder): builder(builder) { }

        inline bool operator()(ast_type* const& lhs, ast_type* const& rhs) const {
            return builder.sametype(lhs, rhs);
        }
    };

    struct sametypelist_predicate {
        __ast_builder_impl&                                             builder;

        inline sametypelist_predicate(__ast_builder_impl& builder): builder(builder) { }
        inline bool operator()(ptr<list<ast_type>> const& lhs, ptr<list<ast_type>> const& rhs) const {
            if(lhs->size() != rhs->size()) return false;
            for(uint32_t i = 0; i < lhs->size(); i++) {
                if(!builder.sametype((*unbox(lhs))[i], (*unbox(rhs))[i])) {
                    return false;
                }
            }
            return true;
        }
    };

    typedef ptr<list<ast_type>>                                         typelistkey_t;
    typedef std::tuple<ast_type*, ptr<list<ast_type>>>                  functypekey_t;

    struct samefunctype_predicate {
        __ast_builder_impl&                                             builder;

        inline samefunctype_predicate(__ast_builder_impl& builder): builder(builder) { }

        inline bool operator()(functypekey_t const& lhs, functypekey_t const& rhs) const {
            auto lhs_rt = std::get<0>(lhs);
            auto rhs_rt = std::get<0>(rhs);
            auto lhs_params = std::get<1>(lhs);
            auto rhs_params = std::get<1>(rhs);

            auto lhs_func = new ast_function_type(std::get<0>(lhs), std::get<1>(lhs));
            auto rhs_func = new ast_function_type(std::get<0>(rhs), std::get<1>(rhs));

            return builder.sametype(lhs_func, rhs_func);
        }
    };

    struct typelist_hasher {
        __ast_builder_impl&                                             builder;

        inline typelist_hasher(__ast_builder_impl& builder): builder(builder) { }

        inline size_t operator()(const typelistkey_t& k) const {
            std::hash<ast_type*> hasher;
            if(k->size() == 0) return 0;
            auto h = hasher(nullptr);
            for(auto t: unbox(k)) {
                h = (h << 5) | hasher(t);
            }
            return h;
        }
    };

    struct functype_hasher {
        __ast_builder_impl&                                             builder;

        inline functype_hasher(__ast_builder_impl& builder): builder(builder) { }

        inline size_t operator()(const functypekey_t& k) const {
            std::hash<ast_type*> hasher;
            auto h = hasher(std::get<0>(k));
            for(auto p: unbox(std::get<1>(k))) {
                h = (h << 5) | hasher(p);
            }
            return h;
        }
    };

    ptr<ast_void_type>                                                  _the_void_type;
    ptr<ast_pointer_type>                                               _the_void_ptr_type;
    ptr<ast_integer_type>                                               _the_boolean_type;
    ptr<ast_stmt>                                                       _the_nop_stmt;
    ptr<ast_stmt>                                                       _the_break_stmt;
    ptr<ast_stmt>                                                       _the_continue_stmt;

    ptr<ast_expr>                                                       _true_value;
    ptr<ast_expr>                                                       _false_value;

    std::map<uint32_t, ptr<ast_integer_type>>                           _unsigned_integer_types;
    std::map<uint32_t, ptr<ast_integer_type>>                           _signed_integer_types;
    std::map<uint32_t, ptr<ast_real_type>>                              _real_types;
    std::unordered_map<ast_type*, ptr<ast_pointer_type>, std::hash<ast_type*>, sametype_predicate>
                                                                        _pointer_types;
    std::unordered_map<functypekey_t, ptr<ast_function_type>, functype_hasher, samefunctype_predicate>
                                                                        _function_types;
    std::map<std::tuple<ast_type*, uint32_t>, ptr<ast_array_type>>      _array_types;
    std::unordered_map<typelistkey_t, ptr<ast_record_type>, typelist_hasher, sametypelist_predicate>
                                                                        _record_types;

    ast_name_mangler_t*                                                 _mangler_ptr;

};

template<typename TMangler = ast_default_name_mangler,
         typename std::enable_if<std::is_base_of<ast_name_mangler_t, TMangler>::value, int>::type = 0>
struct ast_builder : public __ast_builder_impl {
public:

    ast_builder(translation_unit& tu) noexcept : __ast_builder_impl(tu, new TMangler()) { }
    virtual ~ast_builder() noexcept = default;

};


}



#endif /* AST_BUILDER_HPP_ */
