/*
 * token.hpp
 *
 *  Created on: Nov 10, 2017
 *      Author: derick
 */

#ifndef PYLAR_INCLUDE_PYLAR_TOKEN_HPP_
#define PYLAR_INCLUDE_PYLAR_TOKEN_HPP_

#include "pylar/types.hpp"
#include <string>

namespace pylar {


struct source_location {
    std::string                                                 filename;
    uint32_t                                                    line;
    uint32_t                                                    column;
};


struct token {

    inline token(token_id_t symbol) noexcept
            : symbol(symbol) {
        // do nothing
    }

    token_id_t                                                  symbol;

};


struct terminal_token final : public token {

    inline terminal_token(token_id_t symbol, const source_location& location, const std::string& text) noexcept
            : token(symbol),
              location(location),
              text(text) {
        // do nothing
    }

    source_location                                             location;
    std::string                                                 text;

};


struct nonterminal_token final : public token {

    inline nonterminal_token(token_id_t symbol, rule_id_t rule, const token_list_t& rhs) noexcept
            : token(symbol),
              rule(rule),
              rhs(rhs) {
        // do nothing
    }

    rule_id_t                                                   rule;
    token_list_t                                                rhs;

};


}


#endif /* PYLAR_INCLUDE_PYLAR_TOKEN_HPP_ */
