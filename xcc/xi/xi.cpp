/*
 * xi.cpp
 *
 *  Created on: Jan 25, 2017
 *      Author: derick
 */

#include <xi_internal.hpp>
#include <cstdio>

#include "xi-parser.hpp"
#include "xi-lex.hpp"
#include "xi.hpp"

#include "ircodegen.hpp"

namespace xcc {

int ximain(const char* input_filename, const char* output_file, std::vector<std::string>& args) {
    translation_unit            unit;
    xi_builder_t                builder(unit);
    std::string                 output_filename;

    //TODO: handle compiler args

    xiin = fopen(input_filename, "r");
    xiparse(unit, builder);


    ircode_context ctx(output_filename);

    ctx.generate(unit, output_file);

    return 0;
}



}





