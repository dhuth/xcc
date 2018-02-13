/*
 * parse.cpp
 *
 *  Created on: Feb 5, 2018
 *      Author: derick
 */

#include "parse.hpp"

namespace xcc {

#define TOKEN(t, n)             {token_id::t, n},
const std::map<token_id, std::string> patterns = {
#include "tokens.def.h"
};
#undef  TOKEN

#define CONTEXT_NAME(n)         parse_context_id::n
#define TOKEN_NAME(t)           token_id::t
#define CB(...)                 { __VA_ARGS__ }
#define CONTEXT(n , l)          {n, {n, l}}
const std::map<parse_context_id, parse_context> contexts = {
#include "context.def.h"
};
#undef  CONTEXT_NAME
#undef  TOKEN_NAME
#undef  CB
#undef  CONTEXT


static std::string read_squote_line(std::iostream& s_input) noexcept {
    std::stringstream   s_output;
    bool                ignore_quote = false;
    while(!s_input.eof()) {
    }
}


static std::string read_paren_enclosed_line(std::iostream& s_input, int depth) noexcept {
    std::stringstream   s_output;
    while(!s_input.eof()) {
        char c = s_input.get();
        switch(c) {
        case ')':
            s_output << c;
            depth--;
            if(depth == 0)
                return s_output.str();
            break;
        case '(':
            s_output << c;
            depth++;
            break;
        default:
            s_output << c;
        }
    }
    return s_output.str();
}


static std::string readline(std::iostream& s_input) noexcept {
    std::stringstream   s_output;
    bool                ignore_nl = false;

    while(!s_input.eof()) {
        char c = s_input.get();
        switch(c) {
        case '\n':
            s_output << c;
            if(!ignore_nl)
                return s_output.str();
            break;
        case '\\':
            s_output << c;
            ignore_nl = true;
            break;
        case '(':
            s_output << c;
            s_output << read_paren_enclosed_line(s_input, 1);
            break;
        case '"':
            s_output << c;
            s_output << read_dquote_line(s_input);
            break;
        case '\'':
            s_output << c;
            s_output << read_squote_line(s_input);
            break;
        default:
        }
        s_output << c;
    }
    return s_output.str();
}



}


