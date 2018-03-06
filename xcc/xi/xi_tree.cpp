/*
 * xi_tree.cpp
 *
 *  Created on: Nov 9, 2017
 *      Author: derick
 */

#include "xi_tree.hpp"
#include "error.hpp"


namespace xcc {

std::string to_string(xcc::xi_op_expr::xi_operator op) noexcept {
#define CASE(op) case xcc::xi_op_expr::xi_operator::op: return #op;
    switch(op) {
        CASE(__none__)
        CASE(__and__)
        CASE(__or__)
        CASE(__xor__)
        CASE(__not__)
        CASE(__rand__)
        CASE(__ror__)
        CASE(__rxor__)
        CASE(__iand__)
        CASE(__ior__)
        CASE(__ixor__)
        CASE(__add__)
        CASE(__sub__)
        CASE(__mul__)
        CASE(__div__)
        CASE(__mod__)
        CASE(__radd__)
        CASE(__rsub__)
        CASE(__rmul__)
        CASE(__rdiv__)
        CASE(__rmod__)
        CASE(__iadd__)
        CASE(__isub__)
        CASE(__imul__)
        CASE(__idiv__)
        CASE(__imod__)
        CASE(__sl__)
        CASE(__sr__)
        CASE(__band__)
        CASE(__bor__)
        CASE(__bxor__)
        CASE(__bnot__)
        CASE(__rsl__)
        CASE(__rsr__)
        CASE(__rband__)
        CASE(__rbor__)
        CASE(__rbxor__)
        CASE(__isl__)
        CASE(__isr__)
        CASE(__iband__)
        CASE(__ibor__)
        CASE(__ibxor__)
        CASE(__lt__)
        CASE(__le__)
        CASE(__gt__)
        CASE(__ge__)
        CASE(__eq__)
        CASE(__ne__)
        CASE(__rlt__)
        CASE(__rle__)
        CASE(__rgt__)
        CASE(__rge__)
        CASE(__req__)
        CASE(__rne__)
        CASE(__invoke__)
        CASE(__index__)
        CASE(__ctor__)
        CASE(__dtor__)
        CASE(__get__)
        CASE(__set__)
        CASE(__alloc__)
        CASE(__dealloc__)
        CASE(__assign__)
        CASE(__move__)
        CASE(__copy__)
        CASE(__deref__)
        CASE(__address_of__)
    default:
        return "<<unknown_op>>";
    }
#undef CASE
}

}

namespace std {
std::string to_string(xcc::xi_op_expr::xi_operator op) noexcept {
    return xcc::to_string(op);
}
}

