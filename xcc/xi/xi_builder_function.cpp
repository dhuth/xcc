/*
 * xi_builder_function.cpp
 *
 *  Created on: Feb 13, 2017
 *      Author: derick
 */

#include "xi_builder.hpp"
#include "xi_builder_function.hpp"
#include "xi_lower.hpp"

namespace xcc {

xi_function_context::xi_function_context(ast_context* p, xi_function_decl* func): ast_context(p), _func(func) { }

void xi_function_context::insert(const char*, ast_decl*) {
    throw std::runtime_error("Not reachable");
}

ast_type* xi_function_context::get_return_type() {
    return this->_func->return_type;
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

static bool same_function_by_signature(xi_builder* builder, xi_function_decl* lfunc, xi_function_decl* rfunc, bool check_return_type) {
    //TODO: check context

    std::string lname = lfunc->name;
    std::string rname = rfunc->name;

    if(lname != rname) {
        return false;
    }

    if(check_return_type) {
        if(!builder->sametype(lfunc->return_type, rfunc->return_type))
            return false;
    }

    if(lfunc->parameters->size() == rfunc->parameters->size()) {
        for(size_t i = 0; i < lfunc->parameters->size(); i++) {
            if(!builder->sametype((*lfunc->parameters)[i]->type, (*rfunc->parameters)[i]->type)) {
                return false;
            }
        }
    }
    return false;
}

xi_function_decl* xi_builder::define_global_function(ast_type* rtype, const char* name, list<xi_parameter_decl>* parameters) {
    auto func = new xi_function_decl(name, rtype, parameters);
    auto others = filter<xi_function_decl>(this->context->findall(name, false));

    for(auto ofunc: others) {
        if(same_function_by_signature(this, func, ofunc, true)) {
            delete func;
            return ofunc;
        }
    }

    this->context->insert(name, func);
    this->all_functions.push_back(func);
    return func;
}

void xi_builder::push_function_and_body(xi_function_decl* func) {
    this->context = new xi_function_context(this->context, func);
    this->push_block(func->body);
}

void xi_builder::push_function(xi_function_decl* func) {
    this->context = new xi_function_context(this->context, func);
}

void xi_builder::pop_function_and_body() {
    this->pop();    // pop function body
    this->pop();    // pop function
}

ast_parameter_decl* xi_builder::lower_parameter(xi_parameter_decl* pdecl) {
    if(!pdecl->generated_parameter) {
        auto gparam = new ast_parameter_decl(pdecl->name, this->lower(pdecl->type));
        pdecl->generated_parameter = gparam;
        //gparam->generated_name = this->get_mangled_name.visit(gparam);
        this->_lower_walker->set(pdecl, gparam);
    }
    return pdecl->generated_parameter;
}

ast_function_decl* xi_builder::lower_function(xi_function_decl* func) {
    if(func->generated_function) {
        return func->generated_function;
    }

    auto lower_parameters = map<ast_parameter_decl, xi_parameter_decl>(func->parameters, [&](xi_parameter_decl* pdecl) -> ast_parameter_decl* {
        auto gparam = this->lower_parameter(pdecl);
        return gparam;
    });
    auto gfunc = new ast_function_decl(func->name, this->lower(func->return_type), lower_parameters, nullptr);
    if(!func->is_c_extern) {
        gfunc->generated_name       = this->get_mangled_name.visit(gfunc);
        gfunc->is_extern            = func->is_extern;
        gfunc->is_extern_visible    = func->is_extern_visible;
        gfunc->is_c_extern          = func->is_c_extern;
    }
    else {
        gfunc->generated_name = func->name;
        gfunc->is_extern            = func->is_extern;
        gfunc->is_extern_visible    = func->is_extern_visible;
        gfunc->is_c_extern          = func->is_c_extern;
    }
    this->_lower_walker->set(func, gfunc);
    return gfunc;
}

void xi_builder::lower_body(xi_function_decl* func) {
    func->generated_function->body = this->lower(func->body->as<ast_stmt>());
}

}


