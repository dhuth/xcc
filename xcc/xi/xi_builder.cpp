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

}


