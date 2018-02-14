
#ifndef XCC_XPP_PARSE_HPP_
#define XCC_XPP_PARSE_HPP_

#include <iostream>
#include <sstream>
#include <map>
#include <regex>

namespace xcc {

#define TOKEN(t, ...)                   t,
enum class token_id : uint32_t {
#include "tokens.def.h"
};
#undef  TOKEN

#define CONTEXT_NAME(n)                 n
#define CONTEXT(t, ...)                 t,
enum class parse_context_id : uint32_t {
#include "context.def.h"
};
#undef  CONTEXT
#undef  CONTEXT_NAME

struct parse_context {
    parse_context_id                                        name;
    std::vector<token_id>                                   token_types;
};

struct token {
    token_id                                                type;
    std::string                                             text;
    uint32_t                                                line_number;
    uint32_t                                                column_number;
    uint32_t                                                len;
};

extern const std::map<token_id,         std::string>        patterns;
extern const std::map<parse_context_id, parse_context>      contexts;

}

#endif /* XCC_XPP_PARSE_HPP_ */
