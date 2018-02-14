/*
 * ast_eval.cpp
 *
 *  Created on: Jan 19, 2018
 *      Author: derick
 */

#include "ast_eval.hpp"
#include "error.hpp"

namespace xcc {

#define __box_bool(tp, v)               box<ast_expr>(new ast_integer(tp, llvm::APSInt(llvm::APInt(1, ((v) ? 1 : 0), false), false)))

// Constant values
template<typename T> static ptr<ast_expr> ast_eval_constant(T* v, ast_compile_time_evaluator&, ast_eval_context&) { return v; }

static inline llvm::APSInt __get_ivalue(ast_expr* e) {
    return llvm::APSInt(e->as<ast_integer>()->value, !(e->type->as<ast_integer_type>()->is_unsigned));
}

static inline llvm::APFloat __get_fvalue(ast_expr* e) {
    return (llvm::APFloat) e->as<ast_real>()->value;
}

static inline bool __get_bvalue(ast_expr* e) {
    return __get_ivalue(e).getBoolValue();
}

// Cast helpers
static ptr<ast_expr> ast_eval_cast_bitcast(ast_type* tp, ast_expr* expr) {
    //TODO: some assertions...
    switch(expr->get_tree_type()) {
    case tree_type_id::ast_integer:     return box<ast_expr>(new ast_integer(tp, __get_ivalue(expr)));
    case tree_type_id::ast_real:        return box<ast_expr>(new ast_real   (tp, __get_fvalue(expr)));
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
    auto value      =                __get_ivalue(expr);
    auto new_width  = (uint32_t)     tp->as<ast_integer_type>()->bitwidth;

    return box<ast_expr>(new ast_integer(tp, value.trunc(new_width)));
}

static ptr<ast_expr> ast_eval_cast_zext(ast_type* tp, ast_expr* expr) {
    //TODO: some assertions...
    auto value      =                __get_ivalue(expr);
    auto new_width  = (uint32_t)     tp->as<ast_integer_type>()->bitwidth;

    return box<ast_expr>(new ast_integer(tp, value.zext(new_width)));
}

static ptr<ast_expr> ast_eval_cast_sext(ast_type* tp, ast_expr* expr) {
    //TODO: some assertions...
    auto value      =                __get_ivalue(expr);
    auto new_width  = (uint32_t)     tp->as<ast_integer_type>()->bitwidth;

    return box<ast_expr>(new ast_integer(tp, value.sext(new_width)));
}

static const llvm::fltSemantics& __get_semantics(uint32_t bitwidth) {
    switch(bitwidth) {
    case 16:    return llvm::APFloat::IEEEhalf();
    case 32:    return llvm::APFloat::IEEEsingle();
    case 64:    return llvm::APFloat::IEEEdouble();
    case 128:   return llvm::APFloat::IEEEquad();
    default:
        //TODO:
        assert(false);
        return llvm::APFloat::Bogus();
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
    auto value      =                __get_ivalue(expr);
    auto bitwidth   = (uint32_t)     tp->as<ast_real_type>()->bitwidth;

    auto nvalue     = llvm::APFloat(__get_semantics(bitwidth));
    nvalue.convertFromAPInt(value, !is_unsigned, llvm::APFloat::roundingMode::rmTowardZero);
    return box<ast_expr>(new ast_real(tp, nvalue));
}

static ptr<ast_expr> ast_eval_cast_ftoi(ast_type* tp, ast_expr* expr, bool is_unsigned) {
    //TODO: some assertions...
    auto value      =               __get_fvalue(expr);
    auto bitwidth   = (uint32_t)    tp->as<ast_integer_type>()->bitwidth;
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
        __throw_unhandled_operator(__FILE__, __LINE__, (ast_op)cexpr->op, "ast_eval_cast()");
    }
}


static ptr<ast_expr> ast_eval_binary_op_land(ast_type* tp, ast_expr* lhs, ast_expr* rhs, ast_compile_time_evaluator& evaluator, ast_eval_context& context) {
    auto lvalue = __get_bvalue(evaluator.visit(lhs, evaluator, context));
    if(lvalue) {
        auto rvalue = __get_bvalue(evaluator.visit(rhs, evaluator, context));
        return __box_bool(tp, rvalue);
    }
    return __box_bool(tp, false);
}

static ptr<ast_expr> ast_eval_binary_op_lor(ast_type* tp, ast_expr* lhs, ast_expr* rhs, ast_compile_time_evaluator& evaluator, ast_eval_context& context) {
    auto lvalue = __get_bvalue(evaluator.visit(lhs, evaluator, context));
    if(!lvalue) {
        auto rvalue = __get_bvalue(evaluator.visit(rhs, evaluator, context));
        return __box_bool(tp, rvalue);
    }
    return __box_bool(tp, true);
}

// Binary operators
static ptr<ast_expr> ast_eval_binary_op(ast_binary_op* opexpr, ast_compile_time_evaluator& evaluator, ast_eval_context& context) {
    if      (opexpr->op == ast_op::land)    { return ast_eval_binary_op_land(opexpr->type, opexpr->lhs, opexpr->rhs, evaluator, context); }
    else if (opexpr->op == ast_op::lor)     { return ast_eval_binary_op_lor(opexpr->type, opexpr->lhs, opexpr->rhs, evaluator, context); }

#define __do_iop(l, op, r)              (__get_ivalue(l) op __get_ivalue(r))
#define __do_imt(l, mt, r)              (__get_ivalue(l).mt(__get_ivalue(r)))
#define __do_fop(l, op, r)              (__get_fvalue(l) op __get_fvalue(r))
#define __do_fmt(l, mt, r)              (__get_fvalue(l).mt(__get_fvalue(r)))
#define __box_iop(tp, l, op, r)         box<ast_expr>(new ast_integer(tp, __do_iop(l, op, r)))
#define __box_imt(tp, l, mt, r)         box<ast_expr>(new ast_integer(tp, __do_imt(l, mt, r)))
#define __box_fop(tp, l, op, r)         box<ast_expr>(new ast_real(tp, __do_fop(l, op, r)))
#define __box_fmt(tp, l, mt, r)         box<ast_expr>(new ast_real(tp, __do_fmt(l, mt, r)))

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
        // Logical binary operators
    default:
        __throw_unhandled_operator(__FILE__, __LINE__, opexpr->op, "ast_eval_binary_op()");
    }

#undef __do_iop
#undef __do_imt
#undef __do_fop
#undef __do_fmt
#undef __box_iop
#undef __box_imt
#undef __box_fop
#undef __box_fmt
}

static inline ptr<ast_expr> __get_ineg(ast_type* tp, ast_expr* e) {
    return box<ast_expr>(new ast_integer(tp, -__get_ivalue(e)));
}

static inline ptr<ast_expr> __get_fneg(ast_type* tp, ast_expr* e) {
    return box<ast_expr>(new ast_real(tp, llvm::APFloat(__get_semantics((uint32_t) tp->as<ast_real_type>()->bitwidth)) - __get_fvalue(e)));
}

static inline ptr<ast_expr> __get_lnot(ast_type* tp, ast_expr* e) {
    return __box_bool(tp, !__get_bvalue(e));
}

static inline ptr<ast_expr> __get_bnot(ast_type* tp, ast_expr* e) {
    return box<ast_expr>(new ast_integer(tp, ~__get_ivalue(e)));
}

// Unary op
static ptr<ast_expr> ast_eval_unary_op(ast_unary_op* opexpr, ast_compile_time_evaluator& evaluator, ast_eval_context& context) {
    auto expr = evaluator.visit(opexpr->expr, evaluator, context);

    switch((ast_op)opexpr->op) {
    case ast_op::ineg:                  return __get_ineg(opexpr->type, expr);
    case ast_op::fneg:                  return __get_fneg(opexpr->type, expr);
    case ast_op::lnot:                  return __get_lnot(opexpr->type, expr);
    case ast_op::bnot:                  return __get_bnot(opexpr->type, expr);
    default:
        __throw_unhandled_operator(__FILE__, __LINE__, (ast_op)opexpr->op, "ast_eval_unary_op()");
    }
}

// Index
static ptr<ast_expr> ast_eval_index(ast_index* iexpr, ast_compile_time_evaluator& evaluator, ast_eval_context& context) {
    auto arr_expr = evaluator.visit(iexpr->arr_expr,   evaluator, context)->as<ast_array>();
    auto idx_expr = evaluator.visit(iexpr->index_expr, evaluator, context)->as<ast_integer>();

    return box<ast_expr>(arr_expr->values[__get_ivalue(idx_expr).getZExtValue()]);
}

// Declref
static ptr<ast_expr> ast_eval_declref(ast_declref* dexpr, ast_compile_time_evaluator& evaluator, ast_eval_context& context) {
    return context.get(dexpr->declaration);
}

// Memberref
static ptr<ast_expr> ast_eval_memberref(ast_memberref* mexpr, ast_compile_time_evaluator& evaluator, ast_eval_context& context) {
    auto rexpr = evaluator.visit(mexpr->objexpr, evaluator, context)->as<ast_record>();
    return rexpr->values[mexpr->member_index];
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
    auto args = map(cexpr->arguments, std::function<ast_expr*(ast_expr*)>([&](ast_expr* e) mutable ->ast_expr* {
        return evaluator.visit(e, evaluator, context);
    }));
    return evaluator.call(cexpr->funcdecl->as<ast_function_decl>(), args, context);
}

// Statement expression
static ptr<ast_expr> ast_eval_stmt_expr(ast_stmt_expr* sexpr, ast_compile_time_evaluator& evaluator, ast_eval_context& context) {
    auto v = evaluator.visit(sexpr->temp->value, evaluator, context);
    context.bind((ast_decl*) sexpr->temp, evaluator.visit(sexpr->temp->value, evaluator, context));
    evaluator.eval_stmt(sexpr->statement, context);
    //TODO: maybe check for return / continue / break?
    return context.get((ast_decl*) sexpr->temp);
}

static ptr<ast_expr> ast_eval_nop_stmt(ast_nop_stmt*, ast_compile_time_evaluator& evaluator, ast_eval_context& context) {
    // do nothing
    return box<ast_expr>(nullptr);
}

static ptr<ast_expr> ast_eval_expr_stmt(ast_expr_stmt* estmt, ast_compile_time_evaluator& evaluator, ast_eval_context& context) {
    return evaluator.visit(estmt->expr, evaluator, context);
}

static ptr<ast_expr> ast_eval_assign_stmt(ast_assign_stmt* astmt, ast_compile_time_evaluator& evaluator, ast_eval_context& context) {
    //TODO: ...
    auto valexpr = evaluator.visit(astmt->rhs, evaluator, context);
    auto objexpr = evaluator.visit(astmt->lhs, evaluator, context);

    switch(objexpr->get_tree_type()) {
    default:
        __throw_unsuported_tree_type(__FILE__, __LINE__, objexpr, "ast_eval_assign_stmt()");
    }
}

static ptr<ast_expr> ast_eval_decl_stmt(ast_decl_stmt* dstmt, ast_compile_time_evaluator& evaluator, ast_eval_context& context) {
    //TODO: ...
    __throw_unsuported_tree_type(__FILE__, __LINE__, dstmt, "ast_eval_decl_stmt()");
}

static ptr<ast_expr> ast_eval_block_stmt(ast_block_stmt* bstmt, ast_compile_time_evaluator& evaluator, ast_eval_context& context) {
    ast_eval_context block_ctx = context.push();
    for(auto d: bstmt->decls) {
        block_ctx.bind(d, ptr<ast_expr>(nullptr));
    }
    evaluator.eval_stmt(bstmt->stmts, block_ctx);

    // Handle break and continue
    if(block_ctx.is_break)      { context.is_break = true; }
    if(block_ctx.is_continue)   { context.is_continue = true; }

    // Handle return
    if(block_ctx.is_return) {
        context.is_return       = block_ctx.is_return;
        context.return_value    = block_ctx.return_value;
    }

    return box<ast_expr>(nullptr);
}

static ptr<ast_expr> ast_eval_if_stmt(ast_if_stmt* istmt, ast_compile_time_evaluator& evaluator, ast_eval_context& context) {
    auto ictx = context.push();
    auto cond = __get_bvalue(evaluator.visit(istmt->condition, evaluator, ictx));
    if(cond) {
        evaluator.eval_stmt(istmt->true_stmt, ictx);
    }
    else {
        evaluator.eval_stmt(istmt->false_stmt, ictx);
    }

    // Handle break and continue
    if(ictx.is_break)           { context.is_break = true; }
    if(ictx.is_continue)        { context.is_continue = true; }

    // Handle return
    if(ictx.is_return) {
        context.is_return       = ictx.is_return;
        context.return_value    = ictx.return_value;
    }

    return box<ast_expr>(nullptr);
}

static ptr<ast_expr> ast_eval_while_stmt(ast_while_stmt* wstmt, ast_compile_time_evaluator& evaluator, ast_eval_context& context) {
    auto wctx = context.push();
    while(__get_bvalue(evaluator.visit(wstmt->condition, evaluator, wctx))) {
        evaluator.eval_stmt(wstmt->stmt, wctx);

        if(wctx.is_break || wctx.is_return) { break; }
        wctx.is_continue = false;
    }

    // Handle return
    if(wctx.is_return) {
        context.is_return       = wctx.is_return;
        context.return_value    = wctx.return_value;
    }

    return box<ast_expr>(nullptr);
}

static ptr<ast_expr> ast_eval_for_stmt(ast_for_stmt* fstmt, ast_compile_time_evaluator& evaluator, ast_eval_context& context) {
    auto fctx = context.push();
    evaluator.eval_stmt(fstmt->init_stmt, fctx);
    while(__get_bvalue(evaluator.visit(fstmt->condition, evaluator, fctx))) {
        evaluator.eval_stmt(fstmt->body,      fctx);

        if(fctx.is_break || fctx.is_return) { break; }
        fctx.is_continue = false;

        evaluator.eval_stmt(fstmt->each_stmt, fctx);
    }

    // Handle return
    if(fctx.is_return) {
        context.is_return       = fctx.is_return;
        context.return_value    = fctx.return_value;
    }

    return box<ast_expr>(nullptr);
}

static ptr<ast_expr> ast_eval_return_stmt(ast_return_stmt* rstmt, ast_compile_time_evaluator& evaluator, ast_eval_context& context) {
    context.is_return = true;
    if(rstmt->expr != nullptr) {
        context.return_value = evaluator.visit(rstmt->expr, evaluator, context);
    }

    return box<ast_expr>(nullptr);
}

static ptr<ast_expr> ast_eval_break_stmt(ast_break_stmt*, ast_compile_time_evaluator&, ast_eval_context& ctx) {
    ctx.is_break = true;
    return box<ast_expr>(nullptr);
}

static ptr<ast_expr> ast_eval_continue_stmt(ast_continue_stmt*, ast_compile_time_evaluator&, ast_eval_context& ctx) {
    ctx.is_continue = true;
    return box<ast_expr>(nullptr);
}

#undef __box_bool

ast_compile_time_evaluator::ast_compile_time_evaluator() noexcept {
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

    this->addfunction(&ast_eval_nop_stmt);
    this->addfunction(&ast_eval_expr_stmt);
    this->addfunction(&ast_eval_assign_stmt);
    this->addfunction(&ast_eval_decl_stmt);
    this->addfunction(&ast_eval_block_stmt);
    this->addfunction(&ast_eval_if_stmt);
    this->addfunction(&ast_eval_while_stmt);
    this->addfunction(&ast_eval_for_stmt);
    this->addfunction(&ast_eval_return_stmt);
    this->addfunction(&ast_eval_break_stmt);
    this->addfunction(&ast_eval_continue_stmt);
}

void ast_eval_context::bind(ast_decl* decl, ptr<ast_expr> e) noexcept {
    this->_bindings[decl] = e;
}

ptr<ast_expr> ast_eval_context::get(ast_decl* decl) noexcept {
    if(this->_bindings.find(decl) == this->_bindings.end()) {
        return this->_prev->get(decl);
    }
    return this->_bindings[decl];
}

void ast_eval_context::set(ast_decl* decl, ptr<ast_expr> e) noexcept {
    if(this->_bindings.find(decl) == this->_bindings.end()) {
        this->_prev->set(decl, e);
    }
    this->_bindings[decl] = e;
}

ast_eval_context ast_eval_context::push() noexcept {
    return ast_eval_context(this, this->_ast_builder);
}

ptr<ast_expr> ast_compile_time_evaluator::call(ast_function_decl* fdecl, list<ast_expr>* args, ast_eval_context& context) noexcept {
    auto arg_iter   = begin(*args);
    auto prm_iter   = begin(fdecl->parameters);

    ast_eval_context fcontext = context.push();

    auto arg_end    = end(*args);
    while(arg_iter != arg_end) {
        fcontext.bind(*prm_iter, *arg_iter);
        arg_iter++;
        prm_iter++;
    }

    this->eval_stmt(fdecl->body, fcontext);
    return fcontext.return_value;
}

ptr<ast_expr> ast_compile_time_evaluator::eval_stmt(ast_stmt* s, ast_eval_context& context) noexcept {
    this->visit(s, *this, context);

    return box<ast_expr>(nullptr);
}

ptr<ast_expr> ast_compile_time_evaluator::eval_stmt(list<ast_stmt>* slist, ast_eval_context& context) noexcept {
    for(auto iter = slist->begin(); iter != slist->end(); iter++) {
        this->visit(*iter, *this, context);
        if(context.is_break || context.is_continue || context.is_return) {
            break;
        }
    }

    return box<ast_expr>(nullptr);
}


}


