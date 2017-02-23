/*
 * xi.cpp
 *
 *  Created on: Jan 25, 2017
 *      Author: derick
 */

#include <xi_internal.hpp>
#include <cstdio>

#include <iostream>
#include <fstream>

#include "xi-parser.hpp"
#include "xi-lex.hpp"
#include "xi.hpp"

#include "ircodegen.hpp"

namespace xcc {

static void maybe_dump_unit(bool dump_unit, std::string filename, translation_unit& unit) {
    if(dump_unit) {
        std::ofstream ostr(filename);
        for(auto decl: unit.global_variable_declarations) {
            ast_printer::print(decl, ostr);
        }
        for(auto decl: unit.global_function_declarations) {
            ast_printer::print(decl, ostr);
        }
    }
}

int ximain(const char* input_filename, const char* output_filename, std::vector<std::string>& args) {
    translation_unit            unit;
    xi_builder_t                builder(unit);

    bool                        xi_param_dump_unit              = false;
    std::string                 xi_param_dump_unit_filename     = "-";

    for(auto a: args) {
        if(a.substr(0,9) == "dump-unit") {
            xi_param_dump_unit = true;
            auto rest = a.substr(9);
            if(!rest.empty() && rest[0] == '=') {
                xi_param_dump_unit_filename = rest.substr(1);
            }
            else {
                xi_param_dump_unit_filename = std::string(input_filename) + std::string(".dump-unit");
            }
        }
    }

    xiin = fopen(input_filename, "r");
    xiparse(builder);

    builder.resolution_pass();
    builder.lower_pass();

    maybe_dump_unit(xi_param_dump_unit, xi_param_dump_unit_filename, unit);

    ircode_context ctx(input_filename);
    ctx.generate(unit, output_filename);

    return 0;
}



}





