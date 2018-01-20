/*
 * xi_context.cpp
 *
 *  Created on: Nov 22, 2017
 *      Author: derick
 */

#include "xi_builder.hpp"

namespace xcc {

void xi_function_context::insert(const char*, ast_decl*) {
    assert(false);
}

ptr<ast_decl> xi_function_context::find_first_impl(const char* name) {
    // search parameters
    for(auto p: this->func->parameters) {
        if(std::string(name) == (std::string) p->name) {
            return p;
        }
    }
    return nullptr;
}

void xi_function_context::find_all_impl(ptr<list<ast_decl>> lout, const char* name) {
    // search parameters
    for(auto p: this->func->parameters) {
        if(std::string(name) == (std::string) p->name) {
            lout->append(p);
        }
    }
}

}


