/*
 * cstr.cpp
 *
 *  Created on: Feb 26, 2018
 *      Author: derick
 */

#include "cstr.hpp"
#include <sstream>

namespace xcc {

static uint8_t hexval(char c) {
    if(isdigit(c)) {
        return c - '0';
    }
    else if(c >= 'a' && c <= 'f') {
        return (c - 'a') + 10;
    }
    else if(c >= 'A' && c <= 'F') {
        return (c - 'A') + 10;
    }
    //TODO: Handle user error
}

static char read_hex(size_t& i, const std::string& s_in) {
    //TODO: Handle user error
    auto d1 = hexval(s_in[i]);
    auto d0 = hexval(s_in[i + 1]);
    i++;
    return (char) (d1 * 16 + d0);
}

static void handle_escape_sequence(size_t& i, const std::string& s_in, std::stringstream& s_out) noexcept {
    char d = s_in[i];
    switch(d) {
    case '\'':      s_out << '\x27';                break;
    case '"':       s_out << '\x22';                break;
    case '?':       s_out << '\x3f';                break;
    case '\\':      s_out << '\x5c';                break;
    case 'a':       s_out << '\x07';                break;
    case 'b':       s_out << '\x08';                break;
    case 'f':       s_out << '\x0c';                break;
    case 'n':       s_out << '\x0a';                break;
    case 'r':       s_out << '\x0d';                break;
    case 't':       s_out << '\x09';                break;
    case 'v':       s_out << '\x0b';                break;
    case 'x':       s_out << read_hex(++i, s_in);   break;
    default:        s_out << d;
    }
}


std::string cstr_expand_escapes(std::string s_in) noexcept {
    //TODO: Handle user error
    std::stringstream s_out;
    for(size_t i = 0; i < s_in.length(); i++) {
        auto c = s_in[i];
        if(c == '\'') {
            handle_escape_sequence(++i, s_in, s_out);
        }
        else {
            s_out << c;
        }
    }
    return s_out.str();
}

}

