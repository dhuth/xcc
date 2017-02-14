/*
 * xi_builder.cpp
 *
 *  Created on: Feb 1, 2017
 *      Author: derick
 */

#include "xi_builder.hpp"

namespace xcc {

xi_builder::xi_builder(translation_unit& tu) : ast_builder<>(), tu(tu), _context(new xi_namespace_context()) { }


void xi_builder::define_typedef(const char* name, ast_type* type) {
}

xi_const_type* xi_builder::get_const_type(ast_type* type) const noexcept {
    return new xi_const_type(type);
}

ast_variable_decl* xi_builder::define_global_variable(ast_type* type, const char* name) {
    auto var = new ast_variable_decl(name, type, this->make_zero(type));
    var->is_extern = false;
    var->is_extern_visible = true;

    this->_context->insert(name, var);
    this->tu.append(var);
    return var;
}

ast_variable_decl* xi_builder::define_global_variable(ast_type* type, const char* name, ast_expr* ivalue) {
    auto var = new ast_variable_decl(name, type, ivalue);

    this->_context->insert(name, var);
    this->tu.append(var);
    return var;
}

ast_expr* xi_builder::make_op(xi_operator op, ast_expr* expr) {
    return new xi_op_expr(nullptr, op, new list<ast_expr>({expr}));
}

ast_expr* xi_builder::make_op(xi_operator op, ast_expr* lhs, ast_expr* rhs) {
    return new xi_op_expr(nullptr, op, new list<ast_expr>({lhs, rhs}));
}

ast_type* xi_builder::flatten(ast_type* tp) {
    switch(tp->get_tree_type()) {
    case tree_type_id::xi_const_type:
        return this->flatten(tp->as<xi_const_type>()->type);
    }
    return tp;
}

ast_expr* xi_builder::flatten(ast_expr* e) {
    //...
}

void xi_builder::generate() {
    for(auto f : unbox(this->_all_functions)) {
        f->generated_function = this->flatten_function(f);
    }

    for(auto f: unbox(this->_all_functions)) {
        this->flatten_body(f);
        this->tu.append(f->generated_function);
    }
}

void xi_builder::pop() {
    this->_context = this->_context->_parent;
}

}


