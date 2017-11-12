/*
 * types.hpp
 *
 *  Created on: Nov 10, 2017
 *      Author: derick
 */

#ifndef PYLAR_INCLUDE_PYLAR_TYPES_HPP_
#define PYLAR_INCLUDE_PYLAR_TYPES_HPP_

#include <cstdint>

#include <memory>
#include <vector>

namespace pylar {

typedef size_t                                                  token_id_t;
typedef size_t                                                  rule_id_t;

struct token;
typedef std::shared_ptr<token>                                  token_ptr_t;
typedef std::vector<token_ptr_t>                                token_list_t;

}


#endif /* PYLAR_INCLUDE_PYLAR_TYPES_HPP_ */
