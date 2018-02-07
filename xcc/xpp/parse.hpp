
#ifndef XCC_XPP_PARSE_HPP_
#define XCC_XPP_PARSE_HPP_

#include <iostream>
#include <regex>

namespace xcc {

enum class token_type {
    none,
    newline,
    text,
};

struct token {
};

std::string pp_readline(std::istream&);


}


#endif /* XCC_XPP_PARSE_HPP_ */
