/*
 * ast_context.cpp
 *
 *  Created on: Mar 6, 2017
 *      Author: derick
 */


#include "ast_context.hpp"

namespace xcc {

ptr<ast_decl> ast_context::find(const char* name, bool search_parent) {
    auto ff = this->find_first_impl(name);
    if(unbox(ff) != nullptr) {
        return ff;
    }
    else if(search_parent && (this->parent != nullptr)) {
        return this->parent->find(name, true);
    }
    return box<ast_decl>(nullptr);
}

ptr<list<ast_decl>> ast_context::findall(const char* name, bool search_parent) {
    ptr<list<ast_decl>> olist = box(new list<ast_decl>());
    this->findall(olist, name, search_parent);
    return olist;
}

void ast_context::findall(ptr<list<ast_decl>> olist, const char* name, bool search_parent) {
    this->find_all_impl(olist, name);
    if(search_parent && (this->parent != nullptr)) {
        this->parent->findall(olist, name, search_parent);
    }
}

ptr<ast_context> ast_context::pop_context() noexcept {
    return this->parent;
}


ptr<ast_decl> ast_namespace_context::find_first_impl(const char* name) {
    return first_from_list(this->_ns->declarations, name);
}

void ast_namespace_context::find_all_impl(ptr<list<ast_decl>> olist, const char* name) {
    all_from_list(this->_ns->declarations, olist, name);
}


ptr<ast_decl> ast_block_context::find_first_impl(const char* name) {
    return first_from_list(this->_block->decls, name);
}

void ast_block_context::find_all_impl(ptr<list<ast_decl>> olist, const char* name) {
    all_from_list(this->_block->decls, olist, name);
}


ptr<ast_decl> ast_function_context::find_first_impl(const char* name) {
    return first_from_list(this->_func->parameters, name);
}

void ast_function_context::find_all_impl(ptr<list<ast_decl>> olist, const char* name) {
    all_from_list(this->_func->parameters, olist, name);
}
}

