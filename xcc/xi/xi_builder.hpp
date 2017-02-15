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
    void                                            define_typedef(const char*, ast_type*);

    xi_function_decl*                               define_global_function(ast_type*, const char*, list<xi_parameter_decl>*);
    xi_parameter_decl*                              define_parameter(ast_type*, const char*);
    xi_parameter_decl*                              define_parameter(ast_type*);

    ast_expr*                                       make_op(xi_operator op, ast_expr*);
    ast_expr*                                       make_op(xi_operator op, ast_expr*, ast_expr*);

    void                                            push_function(xi_function_decl*);
    void                                            pop_function();

    void                                            generate();

protected:

    ast_expr*                                       lower(ast_expr*);
    ast_type*                                       lower(ast_type*);
    ast_decl*                                       lower(ast_decl*);
    ast_stmt*                                       lower(ast_stmt*);
    ast_function_decl*                              lower_function(xi_function_decl*);
    void                                            lower_body(xi_function_decl*);

private:

    std::vector<ptr<xi_function_decl>>                                  all_functions;

};

}



#endif /* XI_XI_BUILDER_HPP_ */
