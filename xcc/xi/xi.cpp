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
//  [x] parse                               parse input file
//  [ ] read-llvm-metadata
//      dom:
//  [x]     merge-namespaces                merge namespaces with the same name
//  [ ]     resolve-qnames                  resolve qnames for declarations
//  [ ]     merge-type-decls                merge type declarations & definitions (structs, concepts, ...)
//  [ ]     merge-function-decls            merge function declarations & definitions (including operator overloads)
//  [ ]     merge-method-decls              merge method declarations & definitions (including operator overloads)
//  [ ] write-llvm-metadata
//      semantic:
//  [ ]     resolve-qnames                  resolve qnames for expressins (should remove id_exprs)
//  [ ]     type-check                      TODO: this is big. should expand
//      lower:
//  [ ]     promote-closures                TODO: need lambda and whatnot
//  [ ]     methods                         lower xi_method_decls to xi_function_decls (including operators overloads)
//  [ ]     functions                       lower xi_function_decls to ast_function_decls (including operators and overloads and function bodies)

int ximain(const char* input_filename_list, const char* output_filename, std::vector<std::string>& args) {
    setup_xi_printer();

    translation_unit            unit;

    // TODO: read input args

    ircode_context ctx(output_filename);
    xi_builder builder(unit);

    // TODO: maybe read metadata

    // Parse input file
    xi::parser p(builder);
    xiin = fopen(input_filename_list, "r");
    p.parse();
    fclose(xiin);

    builder.semantic_check();
    // TODO: maybe write metadata

    // TODO: lower generics / templates
    // TODO: lower closures
    // TODO: builder.lower();                        // lower to ast
    ctx.generate(unit, output_filename);


    ast_printer::print(builder.get_global_namespace(), std::cout); std::cout << '\n';
    return 0;
}



}





