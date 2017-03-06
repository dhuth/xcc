/*
 * xi_builder_struct.cpp
 *
 *  Created on: Feb 26, 2017
 *      Author: derick
 */


#include "xi_builder.hpp"
#include "xi_builder_struct.hpp"

namespace xcc {

xi_struct_context::xi_struct_context(ast_context* parent, xi_struct_decl* decl)
        : ast_context(parent),
          _struct(decl) {
    //...
}

void xi_struct_context::insert(const char* name, ast_decl* decl) {
    assert(decl->is<xi_member_decl>());
    auto member = decl->as<xi_member_decl>();
    member->parent_decl = this->_struct;
    this->_struct->members->append(member);
}

ast_type* xi_struct_context::get_return_type() {
    assert(false);
    return nullptr;
}

ptr<ast_decl> xi_struct_context::find_first_impl(const char* name) {
    for(auto el: this->_struct->members) {
        std::string elname = el->name;
        if(elname == name) {
            return el;
        }
    }
    return nullptr;
}

void xi_struct_context::find_all_impl(ptr<list<ast_decl>> plist, const char* name) {
    for(auto el: this->_struct->members) {
        std::string elname = el->name;
        if(elname == name) {
            plist->append(el);
        }
    }
}

xi_struct_decl* xi_builder::define_global_struct(const char* name, list<ast_type>* basetypes) {
    xi_struct_decl* decl = new xi_struct_decl(name, basetypes);
    this->all_types.push_back(decl->as<xi_type_decl>());
    this->context->insert(name, decl);
    return decl;
}

xi_member_decl* xi_builder::define_field(ast_type* tp, const char* name) {
    auto newfield = new xi_field_decl(name, tp, nullptr);
    this->context->insert(name, newfield);
    return newfield;
}

xi_member_decl* xi_builder::define_field(ast_type* tp, const char* name, ast_expr* init_expr) {
    auto newfield = new xi_field_decl(name, tp, init_expr);
    this->context->insert(name, newfield);
    return newfield;
}

void xi_builder::push_member(xi_struct_decl* decl) {
    this->push_context<xi_struct_context>(decl);
}


}

