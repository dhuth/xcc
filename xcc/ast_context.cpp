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


ast_namespace_context::ast_namespace_context(ast_context* p, ast_namespace_decl* ns)
        : ast_context(p), _ns(ns), _is_global(p == nullptr) {
    // do nothing
}

ptr<ast_decl> ast_namespace_context::find_first_impl(const char* name) {
    for(auto d: this->_ns->declarations) {
        if(((std::string) d->name) == std::string(name)) {
            return box(d);
        }
    }
    return box<ast_decl>(nullptr);
}

void ast_namespace_context::find_all_impl(ptr<list<ast_decl>> olist, const char* name) {
    for(auto d: this->_ns->declarations) {
        std::string dname = d->name;
        if(dname == std::string(name)) {
            olist->append(d);
        }
    }
}


ast_block_context::ast_block_context(ast_context* p, ast_block_stmt* block) : ast_context(p), _block(block) { }

ptr<ast_decl> ast_block_context::find_first_impl(const char* name) {
    for(auto decl: this->_block->decls) {
        std::string dname = decl->name;
        if(dname == std::string(name)) {
            return box<ast_decl>(decl);
        }
    }
    return box<ast_decl>(nullptr);
}

void ast_block_context::find_all_impl(ptr<list<ast_decl>> olist, const char* name) {
    for(auto decl: this->_block->decls) {
        std::string dname = decl->name;
        if(dname == std::string(name)) {
            olist->append(decl);
        }
    }
}


ast_function_context::ast_function_context(ast_context* p, ast_function_decl* func) : ast_context(p), _func(func) { }

ptr<ast_decl> ast_function_context::find_first_impl(const char* name) {
    for(auto param: this->_func->parameters) {
        std::string pname = param->name;
        if(pname == std::string(name)) {
            return box<ast_decl>(param);
        }
    }
    return box<ast_decl>(nullptr);
}

void ast_function_context::find_all_impl(ptr<list<ast_decl>> olist, const char* name) {
    for(auto param: this->_func->parameters) {
        std::string pname = param->name;
        if(pname == std::string(name)) {
            olist->append(param);
        }
    }
}
}

