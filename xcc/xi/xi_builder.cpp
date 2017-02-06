/*
 * xi_builder.cpp
 *
 *  Created on: Feb 1, 2017
 *      Author: derick
 */

#include "xi_builder.hpp"

namespace xcc {

xi_builder::xi_builder() : ast_builder<>(), _scope(new xi_global_scope()) { }


ast_type* xi_builder::get_named_type(const char* name) {
    return (*this->_scope).find_type(name);
}

ast_variable_decl* xi_builder::define_variable(ast_type* type, const char* name) {
    auto var = new ast_variable_decl(name, type, this->make_zero(type));
    this->_scope->add_decl(name, var);
    return var;
}

void xi_global_scope::add_type(const char* name, ast_type* tp) {
    this->_named_types[name] = box(tp);
}

ast_type* xi_global_scope::find_type(const char* name) {
    auto itr = this->_named_types.find(std::string(name));
    if(itr != this->_named_types.end()) {
        return unbox((*itr).second);
    }
    return nullptr;
}

void xi_global_scope::add_decl(const char* name, ast_variable_decl* decl) {
    this->_named_decls[name] = box(decl->as<ast_decl>());
}

}


