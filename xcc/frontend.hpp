/*
 * frontend.hpp
 *
 *  Created on: Jan 25, 2017
 *      Author: derick
 */

#ifndef FRONTEND_HPP_
#define FRONTEND_HPP_

#include "ast.hpp"
#include "ast_builder.hpp"

#include <functional>

namespace xcc {

struct translation_unit {
    ptr<list<ast_function_decl>>                                        functions;
};


typedef int (*compiler_function)(int, char**);

enum class compiler_stage : uint32_t {
    preprocessor,
    compiler_proper,
    compiler_optimization,
    linker,
    linker_optimization,
    assembler
};

struct frontend {
    std::string                                                         language_name;
    std::string                                                         language_version;
    std::vector<std::string>                                            language_extensions;
    compiler_function                                                   language_compiler;
};

extern const frontend all_frontends[];

}

#endif /* FRONTEND_HPP_ */
