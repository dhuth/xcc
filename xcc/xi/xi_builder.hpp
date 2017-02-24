/*
 * xi_builder.hpp
 *
 *  Created on: Feb 1, 2017
 *      Author: derick
 */

#ifndef XI_XI_BUILDER_HPP_
#define XI_XI_BUILDER_HPP_

#include <functional>
#include <unordered_map>
#include "frontend.hpp"
#include "xi_tree.hpp"
#include "ast_builder.hpp"

namespace xcc {

struct xi_lower_walker;
struct xi_resolve_walker;

struct xi_function_context : public ast_context {
public:

    xi_function_context(ast_context* parent, xi_function_decl* func);
    virtual ~xi_function_context() = default;

    void insert(const char*, ast_decl*) final override; //TODO: just throw error
    ast_type* get_return_type() final override;

protected:

    ptr<ast_decl> find_first_impl(const char*) final override;
    void find_all_impl(ptr<list<ast_decl>>, const char*) final override;

private:

    ptr<xi_function_decl>                                               _func;

};

struct xi_builder final : public ast_builder<> {
public:

    xi_builder(translation_unit&);
    virtual ~xi_builder() = default;

    xi_const_type*                                  get_const_type(ast_type*) const noexcept;
    xi_array_type*                                  get_array_type(ast_type*, list<ast_expr>*) const noexcept;
    ast_type*                                       get_declaration_type(ast_decl*) noexcept override final;

    xi_function_decl*                               define_global_function(ast_type*, const char*, list<xi_parameter_decl>*);
    xi_parameter_decl*                              define_parameter(ast_type*, const char*);
    xi_parameter_decl*                              define_parameter(ast_type*);

    ast_expr*                                       make_op(xi_operator op, ast_expr*);
    ast_expr*                                       make_op(xi_operator op, ast_expr*, ast_expr*);
    ast_expr*                                       make_op(xi_operator op, list<ast_expr>*);
    ast_expr*                                       make_cast_expr(ast_type*, ast_expr*) const override final;
    ast_expr*                                       make_index_expr(ast_expr*, list<ast_expr>*);
    ast_expr*                                       make_call_expr(ast_expr*, list<ast_expr>*) const override final;

    ast_stmt*                                       make_return_stmt(ast_type*, ast_expr*) const noexcept override final;
    ast_stmt*                                       make_assign_stmt(xi_operator, ast_expr*, ast_expr*) const noexcept;
    ast_stmt*                                       make_for_stmt(ast_local_decl*, ast_expr*, ast_stmt*) const noexcept;

    ast_expr*                                       widen(ast_type*, ast_expr*) const override final;

    void                                            push_function(xi_function_decl*);
    void                                            pop_function();

    void                                            generate();

public: //TODO: make private

    ast_expr*                                       resolve(ast_expr*); //TODO: in symbols / out symbols
    ast_type*                                       resolve(ast_type*); //TODO: in symbols / out symbols
    ast_decl*                                       resolve(ast_decl*); //TODO: in symbols / out symbols
    ast_stmt*                                       resolve(ast_stmt*); //TODO: in symbols / out symbols

    ast_expr*                                       lower(ast_expr*);
    ast_type*                                       lower(ast_type*);
    ast_decl*                                       lower(ast_decl*);
    ast_stmt*                                       lower(ast_stmt*);

    ast_parameter_decl*                             lower_parameter(xi_parameter_decl*);
    ast_function_decl*                              lower_function(xi_function_decl*);
    void                                            lower_body(xi_function_decl*);

public:

    void                                            resolution_pass();
    void                                            lower_pass();

private:

    std::vector<ptr<xi_function_decl>>                                  all_functions;

    friend struct xi_resolve_walker;
    friend struct xi_typecheck_walker;
    friend struct xi_lower_walker;

    xi_lower_walker*                                                    _lower_walker;
    //xi_resolve_walker&                                                  _resolve_walker;

};

}



#endif /* XI_XI_BUILDER_HPP_ */
