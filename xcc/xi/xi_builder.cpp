/*
 * xi_builder.cpp
 *
 *  Created on: Feb 1, 2017
 *      Author: derick
 */

#include "xi_builder.hpp"

namespace xcc {

xi_builder::xi_builder(translation_unit& tu) : ast_builder<>(tu) { }


void xi_builder::define_typedef(const char* name, ast_type* type) {
}

xi_const_type* xi_builder::get_const_type(ast_type* type) const noexcept {
    return new xi_const_type(type);
}

ast_expr* xi_builder::make_op(xi_operator op, ast_expr* expr) {
    return new xi_op_expr(nullptr, op, new list<ast_expr>({expr}));
}

ast_expr* xi_builder::make_op(xi_operator op, ast_expr* lhs, ast_expr* rhs) {
    return new xi_op_expr(nullptr, op, new list<ast_expr>({lhs, rhs}));
}

ast_stmt* xi_builder::make_return_stmt(ast_type* rt, ast_expr* expr) const noexcept {
    return new ast_return_stmt(new ast_cast(rt, ast_op::none, expr));
}

ast_type* xi_builder::lower(ast_type* tp) {
    //TODO:...
    return tp;
}

ast_decl* xi_builder::lower(ast_decl* decl) {
    //TODO:...
    switch(decl->get_tree_type()) {
    case tree_type_id::xi_parameter_decl:       return this->lower_parameter(decl->as<xi_parameter_decl>());
    }
    return decl;
}

void xi_builder::resolvenames_pass() {
    //TODO:...
}

void xi_builder::typecheck_pass() {
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
    this->resolvenames_pass();
    this->typecheck_pass();
    this->lower_pass();
}

}


