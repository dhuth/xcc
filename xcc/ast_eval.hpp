/*
 * ast_eval.hpp
 *
 *  Created on: Jan 19, 2018
 *      Author: derick
 */

#ifndef XCC_AST_EVAL_HPP_
#define XCC_AST_EVAL_HPP_


#include "ast.hpp"
#include "ast_builder.hpp"

namespace xcc {

struct ast_compile_time_evaluator;

struct ast_eval_context final {
public:

    explicit ast_eval_context(ast_builder_impl_t& builder) noexcept
            : _ast_builder(builder),
              _prev(nullptr) {
        this->is_return         = false;
        this->is_continue       = false;
        this->is_break          = false;
        this->return_value      = box<ast_expr>(nullptr);
    }
    ~ast_eval_context() = default;

    ast_eval_context(const ast_eval_context&) = delete;
    ast_eval_context(const ast_eval_context&&) = delete;

    ptr<ast_expr> get(ast_decl*) noexcept;
    void set(ast_decl*, ptr<ast_expr>) noexcept;
    void bind(ast_decl*, ptr<ast_expr>) noexcept;

    ast_eval_context push() noexcept;

    bool                                                    is_return   = false;
    bool                                                    is_continue = false;
    bool                                                    is_break    = false;
    ptr<ast_expr>                                           return_value;

protected:
private:

    explicit ast_eval_context(ast_eval_context* prev, ast_builder_impl_t& builder) noexcept
            : _ast_builder(builder),
              _prev(prev) {
        this->is_return         = false;
        this->is_continue       = false;
        this->is_break          = false;
        this->return_value      = box<ast_expr>(nullptr);
    }

    ast_builder_impl_t&                                     _ast_builder;
    ast_eval_context*                                       _prev;
    std::map<ast_decl*, ptr<ast_expr>>                      _bindings;

};


struct ast_compile_time_evaluator : public dispatch_visitor<ptr<ast_expr>, ast_compile_time_evaluator&, ast_eval_context&> {
public:

    explicit ast_compile_time_evaluator() noexcept;
    virtual ~ast_compile_time_evaluator() = default;

    ast_compile_time_evaluator(const ast_compile_time_evaluator&) = delete;
    ast_compile_time_evaluator(const ast_compile_time_evaluator&&) = delete;

    ptr<ast_expr> call(ast_function_decl*, list<ast_expr>*, ast_eval_context&) noexcept;
    ptr<ast_expr> eval_stmt(ast_stmt*, ast_eval_context&) noexcept;
    ptr<ast_expr> eval_stmt(list<ast_stmt>*, ast_eval_context&) noexcept;

protected:
private:
};

}

#endif /* XCC_AST_EVAL_HPP_ */
