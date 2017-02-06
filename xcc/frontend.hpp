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
    std::vector<ptr<ast_function_decl>>                                 global_function_declarations;
    std::vector<ptr<ast_variable_decl>>                                 global_variable_declarations;

    void append(ast_function_decl*) noexcept;
    void append(ast_variable_decl*) noexcept;
};


typedef int (*compiler_function)(const char*, const char*, std::vector<std::string>&);

enum class compiler_stage : uint32_t {
    preprocessor,
    compiler_proper,
    linker,
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
