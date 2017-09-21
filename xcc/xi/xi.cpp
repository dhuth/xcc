/*
 * xi.cpp
 *
 *  Created on: Jan 25, 2017
 *      Author: derick
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cstdio>

#include <iostream>
#include <fstream>

#include "xi_builder.hpp"
#include "xi-parser.hpp"
#include "xi-lex.hpp"
#include "xi.hpp"

#include "ircodegen.hpp"

namespace xcc {

int ximain(const char* input_filename, const char* output_filename, std::vector<std::string>& args) {
    translation_unit            unit;

    //TODO: read input args

    xi_builder builder(unit);
    xiin = fopen(input_filename, "r");

    xiparse(builder);

    ircode_context ctx(input_filename);
    ctx.generate(unit, output_filename);

    return 0;
}



}





