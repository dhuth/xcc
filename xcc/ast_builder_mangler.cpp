/*
 * ast_builder_mangler.cpp
 *
 *  Created on: Feb 14, 2017
 *      Author: derick
 */
#include "ast_builder.hpp"

//itanium-cxx-abi.github.io/cxx-abi/abi.html

namespace xcc {

std::string ast_default_name_mangler::operator()(ast_tree* t) {
    if(t->is<ast_decl>()) {
        return "_Z" + this->visit(t);
    }
    return this->visit(t);
}
std::string ast_default_name_mangler::operator()(std::string prefix, ast_tree* t) {
    if(t->is<ast_decl>()) {
        if(prefix.empty()) {
            return "_Z" + this->visit(t);
        }
        else {
            return "_ZN" + prefix + this->visit(t);
        }
    }
    return prefix + this->visit(t);
}

std::string ast_default_name_mangler::mangle_variable(ast_variable_decl* decl) {
    return (std::string) decl->name;
}
std::string ast_default_name_mangler::mangle_parameter(ast_parameter_decl* decl) {
    return (std::string) decl->name;
}
std::string ast_default_name_mangler::mangle_function(ast_function_decl* decl) {
    std::stringstream s;
    s << (std::string) decl->name << "F";
    s << this->visit(decl->return_type);
    for(auto param : decl->parameters) {
        s << (std::string) this->visit(param->type);
    }
    s << "E";
    return s.str();
}




std::string ast_default_name_mangler::mangle_void_type(ast_void_type*) {
    return "v";
}
std::string ast_default_name_mangler::mangle_integer_type(ast_integer_type* tp) {
    if(tp->is_unsigned) {
        switch((uint32_t) tp->bitwidth) {
        case 1:  return "b";
        case 8:  return "c";
        case 16: return "t";
        case 32: return "j";
        case 64: return "m";
        }
        return "u" + std::to_string((uint32_t) tp->bitwidth);
    }
    else {
        switch((uint32_t) tp->bitwidth) {
        case 1:  return "b";
        case 8:  return "a";
        case 16: return "s";
        case 32: return "i";
        case 64: return "l";
        }
        return "i" + std::to_string((uint32_t) tp->bitwidth);
    }
}
std::string ast_default_name_mangler::mangle_real_type(ast_real_type* tp) {
    switch((uint32_t) tp->bitwidth) {
    case 16:  return "Dh";
    case 32:  return "f";
    case 64:  return "d";
    case 80:  return "e";
    case 128: return "g";
    }
    return "f" + std::to_string((uint32_t) tp->bitwidth);
}
std::string ast_default_name_mangler::mangle_array_type(ast_array_type* tp) {
    return  "A" + std::to_string((uint32_t) tp->size) + "_" + this->visit(tp->element_type);
}
std::string ast_default_name_mangler::mangle_pointer_type(ast_pointer_type* tp) {
    return "P" + this->visit(tp->element_type);
}
std::string ast_default_name_mangler::mangle_function_type(ast_function_type* tp) {
    std::stringstream s;
    s << "F" << this->visit(tp->return_type);
    for(auto param_tp : tp->parameter_types) {
        s << this->visit(param_tp);
    }
    s << "E";
    return s.str();
}
std::string ast_default_name_mangler::mangle_record_type(ast_record_type* rec) {
    std::stringstream s;
    s << "R";
    for(auto tp: rec->field_types) {
        s << this->visit(tp);
    }
    return s.str();
}

}


