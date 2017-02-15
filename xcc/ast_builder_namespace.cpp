/*
 * xi_builder_namespace.cpp
 *
 *  Created on: Feb 13, 2017
 *      Author: derick
 */

#include "ast_builder.hpp"

namespace xcc {

ast_namespace_context::ast_namespace_context() : ast_context(), _ns(new ast_namespace_decl("")), _is_global(true) { }
ast_namespace_context::ast_namespace_context(ast_context* p, ast_namespace_decl* ns) : ast_context(p), _ns(ns), _is_global(false) { }

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

ast_namespace_decl* __ast_builder_impl::define_namespace(const char*name) noexcept {
    return new ast_namespace_decl(name);
}

void __ast_builder_impl::push_namespace(ast_namespace_decl* ns) noexcept {
    this->push_context<ast_namespace_context>(ns);
}

}
