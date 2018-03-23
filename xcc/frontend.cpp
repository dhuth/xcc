/*
 * frontend.cpp
 *
 *  Created on: Jan 25, 2017
 *      Author: derick
 */

#include "frontend.hpp"
#include "all_frontends.def.hpp"

namespace xcc {

void translation_unit::append(ast_function_decl* decl) noexcept {
    this->global_function_declarations.push_back(box(decl));
}

void translation_unit::append(ast_variable_decl* decl) noexcept {
    this->global_variable_declarations.push_back(box(decl));
}

const frontend all_frontends[] = {
#include "all_frontend_compilers.def.hpp"
        {"", "", "", {""}, NULL}
};

const preprocessor all_preprocessors[] = {
#include "all_preprocessors.def.hpp"
        {"", "", NULL}
};

}

