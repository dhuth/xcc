/*
 * frontend.cpp
 *
 *  Created on: Jan 25, 2017
 *      Author: derick
 */

#include "frontend.hpp"
#include "all_frontends.def.hpp"

namespace xcc {

const frontend all_frontends[] = {
#include "all_frontend_compilers.def.hpp"
        {"", "", {""}, NULL}
};


}

