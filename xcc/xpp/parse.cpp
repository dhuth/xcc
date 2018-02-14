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
#define CONTEXT(n , l)          {n, {n, l}},
const std::map<parse_context_id, parse_context> contexts = {
#include "context.def.h"
};
#undef  CONTEXT_NAME
#undef  TOKEN_NAME
#undef  CB
#undef  CONTEXT



}


