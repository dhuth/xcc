/*
 * ast_eval.cpp
 *
 *  Created on: Jan 19, 2018
 *      Author: derick
 */

#include "ast_eval.hpp"

namespace xcc {

// Constant values
template<typename T> static ptr<ast_expr> ast_eval_constant(T* v, ast_compile_time_evaluator&, ast_eval_context&) { return v; }

// Cast helpers
static ptr<ast_expr> ast_eval_cast_bitcast(ast_type* tp, ast_expr* expr) {
    //TODO: some assertions...
    switch(expr->get_tree_type()) {
    case tree_type_id::ast_integer:     return box<ast_expr>(new ast_integer(tp, (llvm::APSInt)  expr->as<ast_integer>()->value));
    case tree_type_id::ast_real:        return box<ast_expr>(new ast_real   (tp, (llvm::APFloat) expr->as<ast_real>()->value));
    case tree_type_id::ast_string:      return box<ast_expr>(new ast_string (tp, (std::string)   expr->as<ast_string>()->value));
    case tree_type_id::ast_array:       return box<ast_expr>(new ast_array  (tp,                 expr->as<ast_array>()->values));
    case tree_type_id::ast_record:      return box<ast_expr>(new ast_record (tp,                 expr->as<ast_record>()->values));
    default:
        // TODO: complain
        break;
    }
    return box<ast_expr>((ast_expr*) nullptr);
}

static ptr<ast_expr> ast_eval_cast_trunc(ast_type* tp, ast_expr* expr) {
    //TODO: some assertions...
    auto value      = (llvm::APSInt) expr->as<ast_integer>()->value;
    auto new_width  = (uint32_t)     tp->as<ast_integer_type>()->bitwidth;

    return box<ast_expr>(new ast_integer(tp, value.trunc(new_width)));
}

static ptr<ast_expr> ast_eval_cast_zext(ast_type* tp, ast_expr* expr) {
    //TODO: some assertions...
    auto value      = (llvm::APSInt) expr->as<ast_integer>()->value;
    auto new_width  = (uint32_t)     tp->as<ast_integer_type>()->bitwidth;

    return box<ast_expr>(new ast_integer(tp, value.zext(new_width)));
}

static ptr<ast_expr> ast_eval_cast_sext(ast_type* tp, ast_expr* expr) {
    //TODO: some assertions...
    auto value      = (llvm::APSInt) expr->as<ast_integer>()->value;
    auto new_width  = (uint32_t)     tp->as<ast_integer_type>()->bitwidth;

    return box<ast_expr>(new ast_integer(tp, value.sext(new_width)));
}

static const llvm::fltSemantics& __get_semantics(uint32_t bitwidth) {
    switch(bitwidth) {
    case 16:    return llvm::APFloat::IEEEhalf;
    case 32:    return llvm::APFloat::IEEEsingle;
    case 64:    return llvm::APFloat::IEEEdouble;
    case 128:   return llvm::APFloat::IEEEquad;
    default:
        //TODO:
        assert(false);
        return llvm::APFloat::Bogus;
    }
}

static ptr<ast_expr> ast_eval_cast_fp(ast_type* tp, ast_expr* expr) {
    //TODO: some assertions...
    auto value      = (llvm::APFloat) expr->as<ast_real>()->value;
    auto new_width  = (uint32_t)      tp->as<ast_real_type>()->bitwidth;
    bool throwaway  = false;

    value.convert(__get_semantics(new_width), llvm::APFloat::roundingMode::rmTowardZero, &throwaway);
    return box<ast_expr>(new ast_real(tp, value)); //TODO: something else...
}

static ptr<ast_expr> ast_eval_cast_itof(ast_type* tp, ast_expr* expr, bool is_unsigned) {
    //TODO: some assertions...
    auto value      = (llvm::APSInt) expr->as<ast_integer>()->value;
    auto bitwidth   = (uint32_t)     tp->as<ast_real_type>()->bitwidth;

    auto nvalue     = llvm::APFloat(__get_semantics(bitwidth));
    nvalue.convertFromAPInt(value, !is_unsigned, llvm::APFloat::roundingMode::rmTowardZero);
    return box<ast_expr>(new ast_real(tp, nvalue));
}

static ptr<ast_expr> ast_eval_cast_ftoi(ast_type* tp, ast_expr* expr, bool is_unsigned) {
    //TODO: some assertions...
    auto value      = (llvm::APFloat) expr->as<ast_real>()->value();
    auto bitwidth   = (uint32_t)      tp->as<ast_integer_type>()->bitwidth;
    bool throwaway  = false;

    llvm::APSInt nvalue;
    value.convertToInteger(nvalue, llvm::APFloat::roundingMode::rmTowardZero, &throwaway);
    return box<ast_expr>(new ast_integer(tp, nvalue));
}

// Cast operators
static ptr<ast_expr> ast_eval_cast(ast_cast* cexpr, ast_compile_time_evaluator& evaluator, ast_eval_context& context) {
    auto expr = evaluator.visit(cexpr->expr, evaluator, context);

    switch((ast_op)cexpr->op) {
    case ast_op::bitcast:               return ast_eval_cast_bitcast(cexpr->type, expr);
    case ast_op::trunc:                 return ast_eval_cast_trunc(cexpr->type, expr);
    case ast_op::zext:                  return ast_eval_cast_zext(cexpr->type, expr);
    case ast_op::sext:                  return ast_eval_cast_sext(cexpr->type, expr);
    case ast_op::ftrunc:                return ast_eval_cast_fp(cexpr->type, expr);
    case ast_op::fext:                  return ast_eval_cast_fp(cexpr->type, expr);
    case ast_op::utof:                  return ast_eval_cast_itof(cexpr->type, expr, true);
    case ast_op::itof:                  return ast_eval_cast_itof(cexpr->type, expr, false);
    case ast_op::ftou:                  return ast_eval_cast_ftoi(cexpr->type, expr, true);
    case ast_op::ftoi:                  return ast_eval_cast_ftoi(cexpr->type, expr, false);
    case ast_op::ptou:                  return ast_eval_cast_bitcast(cexpr->type, expr); //TODO: constant pointer values to declarations and whatnot
    case ast_op::utop:                  return ast_eval_cast_bitcast(cexpr->type, expr); //TODO: constant pointer values to declarations and whatnot
    default:
        assert(false);
    }
}

#define __get_ivalue(e)                 ((llvm::APSInt)  e->as<ast_integer>()->value)
#define __get_fvalue(e)                 ((llvm::APFloat) e->as<ast_real>()->value)

#define __do_iop(l, op, r)              (__get_ivalue(l) op __get_ivalue(r))
#define __do_imt(l, mt, r)              (__get_ivalue(l).mt(__get_ivalue(r)))
#define __do_fop(l, op, r)              (__get_fvalue(l) op __get_fvalue(r))
#define __do_fmt(l, mt, r)              (__get_fvalue(l).mt(__get_fvalue(r)))

#define __box_iop(tp, l, op, r)         box<ast_expr>(new ast_integer(tp, __do_iop(l, op, r)))
#define __box_imt(tp, l, mt, r)         box<ast_expr>(new ast_integer(tp, __do_imt(l, mt, r)))
#define __box_fop(tp, l, op, r)         box<ast_expr>(new ast_real(tp, __do_fop(l, op, r)))
#define __box_fmt(tp, l, mt, r)         box<ast_expr>(new ast_real(tp, __do_fmt(l, mt, r)))

#define __box_bool(tp, v)               box<ast_expr>(new ast_integer(tp, llvm::APSInt(1, ((v) ? 1 : 0), false)))

// Binary operators
static ptr<ast_expr> ast_eval_binary_op(ast_binary_op* opexpr, ast_compile_time_evaluator& evaluator, ast_eval_context& context) {
    auto lexpr = evaluator.visit(opexpr->lhs, evaluator, context);
    auto rexpr = evaluator.visit(opexpr->rhs, evaluator, context);

    switch((ast_op)opexpr->op) {
        // Unsigned or dont care
    case ast_op::iadd:                  return __box_iop(opexpr->type, lexpr,    +, rexpr);
    case ast_op::isub:                  return __box_iop(opexpr->type, lexpr,    -, rexpr);
    case ast_op::imul:                  return __box_iop(opexpr->type, lexpr,    *, rexpr);
    case ast_op::udiv:                  return __box_imt(opexpr->type, lexpr, udiv, rexpr);
    case ast_op::umod:                  return __box_imt(opexpr->type, lexpr, urem, rexpr);
    case ast_op::band:                  return __box_iop(opexpr->type, lexpr,    &, rexpr);
    case ast_op::bor:                   return __box_iop(opexpr->type, lexpr,    |, rexpr);
    case ast_op::bxor:                  return __box_iop(opexpr->type, lexpr,    ^, rexpr);
    case ast_op::bshl:                  return __box_imt(opexpr->type, lexpr,  shl, rexpr);
    case ast_op::bshr:                  return __box_imt(opexpr->type, lexpr, lshr, rexpr);
    case ast_op::ashr:                  return __box_imt(opexpr->type, lexpr, ashr, rexpr);
        // Signed
    case ast_op::idiv:                  return __box_imt(opexpr->type, lexpr, sdiv, rexpr);
    case ast_op::imod:                  return __box_imt(opexpr->type, lexpr, srem, rexpr);
        // Floating point
    case ast_op::fadd:                  return __box_fop(opexpr->type, lexpr, +, rexpr);
    case ast_op::fsub:                  return __box_fop(opexpr->type, lexpr, -, rexpr);
    case ast_op::fmul:                  return __box_fop(opexpr->type, lexpr, *, rexpr);
    case ast_op::fdiv:                  return __box_fop(opexpr->type, lexpr, /, rexpr);
    case ast_op::fmod:                  // TODO: what to do?
        // Unsigned comparisons
    case ast_op::cmp_eq:                return __box_bool(opexpr->type, __do_iop(lexpr,  ==, rexpr));
    case ast_op::cmp_ne:                return __box_bool(opexpr->type, __do_iop(lexpr,  !=, rexpr));
    case ast_op::icmp_ult:              return __box_bool(opexpr->type, __do_imt(lexpr, ult, rexpr));
    case ast_op::icmp_ule:              return __box_bool(opexpr->type, __do_imt(lexpr, ule, rexpr));
    case ast_op::icmp_ugt:              return __box_bool(opexpr->type, __do_imt(lexpr, ugt, rexpr));
    case ast_op::icmp_uge:              return __box_bool(opexpr->type, __do_imt(lexpr, uge, rexpr));
        // Signed comparisons
    case ast_op::icmp_slt:              return __box_bool(opexpr->type, __do_imt(lexpr, slt, rexpr));
    case ast_op::icmp_sle:              return __box_bool(opexpr->type, __do_imt(lexpr, sle, rexpr));
    case ast_op::icmp_sgt:              return __box_bool(opexpr->type, __do_imt(lexpr, sgt, rexpr));
    case ast_op::icmp_sge:              return __box_bool(opexpr->type, __do_imt(lexpr, sge, rexpr));
        // Floating point comparisons
    case ast_op::fcmp_oeq:              return __box_bool(opexpr->type, __do_fmt(lexpr, compare, rexpr) == llvm::APFloat::cmpEqual);
    case ast_op::fcmp_one:              return __box_bool(opexpr->type, __do_fmt(lexpr, compare, rexpr) != llvm::APFloat::cmpEqual);
    case ast_op::fcmp_olt:              return __box_bool(opexpr->type, __do_fmt(lexpr, compare, rexpr) == llvm::APFloat::cmpLessThan);
    case ast_op::fcmp_ole:              return __box_bool(opexpr->type, __do_fmt(lexpr, compare, rexpr) != llvm::APFloat::cmpGreaterThan);
    case ast_op::fcmp_ogt:              return __box_bool(opexpr->type, __do_fmt(lexpr, compare, rexpr) == llvm::APFloat::cmpGreaterThan);
    case ast_op::fcmp_oge:              return __box_bool(opexpr->type, __do_fmt(lexpr, compare, rexpr) != llvm::APFloat::cmpLessThan);
    case ast_op::fcmp_ueq:              return __box_bool(opexpr->type, __do_fmt(lexpr, compare, rexpr) == llvm::APFloat::cmpEqual);
    case ast_op::fcmp_une:              return __box_bool(opexpr->type, __do_fmt(lexpr, compare, rexpr) != llvm::APFloat::cmpEqual);
    case ast_op::fcmp_ult:              return __box_bool(opexpr->type, __do_fmt(lexpr, compare, rexpr) == llvm::APFloat::cmpLessThan);
    case ast_op::fcmp_ule:              return __box_bool(opexpr->type, __do_fmt(lexpr, compare, rexpr) != llvm::APFloat::cmpGreaterThan);
    case ast_op::fcmp_ugt:              return __box_bool(opexpr->type, __do_fmt(lexpr, compare, rexpr) == llvm::APFloat::cmpGreaterThan);
    case ast_op::fcmp_uge:              return __box_bool(opexpr->type, __do_fmt(lexpr, compare, rexpr) != llvm::APFloat::cmpLessThan);
    //TODO: logical operators
    }
}

// Unary op
static ptr<ast_expr> ast_eval_unary_op(ast_unary_op* opexpr, ast_compile_time_evaluator& evaluator, ast_eval_context& context) {
    //TODO: ...
}

// Index
static ptr<ast_expr> ast_eval_index(ast_index* iexpr, ast_compile_time_evaluator& evaluator, ast_eval_context& context) {
    //TODO: ...
}

// Declref
static ptr<ast_expr> ast_eval_declref(ast_declref* dexpr, ast_compile_time_evaluator& evaluator, ast_eval_context& context) {
    //TODO: ...
}

// Memberref
static ptr<ast_expr> ast_eval_memberref(ast_memberref* mexpr, ast_compile_time_evaluator& evaluator, ast_eval_context& context) {
    //TODO: ...
}

// Deref
static ptr<ast_expr> ast_eval_deref(ast_deref* rexpr, ast_compile_time_evaluator& evaluator, ast_eval_context& context) {
    //TODO: ...
}

// Addressof
static ptr<ast_expr> ast_eval_addressof(ast_addressof* aexpr, ast_compile_time_evaluator& evaluator, ast_eval_context& context) {
    //TODO: ...
}

// Invoke
static ptr<ast_expr> ast_eval_invoke(ast_invoke* iexpr, ast_compile_time_evaluator& evaluator, ast_eval_context& context) {
    //TODO: ...
}

// Call
static ptr<ast_expr> ast_eval_call(ast_call* cexpr, ast_compile_time_evaluator& evaluator, ast_eval_context& context) {
    //TODO: ...
}

// Statement expression
// Invoke
static ptr<ast_expr> ast_eval_stmt_expr(ast_stmt_expr* sexpr, ast_compile_time_evaluator& evaluator, ast_eval_context& context) {
    //TODO: ...
}

ast_compile_time_evaluator::ast_compile_time_evaluator() {
    //TODO: keep at it
    this->addfunction(&ast_eval_constant<ast_integer>);
    this->addfunction(&ast_eval_constant<ast_real>);
    this->addfunction(&ast_eval_constant<ast_string>);
    this->addfunction(&ast_eval_constant<ast_array>);
    this->addfunction(&ast_eval_constant<ast_record>);

    this->addfunction(&ast_eval_cast);
    this->addfunction(&ast_eval_binary_op);
    this->addfunction(&ast_eval_unary_op);
    this->addfunction(&ast_eval_index);
    this->addfunction(&ast_eval_declref);
    this->addfunction(&ast_eval_memberref);
    this->addfunction(&ast_eval_deref);
    this->addfunction(&ast_eval_addressof);
    this->addfunction(&ast_eval_invoke);
    this->addfunction(&ast_eval_call);
    this->addfunction(&ast_eval_stmt_expr);
}


}


