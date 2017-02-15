/*
 * xi_builder_function.cpp
 *
 *  Created on: Feb 13, 2017
 *      Author: derick
 */

#include "xi_builder.hpp"

namespace xcc {

xi_function_context::xi_function_context(ast_context* p, xi_function_decl* func): ast_context(p), _func(func) { }

void xi_function_context::insert(const char*, ast_decl*) {
    throw std::runtime_error("Not reachable");
}

ptr<ast_decl> xi_function_context::find_first_impl(const char* name) {
    for(auto p: this->_func->parameters) {
        std::string pname = p->name;
        if(pname == std::string(name)) {
            return box<ast_decl>(p);
        }
    }
    return box<ast_decl>(nullptr);
}

void xi_function_context::find_all_impl(ptr<list<ast_decl>> olist, const char* name) {
    for(auto p: this->_func->parameters) {
        std::string pname = p->name;
        if(pname == std::string(name)) {
            olist->append(p->as<ast_decl>());
        }
    }
}

xi_parameter_decl* xi_builder::define_parameter(ast_type* type, const char* name) {
    return new xi_parameter_decl(name, type);
}

xi_parameter_decl* xi_builder::define_parameter(ast_type* type) {
    return new xi_parameter_decl("$annon", type);
}

xi_function_decl* xi_builder::define_global_function(ast_type* rtype, const char* name, list<xi_parameter_decl>* parameters) {
    auto func = new xi_function_decl(name, rtype, parameters);

    this->context->insert(name, func);
    this->all_functions->append(func);
    return func;

}

void xi_builder::push_function(xi_function_decl* func) {
    this->context = new xi_function_context(this->context, func);
    this->push_block(func->body);
}

void xi_builder::pop_function() {
    this->pop();    // pop function body
    this->pop();    // pop function
}

ast_function_decl* xi_builder::flatten_function(xi_function_decl* func) {
    ptr<list<ast_parameter_decl>> flat_parameters = new list<ast_parameter_decl>();

    //TODO: flatten types
    for(auto param: func->parameters) {
        auto fparam = new ast_parameter_decl(param->name, this->flatten(param->type));
        param->generated_parameter = fparam;
        flat_parameters->append(fparam);
    }

    return new ast_function_decl(func->name, this->flatten(func->return_type), flat_parameters, nullptr);
}

void xi_builder::flatten_body(xi_function_decl* func) {
    //TODO: flatten stmts...
}

}


