/*
 * xi.cpp
 *
 *  Created on: Jan 25, 2017
 *      Author: derick
 */

#include <cstdio>

#include "xi-internal.hpp"
#include "xi-parser.hpp"
#include "xi-lex.hpp"
#include "xi.hpp"

namespace xcc {

int ximain(const char* input_filename, std::vector<std::string>& args) {
    translation_unit            unit;
    xi_builder_t                builder;

    xiin = fopen(input_filename, "r");
    xiparse(unit, builder);

    return 0;
}



}





