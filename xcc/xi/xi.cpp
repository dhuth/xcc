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

extern void setup_xi_printer();

// compiler phases:
//  [.] parse                               parse input file(s)
//  [ ] read-llvm-metadata
//      dom:
//  [x]     merge-namespaces                merge namespaces with the same name
//  [x]     resolve-qnames                  resolve qnames for declarations
//  [.]     merge-decls                     merge all other declarations & definitions (structs, concepts, ...)
//  [ ]     set-member-parent               set member defining types,
//                                          add extension methods to type declarations
//  [.] write-llvm-metadata
//      semantic:
//  [ ]     type-check                      TODO: this is big. should expand
//  [ ]     object-lifetype-analysis
//      lower:
//  [ ]     lower-generics                  TODO: need generics
//  [ ]     promote-closures                TODO: need lambda and whatnot
//  [ ]     methods                         lower xi_method_decls to xi_function_decls (including operators overloads)
//  [ ]     functions                       lower xi_function_decls to ast_function_decls (including operators and overloads and function bodies)

int ximain(const char* input_filename_name, const char* output_filename, std::vector<std::string>& args) {
    setup_xi_printer();

    translation_unit            unit;

    // TODO: read input args

    ircode_context ctx(output_filename);
    xi_builder builder(unit);

    // TODO: loop over included library objects
    builder.read_metadata_pass(ctx);

    // Parse input file
    xi::parser p(std::string(input_filename_name), builder);
    xiin = fopen(input_filename_name, "r");
    p.parse();
    fclose(xiin);

    builder.dom_pass();
    builder.write_metadata_pass(ctx);
    builder.semantic_pass();
    builder.lower_pass();
    ctx.generate(unit, output_filename);

    //TODO: this should maybe be an option after every major pass
    ast_printer::print(builder.get_global_namespace(), std::cout); std::cout << '\n';
    return 0;
}



}





