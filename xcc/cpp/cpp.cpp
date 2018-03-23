/*
 * cpp.cpp
 *
 *  Created on: Mar 23, 2018
 *      Author: derick
 */

#include "cpp.hpp"
#include <sstream>

namespace xcc {

int cpp_main(const char* infile, const char* outfile, std::vector<std::string>& args) {
    std::stringstream cmd;
    cmd << "/usr/bin/cpp";
    for(auto a: args) {
        cmd << " " << a;
    }
    cmd << " " << infile << " -o " << outfile;

    return system(cmd.str().c_str());
}

}

