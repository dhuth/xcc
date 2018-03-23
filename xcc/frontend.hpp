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


typedef int (*pp_function)      (const char*, const char*, const std::vector<std::string>&);
typedef int (*compiler_function)(const char*, const char*, const std::vector<std::string>&);

enum class compiler_stage : uint32_t {
    preprocessor,
    compiler_proper,
    assembler,
    linker,
};

struct preprocessor {
    std::string                                                         pp_name;
    std::string                                                         pp_version;
    pp_function                                                         pp_compiler;
};

struct frontend {
    std::string                                                         language_name;
    std::string                                                         language_version;
    std::string                                                         language_default_pp;
    std::vector<std::string>                                            language_extensions;
    compiler_function                                                   language_compiler;
};

extern const frontend all_frontends[];
extern const preprocessor all_preprocessors[];

inline void __string_concat(std::stringstream& ss) noexcept {
    // do nothing
}

template<typename T, typename... R>
inline void __string_concat(std::stringstream& ss, T&& v0, R&&... r) noexcept {
    ss << v0;
    __string_concat(ss, std::forward<R>(r)...);
}

template<typename... T>
inline std::string string_concat(T&&... args) noexcept {
    std::stringstream ss;
    __string_concat(ss, std::forward<T>(args)...);
    return ss.str();
}

}

#endif /* FRONTEND_HPP_ */
