/*
 * ast_builder_mangler.cpp
 *
 *  Created on: Feb 14, 2017
 *      Author: derick
 */
#include "ast_builder.hpp"


namespace xcc {

std::string ast_default_name_mangler::operator()(ast_tree* t)                     { return this->visit(t); }
std::string ast_default_name_mangler::operator()(std::string prefix, ast_tree* t) { return prefix + this->visit(t); }

std::string ast_default_name_mangler::mangle_variable(ast_variable_decl* decl) {
    return (std::string) decl->name;
}
std::string ast_default_name_mangler::mangle_parameter(ast_parameter_decl* decl) {
    return (std::string) decl->name;
}
std::string ast_default_name_mangler::mangle_function(ast_function_decl* decl) {
    std::stringstream s;
    s << (std::string) decl->name << "F$";
    for(auto param : decl->parameters) {
        s << (std::string) this->visit(param->type) << ".";
    }
    s << "$";
    return s.str();
}
std::string ast_default_name_mangler::mangle_record(ast_record_decl* decl) {
    return decl->name;
}
std::string ast_default_name_mangler::mangle_record_member(ast_record_member_decl* decl) {
    return (std::string) decl->parent->as<ast_record_decl>()->name + "." + (std::string) decl->name;
}
std::string ast_default_name_mangler::mangle_void_type(ast_void_type*) {
    return "void";
}
std::string ast_default_name_mangler::mangle_integer_type(ast_integer_type* tp) {
    if(tp->is_unsigned) {
        return "u" + std::to_string((uint32_t) tp->bitwidth);
    }
    else {
        return "i" + std::to_string((uint32_t) tp->bitwidth);
    }
}
std::string ast_default_name_mangler::mangle_real_type(ast_real_type* tp) {
    return "f" + std::to_string((uint32_t) tp->bitwidth);
}
std::string ast_default_name_mangler::mangle_array_type(ast_array_type* tp) {
    return this->visit(tp->element_type) + "a$" + std::to_string((uint32_t) tp->size) + ".$";
}
std::string ast_default_name_mangler::mangle_pointer_type(ast_pointer_type* tp) {
    return this->visit(tp->element_type) + "p";
}
std::string ast_default_name_mangler::mangle_function_type(ast_function_type* tp) {
    std::stringstream s;
    s << this->visit(tp->return_type) << "f$";
    for(auto param_tp : tp->parameter_types) {
        s << this->visit(param_tp) << ".";
    }
    s << "$";
    return s.str();
}
std::string ast_default_name_mangler::mangle_record_type(ast_record_type* tp) {
    return (std::string) tp->declaration->name + "r$";
}

}


