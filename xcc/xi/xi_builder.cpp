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

ast_type* xi_builder::lower(ast_type* tp) {
    //TODO...
    return tp;
}

ast_expr* xi_builder::lower(ast_expr* e) {
    //TODO:...
    return e;
}

ast_decl* xi_builder::lower(ast_decl* decl) {
    //TODO:...
    return decl;
}

ast_stmt* xi_builder::lower(ast_stmt* stmt) {
    //TODO:...
    return stmt;
}

void xi_builder::generate() {
    for(auto f : this->all_functions) {
        f->generated_function = this->lower_function(f);
    }

    for(auto f: this->all_functions) {
        this->lower_body(f);
        this->tu.append(f->generated_function);
    }
}

}


