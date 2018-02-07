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

int ximain(const char*, const char*, std::vector<std::string>&);
int xi_dev(const char*, const char*, const std::vector<std::string>&);


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
//  [.] mangle names
//      lower:
//  [ ]     lower-generics                  TODO: need generics
//  [ ]     promote-closures                TODO: need lambda and whatnot
//  [.]     methods                         lower xi_method_decls to xi_function_decls (including operators overloads)
//  [.]     functions                       lower xi_function_decls to ast_function_decls (including operators and overloads and function bodies)

int ximain(const char* input_filenamename, const char* output_filename, std::vector<std::string>& args) {
    setup_xi_printer();

    // TODO: read input args

    translation_unit        tu;
    ircode_context          ctx(output_filename);
    xi_builder              builder(tu);

    // TODO: loop over included library objects
    builder.read_metadata_pass(ctx);

    // Parse input file
    xi::parser p(std::string(input_filenamename), builder);
    xiin = fopen(input_filenamename, "r");
    p.parse();
    fclose(xiin);

    builder.dom_pass();
    builder.write_metadata_pass(ctx);
    builder.semantic_pass();
    builder.mangle_names_pass();
    builder.lower_pass(ctx);

    //TODO: this should maybe be an option after every major pass
    //ast_printer::print(builder.get_global_namespace(), std::cout); std::cout << '\n';

    ctx.generate(tu, output_filename);

    return 0;
}

int xi_dev(const char* input_filename, const char* output_filename, const std::vector<std::string>& args) {
    // input args
    std::vector<std::string> devargs;
    for(auto s: args) {
        devargs.push_back(s);
    }
    return ximain(input_filename, output_filename, devargs);
}
}





