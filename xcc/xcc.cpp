/*
 * xcc.cpp
 *
 *  Created on: Jan 20, 2017
 *      Author: derick
 */

#include <cstring>

#include <map>
#include <regex>
#include <string>
#include <tuple>

#include "frontend.hpp"
#include "argp.h"

struct xcc_program_args {
    xcc::compiler_stage                                                 terminal_stage;

    bool                                                                override_out_filename   = false;
    std::string                                                         out_filename            = "";

    bool                                                                override_compiler_name  = false;
    std::string                                                         compiler_name           = "";
    std::string                                                         compiler_version        = "";

    std::vector<std::string>                                            pp_args;
    std::vector<std::string>                                            compiler_args;

    std::vector<std::string>                                            input_filenames;
};

namespace arg_catagories {
    enum __arg_catagories {
        program,
        preproc,
        compiler,
        linker,
        assembler,
    };
}

#define LANG_STD                    0x1

static const argp_option argp_options[] = {
        {NULL,          'E',        NULL,                   OPTION_ARG_OPTIONAL,    NULL,   arg_catagories::program},
        {NULL,          'c',        NULL,                   OPTION_ARG_OPTIONAL,    NULL,   arg_catagories::program},
        {NULL,          'o',        NULL,                   OPTION_ARG_OPTIONAL,    NULL,   arg_catagories::program},
        {"define",      'D',        "define symbol",        OPTION_ARG_OPTIONAL,    NULL,   arg_catagories::preproc},
        {"include",     'I',        "inlcude path",         OPTION_ARG_OPTIONAL,    NULL,   arg_catagories::preproc},
        {NULL,          'x',        "compiler args",        OPTION_ARG_OPTIONAL,    NULL,   arg_catagories::compiler},
        {"std",         LANG_STD,   NULL,                   OPTION_ARG_OPTIONAL,    NULL,   arg_catagories::compiler},
        //{NULL,          'A',        NULL,                   OPTION_ARG_OPTIONAL,    NULL,   arg_catagories::linker},
        {NULL,          'L',        "library path",         OPTION_ARG_OPTIONAL,    NULL,   arg_catagories::linker},
        {NULL,          'l',        "library",              OPTION_ARG_OPTIONAL,    NULL,   arg_catagories::linker},
        {NULL,          'm',        "target args",          OPTION_ARG_OPTIONAL,    NULL,   arg_catagories::assembler},
        {0,             0,          0,                      0,                      0,      0}
};

static error_t arg_parse_func(int key, char* arg, argp_state* state) {
    xcc_program_args*   program_args = (xcc_program_args*) state->input;
    switch(key) {
    case 'E':           program_args->terminal_stage = xcc::compiler_stage::preprocessor;       break;
    case 'c':           program_args->terminal_stage = xcc::compiler_stage::compiler_proper;    break;
    case 'o':           program_args->out_filename   = std::string(arg);
                        program_args->override_out_filename = true;                             break;
    case 'D':           program_args->pp_args.push_back("-D" + std::string(arg));               break;
    case 'I':           program_args->pp_args.push_back("-I" + std::string(arg));               break;
    case 'x':
        {
            auto strarg = std::string(arg);
            if(strarg.find('=') == std::string::npos) {
                program_args->override_compiler_name = true;
                program_args->compiler_name          = strarg;
            }
            else {
                program_args->compiler_args.push_back(strarg);
            }
        }
        break;
    case ARGP_KEY_ARG:
        program_args->input_filenames.push_back(std::string(arg));
        break;
    case LANG_STD:
        program_args->compiler_version = std::string(arg);
        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static const argp argp_program {
    argp_options,
    &arg_parse_func,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

static xcc::compiler_function get_compiler_by_ext(const std::string& ext) {
    for(int i = 0; !xcc::all_frontends[i].language_name.empty(); i++) {
        for(size_t j = 0; j < xcc::all_frontends[i].language_extensions.size(); j++) {
            if(xcc::all_frontends[i].language_extensions[j] == ext) {
                return xcc::all_frontends[i].language_compiler;
            }
        }
    }
    //TODO: error
    return NULL;
}

static xcc::compiler_function get_compiler_by_name(const std::string& name, const std::string& version) {
    for(int i = 0; !xcc::all_frontends[i].language_name.empty(); i++) {
        if(xcc::all_frontends[i].language_name == name) {
            if(version.empty()) {
                return xcc::all_frontends[i].language_compiler;
            }
            else if(xcc::all_frontends[i].language_version == version) {
                return xcc::all_frontends[i].language_compiler;
            }
        }
    }
    //TODO: error
    return NULL;
}

static inline std::string getext(std::string filename) {
    return filename.substr(filename.find_last_of('.') + 1);
}

static std::string handle_compile_from_file(const std::string& filename, xcc_program_args* args) {
    xcc::compiler_function cfunc;
    if(!args->override_compiler_name) {
        cfunc   = get_compiler_by_ext(getext(filename));
    }
    else {
        cfunc   = get_compiler_by_name(args->compiler_name, args->compiler_version);
    }

    std::string outfile = (filename.substr(0, filename.find_last_of('.')) + ".ll");
    cfunc(filename.c_str(), outfile.c_str(), args->compiler_args);

    return outfile;
}

int main(int argc, char** argv) {
    int                     arg_index = 0;
    xcc_program_args        args;
    args.terminal_stage                 = xcc::compiler_stage::linker;
    args.override_out_filename          = false;
    args.override_compiler_name         = false;

    auto res = argp_parse(&argp_program, argc, argv, 0, &arg_index, &args);

    std::vector<std::string> llfiles;
    for(auto input_file : args.input_filenames) {
        //TODO: preprocess
        llfiles.push_back(handle_compile_from_file(input_file, &args));
    }
}



