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

struct ast_eval_context {
public:

    explicit ast_eval_context(ast_builder_impl_t& builder) noexcept
            : _ast_builder(builder) {
        // do nothing
    }
    virtual ~ast_eval_context() = default;

    ast_eval_context(const ast_eval_context&) = delete;
    ast_eval_context(const ast_eval_context&&) = delete;

protected:
private:

    ast_builder_impl_t&                                     _ast_builder;
    ptr<ast_context>                                        _context;

};


struct ast_compile_time_evaluator : public dispatch_visitor<ptr<ast_expr>, ast_compile_time_evaluator&, ast_eval_context&> {
public:

    explicit ast_compile_time_evaluator() noexcept;
    virtual ~ast_compile_time_evaluator() = default;

    ast_compile_time_evaluator(const ast_compile_time_evaluator&) = delete;
    ast_compile_time_evaluator(const ast_compile_time_evaluator&&) = delete;

protected:
private:
};

}

#endif /* XCC_AST_EVAL_HPP_ */
