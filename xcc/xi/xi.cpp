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

#include <xi/xi-parser.hpp>
#include <xi/xi-lex.hpp>
#include <xi/xi.hpp>

#include "ircodegen.hpp"

namespace xcc {

int ximain(const char* input_filename, const char* output_filename, std::vector<std::string>& args) {
    translation_unit            unit;

    //xi_init_printer();

    xiin = fopen(input_filename, "r");

    //if(xi_param_dump_parse) {
    //    std::ofstream s(xi_param_dump_parse_filename);
    //    builder.dump_parse(s);
    //}

    ircode_context ctx(input_filename);
    //builder.generate();

    //if(xi_param_dump_unit) {
    //    std::ofstream s(xi_param_dump_unit_filename);
    //    builder.dump_unit(s);
    //}

    ctx.generate(unit, output_filename);

    return 0;
}



}





