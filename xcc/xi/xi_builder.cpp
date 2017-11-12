/*
 * xi_builder.cpp
 *
 *  Created on: Aug 20, 2017
 *      Author: derick
 */

#include "xi_builder.hpp"

namespace xcc {

ast_type* xi_builder::get_auto_type() const noexcept {
    return this->_the_auto_type;
}

ast_type* xi_builder::get_const_type(ast_type* type) noexcept {
    auto newtype = box(new xi_const_type(type));
    if(this->_const_types.find(newtype) == this->_const_types.end()) {
        this->_const_types[newtype] = newtype;
        return newtype;
    }
    else {
        return this->_const_types[newtype];
    }
}

ast_type* xi_builder::get_reference_type(ast_type* type) noexcept {
    auto newtype = box(new xi_reference_type(type));
    if(this->_reference_types.find(newtype) == this->_reference_types.end()) {
        this->_reference_types[newtype] = newtype;
        return newtype;
    }
    else {
        return this->_reference_types[newtype];
    }
}

ast_type* xi_builder::get_object_type(xi_decl* decl) noexcept {
    auto newtype = box(new xi_object_type(decl));
    if(this->_object_types.find(newtype) == this->_object_types.end()) {
        this->_object_types[newtype] = newtype;
        return newtype;
    }
    else {
        return this->_object_types[newtype];
    }
}

ast_type* xi_builder::get_tuple_type(list<ast_type>* types) noexcept {
    auto newtype = box(new xi_tuple_type(types));
    if(this->_tuple_types[newtype] == this->_tuple_types.end()) {
        this->_tuple_types[newtype] = newtype;
        return newtype;
    }
    else {
        return this->_tuple_types[newtype];
    }
}


}
