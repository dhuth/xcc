/*
 * xi_context.cpp
 *
 *  Created on: Nov 22, 2017
 *      Author: derick
 */

#include "xi_context.hpp"

namespace xcc {

ptr<ast_decl> xi_function_context::find_first_impl(const char* name) {
    // search parameters
    return first_from_list(this->func->parameters, name);
}

void xi_function_context::find_all_impl(ptr<list<ast_decl>> lout, const char* name) {
    // search parameters
    all_from_list(this->func->parameters, lout, name);
}


ptr<ast_decl> xi_method_context::find_first_impl(const char* name) {
    //TODO: search parameters...
}

void xi_method_context::find_all_impl(ptr<list<ast_decl>> lout, const char* name) {
    //TODO: search parameters
}


ptr<ast_decl> xi_using_context::find_first_impl(const char* name) {
    // search using declarations
    return first_from_list(this->usingdecl->using_declarations, name);
}

void xi_using_context::find_all_impl(ptr<list<ast_decl>> lout, const char* name) {
    // search using declarations
    all_from_list(this->usingdecl->using_declarations, lout, name);
}


ptr<ast_decl> xi_type_context::find_first_impl(const char* name) {
    // search all members
    return first_from_list(this->typedecl->members, name);
}

void xi_type_context::find_all_impl(ptr<list<ast_decl>> lout, const char* name) {
    // search all members
    all_from_list(this->typedecl->members, lout, name);
}


ptr<ast_decl> xi_struct_context::find_first_impl(const char* name) {
    // search all of my members
    auto from_members = first_from_list(this->structdecl->members, name);
    //TODO: ...
    return from_members;
}

void xi_struct_context::find_all_impl(ptr<list<ast_decl>> lout, const char* name) {
    // search all of my members
    all_from_list(this->structdecl->members, lout, name);
}
}


