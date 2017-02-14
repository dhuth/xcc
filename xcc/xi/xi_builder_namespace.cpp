/*
 * xi_builder_namespace.cpp
 *
 *  Created on: Feb 13, 2017
 *      Author: derick
 */

#include "xi_builder.hpp"

namespace xcc {

xi_namespace_context::xi_namespace_context() : xi_context(), _ns(new ast_namespace_decl("")), _is_global(true) { }
xi_namespace_context::xi_namespace_context(xi_context* p, ast_namespace_decl* ns) : xi_context(p), _ns(ns), _is_global(false) { }

void xi_namespace_context::insert(const char* name, ast_decl* decl) {
    this->_ns->declarations->append(decl);
}

ptr<ast_decl> xi_namespace_context::find_first_impl(const char* name) {
    for(auto d: this->_ns->declarations) {
        if(((std::string) d->name) == std::string(name)) {
            return box(d);
        }
    }
    return box<ast_decl>(nullptr);
}

void xi_namespace_context::find_all_impl(ptr<list<ast_decl>> olist, const char* name) {
    for(auto d: this->_ns->declarations) {
        std::string dname = d->name;
        if(dname == std::string(name)) {
            olist->append(d);
        }
    }
}

ast_namespace_decl* xi_builder::define_namespace(const char*name) {
    return new ast_namespace_decl(name);
}

void xi_builder::push_namespace(ast_namespace_decl* ns) {
    this->_context = new xi_namespace_context(this->_context, ns);
}

}
