/*
 * ast_builder.hpp
 *
 *  Created on: Jan 22, 2017
 *      Author: derick
 */

#ifndef AST_BUILDER_HPP_
#define AST_BUILDER_HPP_

#include <map>
#include <unordered_map>
#include <functional>

#include "ast.hpp"

namespace xcc {

typedef dispatch_visitor<std::string>                                   ast_name_mangler_t;
typedef dispatch_visitor<ast_expr>                                      ast_folder_t;

struct ast_default_name_mangler : public ast_name_mangler_t {
public:

    inline ast_default_name_mangler() noexcept {
        this->addmethod(&ast_default_name_mangler::mangle_variable);
        this->addmethod(&ast_default_name_mangler::mangle_function);
        this->addmethod(&ast_default_name_mangler::mangle_record);
        this->addmethod(&ast_default_name_mangler::mangle_record_member);

        this->addmethod(&ast_default_name_mangler::mangle_void_type);
        this->addmethod(&ast_default_name_mangler::mangle_integer_type);
        this->addmethod(&ast_default_name_mangler::mangle_real_type);
        this->addmethod(&ast_default_name_mangler::mangle_array_type);
        this->addmethod(&ast_default_name_mangler::mangle_pointer_type);
        this->addmethod(&ast_default_name_mangler::mangle_function_type);
        this->addmethod(&ast_default_name_mangler::mangle_record_type);
    }

    std::string                operator()(ast_tree* t);
    std::string                operator()(std::string prefix, ast_tree* t);

private:

    std::string                 mangle_void_type(ast_void_type*);
    std::string                 mangle_integer_type(ast_integer_type*);
    std::string                 mangle_real_type(ast_real_type*);
    std::string                 mangle_array_type(ast_array_type*);
    std::string                 mangle_pointer_type(ast_pointer_type*);
    std::string                 mangle_function_type(ast_function_type*);
    std::string                 mangle_record_type(ast_record_type*);

    std::string                 mangle_variable(ast_variable_decl*);
    std::string                 mangle_function(ast_function_decl*);
    std::string                 mangle_record(ast_record_decl*);
    std::string                 mangle_record_member(ast_record_member_decl*);

};

struct __ast_builder_impl {
public:

    __ast_builder_impl(ast_name_mangler_t mangler) noexcept;
    virtual ~__ast_builder_impl() noexcept = default;

            ast_void_type*                      get_void_type()                                                     const noexcept;
            ast_integer_type*                   get_integer_type(uint32_t bitwidth, bool is_unsigned)               const noexcept;
            ast_integer_type*                   get_bool_type()                                                     const noexcept;
            ast_real_type*                      get_real_type(uint32_t bitwidth)                                    const noexcept;
            ast_pointer_type*                   get_pointer_type(ast_type* eltype)                                        noexcept;
            ast_array_type*                     get_array_type(ast_type* artype, uint32_t size)                           noexcept;
            ast_function_type*                  get_function_type(ast_type*, ptr<list<ast_type>>)                         noexcept;
            ast_record_type*                    get_record_type(ast_record_decl*)                                         noexcept;

            ast_type*                           get_declaration_type(ast_decl*)                                           noexcept;


    virtual ast_expr*                           make_integer(const char* txt, uint8_t radix)                        const noexcept;
    virtual ast_expr*                           make_real(const char* txt)                                          const noexcept;

    virtual ast_expr*                           make_zero(ast_type* tp)                                             const noexcept;

    virtual ast_expr*                           make_neg_expr(ast_expr*)                                            const;
    virtual ast_expr*                           make_add_expr(ast_expr*, ast_expr*)                                 const;
    virtual ast_expr*                           make_sub_expr(ast_expr*, ast_expr*)                                 const;
    virtual ast_expr*                           make_mul_expr(ast_expr*, ast_expr*)                                 const;
    virtual ast_expr*                           make_div_expr(ast_expr*, ast_expr*)                                 const;
    virtual ast_expr*                           make_mod_expr(ast_expr*, ast_expr*)                                 const;

    virtual ast_expr*                           make_eq_expr(ast_expr*, ast_expr*)                                  const;
    virtual ast_expr*                           make_ne_expr(ast_expr*, ast_expr*)                                  const;
    virtual ast_expr*                           make_lt_expr(ast_expr*, ast_expr*)                                  const;
    virtual ast_expr*                           make_le_expr(ast_expr*, ast_expr*)                                  const;
    virtual ast_expr*                           make_gt_expr(ast_expr*, ast_expr*)                                  const;
    virtual ast_expr*                           make_ge_expr(ast_expr*, ast_expr*)                                  const;

    virtual ast_expr*                           make_land_expr(ast_expr*, ast_expr*)                                const;
    virtual ast_expr*                           make_lor_expr(ast_expr*, ast_expr*)                                 const;
    virtual ast_expr*                           make_lxor_expr(ast_expr*, ast_expr*)                                const;
    virtual ast_expr*                           make_lnot_expr(ast_expr*)                                           const;

    virtual ast_expr*                           make_band_expr(ast_expr*, ast_expr*)                                const;
    virtual ast_expr*                           make_bor_expr(ast_expr*, ast_expr*)                                 const;
    virtual ast_expr*                           make_bxor_expr(ast_expr*, ast_expr*)                                const;
    virtual ast_expr*                           make_bnot_expr(ast_expr*)                                           const;


    virtual ast_expr*                           make_cast_expr(ast_type*, ast_expr*)                                const;
    virtual ast_expr*                           make_declref_expr(ast_decl*);
    virtual ast_expr*                           make_memberref_expr(ast_expr*, ast_record_member_decl*);
    virtual ast_expr*                           make_deref_expr(ast_expr*)                                          const;
    virtual ast_expr*                           make_addressof_expr(ast_expr*);
    virtual ast_expr*                           make_index_expr(ast_expr*, ast_expr*)                               const;
    virtual ast_expr*                           make_call_expr(ast_expr*, list<ast_expr>*)                          const;

    virtual ast_stmt*                           make_nop_stmt()                                                     const noexcept;
    virtual ast_stmt*                           make_expr_stmt(ast_expr*)                                           const noexcept;

    virtual uint32_t                            foldu32(ast_expr* e)                                                const;

            ast_name_mangler_t                  get_mangled_name;
            //ast_folder_t                        fold;
    virtual bool                                sametype(ast_type*, ast_type*)                                      const;

//protected:

    virtual void                                create_default_types()                                                    noexcept;
            ast_type*                           maxtype(ast_type*, ast_type*)                                       const;
            ast_expr*                           widen(ast_type*, ast_expr*)                                         const;

private:

            ast_expr*                           cast_to(ast_integer_type*, ast_expr*)                               const;
            ast_expr*                           cast_to(ast_real_type*, ast_expr*)                                  const;
            ast_expr*                           cast_to(ast_pointer_type*, ast_expr*)                               const;

    struct sametype_predicate {
        __ast_builder_impl&                                             builder;

        inline sametype_predicate(__ast_builder_impl& builder): builder(builder) { }

        inline bool operator()(ast_type* const& lhs, ast_type* const& rhs) const {
            return builder.sametype(lhs, rhs);
        }
    };

    typedef std::tuple<ast_type*, ptr<list<ast_type>>>                 funckey_t;

    struct samefunctype_predicate {
        __ast_builder_impl&                                             builder;

        inline samefunctype_predicate(__ast_builder_impl& builder): builder(builder) { }

        inline bool operator()(funckey_t const& lhs, funckey_t const& rhs) const {
            auto lhs_rt = std::get<0>(lhs);
            auto rhs_rt = std::get<0>(rhs);
            auto lhs_params = std::get<1>(lhs);
            auto rhs_params = std::get<1>(rhs);

            auto lhs_func = new ast_function_type(std::get<0>(lhs), std::get<1>(lhs));
            auto rhs_func = new ast_function_type(std::get<0>(rhs), std::get<1>(rhs));

            return builder.sametype(lhs_func, rhs_func);
        }
    };

    struct functype_hasher {
        __ast_builder_impl&                                             builder;

        inline functype_hasher(__ast_builder_impl& builder): builder(builder) { }

        inline size_t operator()(const funckey_t& k) const {
            std::hash<ast_type*> hasher;
            auto h = hasher(std::get<0>(k));
            for(auto p: unbox(std::get<1>(k))) {
                h = (h >> 5) | hasher(p);
            }
            return h;
        }
    };

    ptr<ast_void_type>                                                  _the_void_type;
    ptr<ast_pointer_type>                                               _the_void_ptr_type;
    ptr<ast_integer_type>                                               _the_boolean_type;
    std::map<uint32_t, ptr<ast_integer_type>>                           _unsigned_integer_types;
    std::map<uint32_t, ptr<ast_integer_type>>                           _signed_integer_types;
    std::map<uint32_t, ptr<ast_real_type>>                              _real_types;
    std::unordered_map<ast_type*, ptr<ast_pointer_type>, std::hash<ast_type*>, sametype_predicate>
                                                                        _pointer_types;
    std::unordered_map<funckey_t, ptr<ast_function_type>, functype_hasher, samefunctype_predicate>
                                                                        _function_types;
    std::map<std::tuple<ast_type*, uint32_t>, ptr<ast_array_type>>      _array_types;
    std::map<ast_record_decl*, ptr<ast_record_type>>                    _record_types;

    ptr<ast_stmt>                                                       _the_nop_stmt;

};

template<typename TMangler = ast_default_name_mangler,
         typename std::enable_if<std::is_base_of<ast_name_mangler_t, TMangler>::value, int>::type = 0>
struct ast_builder : public __ast_builder_impl {
public:

    ast_builder() noexcept : __ast_builder_impl(TMangler()) { }
    virtual ~ast_builder() noexcept = default;

};


}



#endif /* AST_BUILDER_HPP_ */
