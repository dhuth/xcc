/*
 * ast.cpp
 *
 *  Created on: Aug 27, 2017
 *      Author: derick
 */


#include "ast.hpp"
#include "error.hpp"

namespace xcc {

const char* to_string(ast_op op) {
    switch(op) {
    case ast_op::none:                      return "none";
    case ast_op::bitcast:                   return "bitcast";
    case ast_op::ineg:                      return "ineg";
    case ast_op::fneg:                      return "fneg";
    case ast_op::iadd:                      return "iadd";
    case ast_op::isub:                      return "isub";
    case ast_op::imul:                      return "imul";
    case ast_op::idiv:                      return "idiv";
    case ast_op::udiv:                      return "udiv";
    case ast_op::imod:                      return "imod";
    case ast_op::umod:                      return "umod";
    case ast_op::fadd:                      return "fadd";
    case ast_op::fsub:                      return "fsub";
    case ast_op::fmul:                      return "fmul";
    case ast_op::fdiv:                      return "fdiv";
    case ast_op::fmod:                      return "fmod";
    case ast_op::band:                      return "band";
    case ast_op::bor:                       return "bor";
    case ast_op::bxor:                      return "bxor";
    case ast_op::bnot:                      return "bnot";
    case ast_op::land:                      return "land";
    case ast_op::lor:                       return "lor";
    case ast_op::lnot:                      return "lnot";
    case ast_op::bshl:                      return "bshl";
    case ast_op::bshr:                      return "bshr";
    case ast_op::ashr:                      return "ashr";
    case ast_op::itof:                      return "itof";
    case ast_op::utof:                      return "utof";
    case ast_op::utop:                      return "utop";
    case ast_op::ftoi:                      return "ftoi";
    case ast_op::ftou:                      return "ftou";
    case ast_op::ptou:                      return "ptou";
    case ast_op::trunc:                     return "trunc";
    case ast_op::ftrunc:                    return "ftrunc";
    case ast_op::sext:                      return "sext";
    case ast_op::zext:                      return "zext";
    case ast_op::fext:                      return "fext";
    case ast_op::cmp_eq:                    return "cmp_eq";
    case ast_op::cmp_ne:                    return "cmp_ne";
    case ast_op::icmp_slt:                  return "icmp_slt";
    case ast_op::icmp_sle:                  return "icmp_sle";
    case ast_op::icmp_sgt:                  return "icmp_sgt";
    case ast_op::icmp_sge:                  return "icmp_sge";
    case ast_op::icmp_ult:                  return "icmp_ult";
    case ast_op::icmp_ule:                  return "icmp_ule";
    case ast_op::icmp_ugt:                  return "icmp_ugt";
    case ast_op::icmp_uge:                  return "icmp_uge";
    case ast_op::fcmp_true:                 return "fcmp_true";
    case ast_op::fcmp_false:                return "fcmp_false";
    case ast_op::fcmp_oeq:                  return "fcmp_oeq";
    case ast_op::fcmp_one:                  return "fcmp_one";
    case ast_op::fcmp_olt:                  return "fcmp_olt";
    case ast_op::fcmp_ole:                  return "fcmp_ole";
    case ast_op::fcmp_ogt:                  return "fcmp_ogt";
    case ast_op::fcmp_oge:                  return "fcmp_oge";
    case ast_op::fcmp_ueq:                  return "fcmp_ueq";
    case ast_op::fcmp_une:                  return "fcmp_une";
    case ast_op::fcmp_ult:                  return "fcmp_ult";
    case ast_op::fcmp_ule:                  return "fcmp_ule";
    case ast_op::fcmp_ugt:                  return "fcmp_ugt";
    case ast_op::fcmp_uge:                  return "fcmp_uge";
    case ast_op::__high_level:              return "__hight_level";
    case ast_op::add:                       return "add";
    case ast_op::sub:                       return "sub";
    case ast_op::mul:                       return "mul";
    case ast_op::div:                       return "div";
    case ast_op::mod:                       return "mod";
    case ast_op::neg:                       return "neg";
    case ast_op::shr:                       return "shr";
    case ast_op::shl:                       return "shl";
    case ast_op::eq:                        return "eq";
    case ast_op::ne:                        return "ne";
    case ast_op::lt:                        return "lt";
    case ast_op::le:                        return "le";
    case ast_op::gt:                        return "gt";
    case ast_op::ge:                        return "ge";
    case ast_op::binary_and:                return "binary_and";
    case ast_op::binary_or:                 return "binary_or";
    case ast_op::binary_xor:                return "binary_xor";
    case ast_op::binary_not:                return "binary_not";
    case ast_op::logical_and:               return "logical_and";
    case ast_op::logical_or:                return "logical_or";
    case ast_op::logical_not:               return "logical_not";
    }
    assert(false);
    return nullptr;
}

}
