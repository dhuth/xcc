/*
 * xi_builder.cpp
 *
 *  Created on: Feb 1, 2017
 *      Author: derick
 */

#include "xi_builder.hpp"
#include "xi_lower.hpp"

namespace xcc {

xi_builder::xi_builder(translation_unit& tu)
    : ast_builder<>(tu),
      _lower_walker(new xi_lower_walker(*this)) {
    //...
}

xi_const_type* xi_builder::get_const_type(ast_type* type) const noexcept {
    return new xi_const_type(type);
}

xi_array_type* xi_builder::get_array_type(ast_type* eltype, list<ast_expr>* dims) const noexcept {
    return new xi_array_type(eltype, dims);
}

ast_type* xi_builder::get_declaration_type(ast_decl* decl) noexcept {
    switch(decl->get_tree_type()) {
    case tree_type_id::xi_parameter_decl:           return decl->as<xi_parameter_decl>()->type;
    case tree_type_id::xi_function_decl:
        {
            xi_function_decl*       fdecl = decl->as<xi_function_decl>();
            ast_type*               rtype = fdecl->return_type;
            std::vector<ast_type*>  pvec;
            for(auto p: fdecl->parameters) {
                pvec.push_back(p->type);
            }
            list<ast_type>*         plist = new list<ast_type>(pvec);
            return this->get_function_type(rtype, box(plist));
        }
    }

    return ast_builder<>::get_declaration_type(decl);
}

ast_expr* xi_builder::make_op(xi_operator op, ast_expr* expr) {
    return new xi_op_expr(nullptr, op, new list<ast_expr>({expr}));
}

ast_expr* xi_builder::make_op(xi_operator op, ast_expr* lhs, ast_expr* rhs) {
    return new xi_op_expr(nullptr, op, new list<ast_expr>({lhs, rhs}));
}

ast_expr* xi_builder::make_op(xi_operator op, list<ast_expr>* operands) {
    return new xi_op_expr(nullptr, op, operands);
}

ast_expr* xi_builder::make_cast_expr(ast_type* type, ast_expr* expr) const {
    return new ast_cast(type, ast_op::none, expr);
}

ast_expr* xi_builder::make_index_expr(ast_expr* arrexpr, list<ast_expr>* index_exprs) {
    return new xi_index_expr(nullptr, arrexpr, index_exprs);
}

ast_expr* xi_builder::make_call_expr(ast_expr* fexpr, list<ast_expr>* args) const {
    return new ast_invoke(nullptr, fexpr, args);
}

ast_stmt* xi_builder::make_return_stmt(ast_type* rt, ast_expr* expr) const noexcept {
    return new ast_return_stmt(new ast_cast(rt, ast_op::none, expr));
}

ast_stmt* xi_builder::make_assign_stmt(xi_operator op, ast_expr* lhs, ast_expr* rhs) const noexcept {
    return new xi_assign_stmt(op, lhs, rhs);
}

ast_stmt* xi_builder::make_for_stmt(ast_local_decl* decl, ast_expr* iterexpr, ast_stmt* body) const noexcept {
    return new xi_foriter_stmt(decl, iterexpr, body);
}

ast_expr* xi_builder::widen(ast_type* desttype, ast_expr* expr) const {
    return ast_builder<>::widen(desttype, expr);
}

ast_decl* xi_builder::lower(ast_decl* decl) {
    return this->_lower_walker->lower_decl(decl);
}

ast_stmt* xi_builder::lower(ast_stmt* stmt) {
    return this->_lower_walker->lower_stmt(stmt);
}

ast_expr* xi_builder::lower(ast_expr* expr) {
    return this->_lower_walker->lower_expr(expr);
}

ast_type* xi_builder::lower(ast_type* type) {
    return this->_lower_walker->lower_type(type);
}

void xi_builder::resolution_pass() {
    //TODO:...
}

void xi_builder::lower_pass() {
    for(auto f : this->all_functions) {
        f->generated_function = this->lower_function(f);
    }

    for(auto f: this->all_functions) {
        this->lower_body(f);
        this->tu.append(f->generated_function);
    }
}

void xi_builder::generate() {
    this->resolution_pass();
    this->lower_pass();
}

}


