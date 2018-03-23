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
#include <sstream>
#include <tuple>

#include "frontend.hpp"
#include "argp.h"

struct xcc_program_args {
    xcc::compiler_stage                                                 first_stage;
    xcc::compiler_stage                                                 terminal_stage;

    bool                                                                override_out_filename   = false;
    std::string                                                         out_filename            = "";

    bool                                                                override_compiler_name  = false;
    std::string                                                         compiler_name           = "";
    std::string                                                         compiler_version        = "";

    std::string                                                         gnu_prefix              = "";

    std::vector<std::string>                                            pp_args;
    std::vector<std::string>                                            compiler_args;
    std::vector<std::string>                                            assembler_args;
    std::vector<std::string>                                            linker_args;

    std::vector<std::string>                                            input_filenames;
};

namespace arg_catagories {
    enum __arg_catagories {
        program,
        preproc,
        compiler,
        assembler,
        gnu,
        linker,
    };
}


#define ARG_LANG_STD                                        0x100
#define ARG_GNU_PREFIX                                      0x101

#define ARG_E_DOC                                           "Only run the preprocessor"
#define ARG_S_DOC                                           "Stop compilation after the compiler proper"
#define ARG_c_DOC                                           "Stop compilation after the assembler"
#define ARG_o_DOC                                           "Output filename"
#define ARG_D_DOC                                           "Define a symbol for the preprocessor"
#define ARG_I_DOC                                           "Add include path for the preprocessor"
#define ARG_U_DOC                                           "Undefine a symbol for the preprocessor"
#define ARG_x_DOC                                           "pass an argument to the compiler proper"
#define ARG_f_DOC                                           "pass a flag to the compiler proper"
#define ARG_LANG_STD_DOC                                    "set the language standard"
#define ARG_m_DOC                                           "pass an argument to the assembler"
#define ARG_O_DOC                                           "set the optimization level"
#define ARG_GNU_PREFIX_DOC                                  NULL
#define ARG_L_DOC                                           "Add a library path to the linker"
#define ARG_l_DOC                                           "Pass a library spec to the linker"

static const argp_option argp_options[] = {
        {NULL,          'E',            NULL,               OPTION_ARG_OPTIONAL,    ARG_E_DOC,          arg_catagories::program},
        {NULL,          'S',            NULL,               OPTION_ARG_OPTIONAL,    ARG_S_DOC,          arg_catagories::program},
        {NULL,          'c',            NULL,               OPTION_ARG_OPTIONAL,    ARG_c_DOC,          arg_catagories::program},
        {NULL,          'o',            "filename",         OPTION_ARG_OPTIONAL,    ARG_o_DOC,          arg_catagories::program},
        {"define",      'D',            "symbol",           OPTION_ARG_OPTIONAL,    ARG_D_DOC,          arg_catagories::preproc},
        {"include",     'I',            "path",             OPTION_ARG_OPTIONAL,    ARG_I_DOC,          arg_catagories::preproc},
        {"undef",       'U',            "symbol",           OPTION_ARG_OPTIONAL,    ARG_U_DOC,          arg_catagories::preproc},
        {NULL,          'x',            "arg=value",        OPTION_ARG_OPTIONAL,    ARG_x_DOC,          arg_catagories::compiler},
        {NULL,          'f',            "flag",             OPTION_ARG_OPTIONAL,    ARG_f_DOC,          arg_catagories::compiler},
        {"std",         ARG_LANG_STD,   "language std",     OPTION_ARG_OPTIONAL,    ARG_LANG_STD_DOC,   arg_catagories::compiler},
        {NULL,          'm',            "args",             OPTION_ARG_OPTIONAL,    ARG_m_DOC,          arg_catagories::assembler},
        {NULL,          'O',            "0|1|2|3",          OPTION_ARG_OPTIONAL,    ARG_O_DOC,          arg_catagories::assembler},
        {"gnu-prefix",  ARG_GNU_PREFIX, "prefix",           OPTION_ARG_OPTIONAL,    ARG_GNU_PREFIX_DOC, arg_catagories::gnu},
        {NULL,          'L',            "path",             OPTION_ARG_OPTIONAL,    ARG_L_DOC,          arg_catagories::linker},
        {NULL,          'l',            "library spec",     OPTION_ARG_OPTIONAL,    ARG_l_DOC,          arg_catagories::linker},
        {0,             0,              0,                  0,                      0,                  0}
};

static error_t arg_parse_func(int key, char* arg, argp_state* state) {
    xcc_program_args*   program_args = (xcc_program_args*) state->input;
    switch(key) {
    case 'E':               program_args->terminal_stage = xcc::compiler_stage::preprocessor;       break;
    case 'S':               program_args->terminal_stage = xcc::compiler_stage::compiler_proper;    break;
    case 'c':               program_args->terminal_stage = xcc::compiler_stage::assembler;          break;
    case 'o':               program_args->out_filename   = std::string(arg);
                            program_args->override_out_filename = true;                             break;
    case 'D':               program_args->pp_args.push_back(xcc::string_concat("-D", arg));         break;
    case 'I':               program_args->pp_args.push_back(xcc::string_concat("-I", arg));         break;
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
    case 'f':               program_args->compiler_args.emplace_back(arg);                          break;
    case ARG_LANG_STD:      program_args->compiler_version = std::string(arg);                      break;
    case 'm':               program_args->assembler_args.push_back(xcc::string_concat("-m", arg));  break;
    case 'O':               program_args->assembler_args.push_back(xcc::string_concat("-O", arg));  break;
    case ARG_GNU_PREFIX:    program_args->gnu_prefix = std::string(arg);                            break;
    case 'l':               program_args->linker_args.push_back(xcc::string_concat("-l", arg));     break;
    case 'L':               program_args->linker_args.push_back(xcc::string_concat("-L", arg));     break;

    case ARGP_KEY_ARG:
        program_args->input_filenames.emplace_back(arg);
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

static const xcc::frontend* get_frontend_by_ext(const std::string& ext) {
    for(size_t i = 0; !xcc::all_frontends[i].language_name.empty(); i++) {
        for(size_t j = 0; j < xcc::all_frontends[i].language_extensions.size(); j++) {
            if(xcc::all_frontends[i].language_extensions[j] == ext) {
                return &xcc::all_frontends[i];
            }
        }
    }

    //TODO: error
    return NULL;
}

static const xcc::frontend* get_frontend_by_name(const std::string& name, const std::string& version) {
    for(size_t i = 0; !xcc::all_frontends[i].language_name.empty(); i++) {
        if(xcc::all_frontends[i].language_name == name) {
            if(version.empty()) {
                return &xcc::all_frontends[i];
            }
            else if(xcc::all_frontends[i].language_version == version) {
                return &xcc::all_frontends[i];
            }
        }
    }

    //TODO: error
    return NULL;
}

static inline std::string getext(std::string filename) {
    return filename.substr(filename.find_last_of('.') + 1);
}

static bool run_compiler_function(const xcc::compiler_function& f, std::string in_file, std::string out_file, const std::vector<std::string>& args) {
    if(f(in_file.c_str(), out_file.c_str(), args) != 0) {
        return false;
    }
    return true;
}

static std::string handle_pp_from_file(const std::string& filename, xcc_program_args* args) {
    //TODO: preprocess input file
    return filename;
}

static std::string maybe_handle_pp(const std::string& filename, xcc_program_args* args) {
    //TODO: maybe?
    return handle_pp_from_file(filename, args);
}

static std::string handle_compile_from_file(const std::string& src_filename, xcc_program_args* args) {
    std::string module_name = src_filename;
    std::string pp_filename = maybe_handle_pp(src_filename, args);

    assert(args->terminal_stage >= xcc::compiler_stage::compiler_proper);

    const xcc::frontend* lang;
    if(!args->override_compiler_name) {
        lang    = get_frontend_by_ext(getext(src_filename));
    }
    else {
        lang    = get_frontend_by_name(args->compiler_name, args->compiler_version);
    }

    std::string outfile;
    if((args->terminal_stage == xcc::compiler_stage::compiler_proper) &&
            (args->override_out_filename)) {
        outfile = args->out_filename;
    }
    else {
        switch(args->terminal_stage) {
        case xcc::compiler_stage::compiler_proper:
        case xcc::compiler_stage::assembler:
        case xcc::compiler_stage::linker:
            outfile = (src_filename.substr(0, src_filename.find_last_of('.')) + "." "ll");
            break;
        default:
            throw std::runtime_error(
                    std::string("runtime error: ") +
                    std::string(__FILE__) + ", " +
                    std::to_string(__LINE__) + "compiler stage not supported");
        }
    }

    args->compiler_args.push_back(xcc::string_concat("--module-name=", src_filename));
    run_compiler_function(lang->language_compiler, pp_filename, outfile, args->compiler_args);

    return outfile;
}

static std::string handle_single_input_file(xcc_program_args* args) {
    if(args->terminal_stage == xcc::compiler_stage::preprocessor) {
        return handle_pp_from_file(args->input_filenames[0], args);
    }
    else {
        return handle_compile_from_file(args->input_filenames[0], args);
    }
}

static void handle_multiple_input_files(std::vector<std::string>& outfiles, xcc_program_args* args) {
    for(auto input_filename : args->input_filenames) {
        outfiles.push_back(handle_compile_from_file(input_filename, args));
    }
}

static void compile_llvm_file(const std::string& infile, const std::string& outfile, xcc_program_args* args) {
    std::stringstream cmd;

    cmd << LLVM_EXEC_LLC << " -o " << outfile;

    for(auto cf: args->assembler_args) {
        cmd << " " << cf;
    }
    //TODO: language specific llvm compiler options

    //TODO: handle system output
    system(cmd.str().c_str());
}

static std::string handle_compile_llvm_file(const std::string& infile, xcc_program_args* args) {
    std::string outfile;
    if((args->terminal_stage == xcc::compiler_stage::assembler) &&
            (args->override_out_filename)) {
        outfile = args->out_filename;
    }
    else {
        switch(args->terminal_stage) {
        case xcc::compiler_stage::linker:
        case xcc::compiler_stage::assembler:
            outfile = (infile.substr(0, infile.find_last_of('.')) + ".o");
            break;
        default:
            throw std::runtime_error(
                    std::string("runtime error: ") +
                    std::string(__FILE__) + ", " +
                    std::to_string(__LINE__) + "assembler stage not supported");
        }
    }

    compile_llvm_file(infile, outfile, args);
    return outfile;
}

static void link_llvm_input_files(std::vector<std::string>& infiles, const char* outfile, xcc_program_args* args) {
    //TODO: more....
    std::stringstream cmd;
    cmd << args->gnu_prefix << "ld" << " -o " << outfile;
    for(auto infile: infiles) {
        cmd << " " << infile;
    }
    //TODO: language specific options
    //TODO: handle system output
    system(cmd.str().c_str());
}

static void llvm_backend(std::vector<std::string>& llfiles, xcc_program_args* args) {
    std::vector<std::string> bcfiles;
    for(auto llf: llfiles) {
        auto ext = getext(llf);
        if(ext == "ll") {
            bcfiles.push_back(handle_compile_llvm_file(llf, args));
        }
        else if(ext == "o" || ext == "a" || ext == "bc") {
            bcfiles.push_back(llf);
        }
        else {
            throw std::runtime_error("unknown file extension to backend");
        }
    }

    if(args->terminal_stage >= xcc::compiler_stage::linker) {
        link_llvm_input_files(bcfiles, args->out_filename.c_str(), args);
    }
}

int main(int argc, char** argv) {
    int                     arg_index = 0;
    xcc_program_args        args;
    args.first_stage                    = xcc::compiler_stage::preprocessor;
    args.terminal_stage                 = xcc::compiler_stage::assembler;
    args.override_out_filename          = false;
    args.override_compiler_name         = false;

    auto res = argp_parse(&argp_program, argc, argv, 0, &arg_index, &args);
    // TODO: handle res error
    // TODO: handle input

    // Compile all input files
    std::vector<std::string> outfiles;
    if(args.input_filenames.size() > 1) {
        outfiles.push_back(handle_single_input_file(&args));
    }
    else {
        handle_multiple_input_files(outfiles, &args);
    }

    // Assemble and Link files
    if(args.terminal_stage >= xcc::compiler_stage::linker) {
        llvm_backend(outfiles, &args);
    }
}



