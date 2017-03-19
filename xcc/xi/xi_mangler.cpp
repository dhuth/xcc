/*
 * xi_mangler.cpp
 *
 *  Created on: Mar 18, 2017
 *      Author: derick
 */


#include <iostream>
#include <sstream>
#include "xi_mangler.hpp"


namespace xcc {

static void prefix_cont(std::stringstream& s, ast_namespace_decl* d) {
    if(d != nullptr && ((std::string) d->name) != std::string("global")) {
        prefix_cont(s, d->parent_namespace);
        s << (std::string) d->name;
    }
}

static void prefix(std::stringstream& s, ast_namespace_decl* d) {
    if(d != nullptr && ((std::string) d->name != std::string("global"))) {
        s << "N";
        prefix_cont(s, d);
    }
}

std::string itanium_cxxabi_mangler::mangle_xi_function_decl(xi_function_decl* func) {
    std::stringstream s;

    prefix(s, func->parent_namespace);

    s << (std::string) func->name << "F" << this->visit(func->return_type);
    for(auto p: func->parameters) {
        s << this->visit(p->type);
    }
    s << "E";
    return s.str();
}

std::string itanium_cxxabi_mangler::mangle_xi_const_type(xi_const_type* t) {
    return "K" + this->visit(t->type);
}

std::string itanium_cxxabi_mangler::mangle_xi_ref_type(xi_ref_type* t) {
    return "R" + this->visit(t->element_type);
}

std::string itanium_cxxabi_mangler::mangle_xi_object_type(xi_object_type* t) {
    return t->declaration->name;
}


}
