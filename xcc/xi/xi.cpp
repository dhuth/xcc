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

int ximain(const char*, const char*, std::map<std::string, std::string>&);
int xi_dev(const char*, const char*, const std::vector<std::string>&);

static void parse_file(const char* filename, xi_builder& builder) noexcept {
    std::string filename_str(filename);
    FILE* in = fopen(filename, "r");

    xiset_in(in);
    xi::parser p(filename_str, builder);
    p.parse();

    fclose(in);
}

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

int ximain(const char* input_filename, const char* output_filename, std::map<std::string, std::string>& args) {
    // TODO: read options

    translation_unit        tu;
    ircode_context          ctx(args["--module-name"]);
    xi_builder              builder(tu);

    // TODO: loop over included library objects
    builder.read_metadata_pass(ctx);

    parse_file(input_filename, builder);

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
    std::map<std::string, std::string> devargs;
    for(auto s: args) {
        auto key = s.substr(0, s.find('='));
        auto val = s.substr(s.find('=') + 1);
        devargs[key] = val;
    }
    return ximain(input_filename, output_filename, devargs);
}

}





