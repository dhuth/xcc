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

int ximain(const char* input_filename_list, const char* output_filename, std::vector<std::string>& args) {
    translation_unit            unit;

    // TODO: read input args

    xi_builder builder(unit);
    xi::parser p(builder);

    // TODO: for each input file ...
    xiin = fopen(input_filename_list, "r");
    p.parse();
    fclose(xiin);
    // TODO: end for each

    // TODO: read libraries     ???
    // TODO: merge namespaces
    // TODO: high level semantic check
    // TODO: write library      ???
    // TODO: low level semantic check

    // TODO: lower generics / templates
    // TODO: lower closures
    // TODO: builder.lower();                        // lower to ast

    ircode_context ctx(output_filename);
    ctx.generate(unit, output_filename);

    return 0;
}



}





