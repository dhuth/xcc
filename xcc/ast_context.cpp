/*
 * ast_context.cpp
 *
 *  Created on: Mar 6, 2017
 *      Author: derick
 */


#include "ast_context.hpp"

namespace xcc {

ast_namespace_context::ast_namespace_context(ast_context* p, ast_namespace_decl* ns) : ast_context(p), _ns(ns), _is_global(p == nullptr) { }

void ast_namespace_context::insert(const char* name, ast_decl* decl) {
    this->_ns->declarations->append(decl);
}

ast_type* ast_namespace_context::get_return_type() { return nullptr; }

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

void ast_block_context::insert(const char* name, ast_decl* decl) {
    assert(decl->is<ast_local_decl>());
    this->_block->decls->append(decl->as<ast_local_decl>());
}

ast_type* ast_block_context::get_return_type() { return this->parent->get_return_type(); }

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

void ast_block_context::emit(ast_stmt* stmt) {
    this->_block->stmts->append(stmt);
}


}

