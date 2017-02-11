/*
 * xi_builder.cpp
 *
 *  Created on: Feb 1, 2017
 *      Author: derick
 */

#include "xi_builder.hpp"

namespace xcc {

xi_builder::xi_builder(translation_unit& tu) : ast_builder<>(), tu(tu), scope(new xi_global_scope()) { }


void xi_builder::define_typedef(const char* name, ast_type* type) {
    this->scope->add_type(name, type);
}

xi_const_type* xi_builder::get_const_type(ast_type* type) const noexcept {
    return new xi_const_type(type);
}

ast_variable_decl* xi_builder::define_global_variable(ast_type* type, const char* name, bool is_extern) {
    auto var = new ast_variable_decl(name, type, this->make_zero(type));
    var->is_extern = is_extern;
    var->is_extern_visible = true;
    this->scope->add_decl(name, var);
    this->tu.append(var);
    return var;
}

ast_variable_decl* xi_builder::define_global_variable(ast_type* type, const char* name, ast_expr* ivalue) {
    auto var = new ast_variable_decl(name, type, ivalue);
    //TODO: type inference/static conversion
    this->scope->add_decl(name, var);
    this->tu.append(var);
    return var;
}

ast_expr* xi_builder::make_op(xi_operator op, ast_expr* expr) {
    return new xi_op_expr(nullptr, op, new list<ast_expr>({expr}));
}

ast_expr* xi_builder::make_op(xi_operator op, ast_expr* lhs, ast_expr* rhs) {
    return new xi_op_expr(nullptr, op, new list<ast_expr>({lhs, rhs}));
}


void xi_global_scope::add_type(const char* name, ast_type* tp) {
    this->named_types[name] = box(tp);
}

ast_type* xi_global_scope::find_type(const char* name) {
    auto itr = this->named_types.find(std::string(name));
    if(itr != this->named_types.end()) {
        return unbox(itr->second);
    }
    return nullptr;
}

ast_decl* xi_global_scope::find_decl(const char* name) {
    auto itr = this->named_decls.find(std::string(name));
    if(itr != this->named_decls.end()) {
        return unbox(itr->second);
    }
    return nullptr;
}

void xi_global_scope::add_decl(const char* name, ast_variable_decl* decl) {
    this->named_decls[name] = box(decl->as<ast_decl>());
}

}


