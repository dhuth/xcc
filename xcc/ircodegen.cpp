/*
 * ircodegen.cpp
 *
 *  Created on: Jan 26, 2017
 *      Author: derick
 */

#include <cassert>
#include "ircodegen.hpp"

#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>

namespace xcc {

llvm::Type* ircode_type_generator::generate_void_type(ast_void_type*) {
    return llvm::Type::getVoidTy(context.llvm_context);
}

llvm::Type* ircode_type_generator::generate_integer_type(ast_integer_type* itype) {
    return llvm::IntegerType::get(context.llvm_context, itype->bitwidth);
}

llvm::Type* ircode_type_generator::generate_real_type(ast_real_type* rtype) {
    switch((uint32_t) rtype->bitwidth) {
    case 16:
        return llvm::Type::getHalfTy(context.llvm_context);
    case 32:
        return llvm::Type::getFloatTy(context.llvm_context);
    case 64:
        return llvm::Type::getDoubleTy(context.llvm_context);
    case 80:
        return llvm::Type::getX86_FP80Ty(context.llvm_context);
    case 128:
        return llvm::Type::getFP128Ty(context.llvm_context);
    }
    //TODO: error
    return nullptr;
}

llvm::Type* ircode_type_generator::generate_pointer_type(ast_pointer_type* ptype) {
    auto eltype = this->visit(ptype->element_type);
    return llvm::PointerType::get(eltype, 0);
}

llvm::Type* ircode_type_generator::generate_array_type(ast_array_type* atype) {
    auto eltype = this->visit(atype->element_type);
    return llvm::ArrayType::get(eltype, (uint64_t) atype->size);
}

llvm::Type* ircode_type_generator::generate_function_type(ast_function_type* ftype) {
    auto rtype = this->visit(ftype->return_type);
    std::vector<llvm::Type*> params;
    for(auto p: ftype->parameter_types) {
        params.push_back(this->visit(p));
    }
    return llvm::FunctionType::get(rtype, llvm::ArrayRef<llvm::Type*>(params), false);
}

llvm::Type* ircode_type_generator::generate_record_type(ast_record_type* rtype) {
    ast_record_decl* rdecl = rtype->declaration;
    std::vector<llvm::Type*> types;
    for(auto m : rdecl->members) {
        types.push_back(this->visit(m->type));
    }
    return llvm::StructType::get(context.llvm_context, llvm::ArrayRef<llvm::Type*>(types));
}

llvm::Value* ircode_expr_generator::generate_integer(ast_integer* iexpr) {
    return llvm::ConstantInt::get(context.generate_type(iexpr->type), iexpr->value);
}

llvm::Value* ircode_expr_generator::generate_real(ast_real* fexpr) {
    auto ftype      = context.generate_type(fexpr->type);
    auto fvalue     = llvm::ConstantFP::get(context.llvm_context, fexpr->value);
    return            context.ir_builder.CreateFPCast(fvalue, ftype);
}

llvm::Value* ircode_expr_generator::generate_cast(ast_cast* cexpr) {
    switch((ast_op) cexpr->op) {
    case ast_op::bitcast:
        return context.ir_builder.CreateBitCast(this->visit(cexpr->expr), context.generate_type(cexpr->type));
    case ast_op::trunc:
        return context.ir_builder.CreateTrunc(this->visit(cexpr->expr), context.generate_type(cexpr->type));
    case ast_op::zext:
        return context.ir_builder.CreateZExt(this->visit(cexpr->expr), context.generate_type(cexpr->type));
    case ast_op::sext:
        return context.ir_builder.CreateSExt(this->visit(cexpr->expr), context.generate_type(cexpr->type));
    case ast_op::ftrunc:
        return context.ir_builder.CreateFPTrunc(this->visit(cexpr->expr), context.generate_type(cexpr->type));
    case ast_op::fext:
        return context.ir_builder.CreateFPExt(this->visit(cexpr->expr), context.generate_type(cexpr->type));
    case ast_op::ftoi:
        return context.ir_builder.CreateFPToSI(this->visit(cexpr->expr), context.generate_type(cexpr->type));
    case ast_op::ftou:
        return context.ir_builder.CreateFPToUI(this->visit(cexpr->expr), context.generate_type(cexpr->type));
    case ast_op::utof:
        return context.ir_builder.CreateUIToFP(this->visit(cexpr->expr), context.generate_type(cexpr->type));
    case ast_op::itof:
        return context.ir_builder.CreateSIToFP(this->visit(cexpr->expr), context.generate_type(cexpr->type));
    case ast_op::utop:
        return context.ir_builder.CreateIntToPtr(this->visit(cexpr->expr), context.generate_type(cexpr->type));
    case ast_op::ptou:
        return context.ir_builder.CreatePtrToInt(this->visit(cexpr->expr), context.generate_type(cexpr->type));
    default:
        //TODO: unhandled error
        break;
    }
    //TODO: error
    return nullptr;
}

llvm::Value* ircode_expr_generator::generate_binary_op(ast_binary_op* expr) {
    auto type = context.generate_type(expr->type);
    auto lexpr = this->visit(expr->lhs);
    auto rexpr = this->visit(expr->rhs);
    switch((ast_op) expr->op) {

    case ast_op::iadd:          return context.ir_builder.CreateAdd(lexpr, rexpr);
    case ast_op::fadd:          return context.ir_builder.CreateFAdd(lexpr, rexpr);
    case ast_op::isub:          return context.ir_builder.CreateSub(lexpr, rexpr);
    case ast_op::fsub:          return context.ir_builder.CreateFSub(lexpr, rexpr);
    case ast_op::imul:          return context.ir_builder.CreateMul(lexpr, rexpr);
    case ast_op::fmul:          return context.ir_builder.CreateFMul(lexpr, rexpr);
    case ast_op::idiv:          return context.ir_builder.CreateSDiv(lexpr, rexpr);
    case ast_op::udiv:          return context.ir_builder.CreateUDiv(lexpr, rexpr);
    case ast_op::fdiv:          return context.ir_builder.CreateFDiv(lexpr, rexpr);
    case ast_op::umod:          return context.ir_builder.CreateURem(lexpr, rexpr);
    case ast_op::imod:          return context.ir_builder.CreateSRem(lexpr, rexpr);
    case ast_op::fmod:          return context.ir_builder.CreateFRem(lexpr, rexpr);

    case ast_op::land:          return context.ir_builder.CreateAnd(lexpr, rexpr);
    case ast_op::lor:           return context.ir_builder.CreateOr(lexpr, rexpr);
    case ast_op::band:          return context.ir_builder.CreateAnd(lexpr, rexpr);
    case ast_op::bor:           return context.ir_builder.CreateOr(lexpr, rexpr);
    case ast_op::bxor:          return context.ir_builder.CreateXor(lexpr, rexpr);
    case ast_op::bshl:          return context.ir_builder.CreateShl(lexpr, rexpr);
    case ast_op::bshr:          return context.ir_builder.CreateLShr(lexpr, rexpr);
    case ast_op::ashr:          return context.ir_builder.CreateAShr(lexpr, rexpr);

    case ast_op::cmp_eq:        return context.ir_builder.CreateICmpEQ(lexpr, rexpr);
    case ast_op::cmp_ne:        return context.ir_builder.CreateICmpNE(lexpr, rexpr);
    case ast_op::icmp_ult:      return context.ir_builder.CreateICmpULT(lexpr, rexpr);
    case ast_op::icmp_ule:      return context.ir_builder.CreateICmpULE(lexpr, rexpr);
    case ast_op::icmp_ugt:      return context.ir_builder.CreateICmpUGT(lexpr, rexpr);
    case ast_op::icmp_uge:      return context.ir_builder.CreateICmpUGE(lexpr, rexpr);
    case ast_op::icmp_slt:      return context.ir_builder.CreateICmpSLT(lexpr, rexpr);
    case ast_op::icmp_sle:      return context.ir_builder.CreateICmpSLE(lexpr, rexpr);
    case ast_op::icmp_sgt:      return context.ir_builder.CreateICmpSGT(lexpr, rexpr);
    case ast_op::icmp_sge:      return context.ir_builder.CreateICmpSGE(lexpr, rexpr);
    case ast_op::fcmp_oeq:      return context.ir_builder.CreateFCmpOEQ(lexpr, rexpr);
    case ast_op::fcmp_one:      return context.ir_builder.CreateFCmpONE(lexpr, rexpr);
    case ast_op::fcmp_olt:      return context.ir_builder.CreateFCmpOLT(lexpr, rexpr);
    case ast_op::fcmp_ole:      return context.ir_builder.CreateFCmpOLE(lexpr, rexpr);
    case ast_op::fcmp_ogt:      return context.ir_builder.CreateFCmpOGT(lexpr, rexpr);
    case ast_op::fcmp_oge:      return context.ir_builder.CreateFCmpOGE(lexpr, rexpr);
    case ast_op::fcmp_ueq:      return context.ir_builder.CreateFCmpUEQ(lexpr, rexpr);
    case ast_op::fcmp_une:      return context.ir_builder.CreateFCmpUNE(lexpr, rexpr);
    case ast_op::fcmp_ult:      return context.ir_builder.CreateFCmpULT(lexpr, rexpr);
    case ast_op::fcmp_ule:      return context.ir_builder.CreateFCmpULE(lexpr, rexpr);
    case ast_op::fcmp_ugt:      return context.ir_builder.CreateFCmpUGT(lexpr, rexpr);
    case ast_op::fcmp_uge:      return context.ir_builder.CreateFCmpUGE(lexpr, rexpr);
    default:
        //TODO: error unandled
        break;
    }
    //TODO: error
    return nullptr;
}

llvm::Value* ircode_expr_generator::generate_unary_op(ast_unary_op* b) {
    auto type = context.generate_type(b->type);
    auto expr = this->visit(b->expr);

    switch((ast_op) b->op) {
    case ast_op::ineg:          return context.ir_builder.CreateNeg(expr);
    case ast_op::fneg:          return context.ir_builder.CreateFNeg(expr);
    case ast_op::bnot:          return context.ir_builder.CreateNot(expr);
    case ast_op::lnot:          return context.ir_builder.CreateNot(expr);
    default:
        //TODO: error unhandled
        break;
    }
    //TODO: error
    return nullptr;
}

llvm::Value* ircode_expr_generator::generate_index(ast_index* e) {
    auto type = context.generate_type(e->type);
    auto arr_expr = this->visit(e->arr_expr);
    auto idx_expr = this->visit(e->index_expr);

    return context.ir_builder.CreateExtractElement(arr_expr, idx_expr);
}

llvm::Value* ircode_expr_generator::generate_declref(ast_declref* e) {
    return context.ir_builder.CreateLoad(context.find(e->declaration));
}

llvm::Value* ircode_expr_generator::generate_deref(ast_deref* e) {
    return context.ir_builder.CreateLoad(context.generate_type(e->type), this->visit(e->expr));
}

llvm::Value* ircode_expr_generator::generate_memberref(ast_memberref* e) {
    auto obj = this->visit(e->objexpr);
    std::vector<uint32_t> idxs = { (uint32_t) e->member->member_index };
    return context.ir_builder.CreateExtractValue(obj, llvm::ArrayRef<uint32_t>(idxs));
}

llvm::Value* ircode_expr_generator::generate_addressof(ast_addressof* e) {
    switch(e->expr->get_tree_type()) {
    case tree_type_id::ast_declref:
        return context.find(e->expr->as<ast_declref>()->declaration);
    case tree_type_id::ast_index:
        return context.ir_builder.CreateGEP(context.generate_type(e->type), this->visit(e->as<ast_index>()->arr_expr), this->visit(e->as<ast_index>()->index_expr));
    default:
        throw std::runtime_error(std::string("unhandled ") + std::string(e->expr->get_tree_type_name()) + std::string(" in ircode_expr_generator::generate_addressof"));
    }
}

llvm::Value* ircode_expr_generator::generate_invoke(ast_invoke* e) {
    auto func = this->visit(e->funcexpr);
    std::vector<llvm::Value*> args;
    for(auto a: e->arguments) {
        args.push_back(this->visit(a));
    }
    return context.ir_builder.CreateCall(func, llvm::ArrayRef<llvm::Value*>(args));
}

void ircode_context::generate(translation_unit& tu, const char* outfile) {
    for(auto vdecl: tu.global_variable_declarations) {
        this->generate_variable_decl(unbox(vdecl));
    }

    for(auto fdecl: tu.global_function_declarations) {
        this->generate_function_decl(unbox(fdecl));
    }

    for(auto fdecl: tu.global_function_declarations) {
        this->generate_function_body(unbox(fdecl));
    }

    std::error_code ec;
    llvm::raw_fd_ostream outstream(outfile, ec, llvm::sys::fs::F_None);

    this->module->print(outstream, nullptr);

}

void ircode_context::generate_decl(ast_decl* decl) {
    switch(decl->get_tree_type()) {
    case tree_type_id::ast_variable_decl:   this->generate_variable_decl(decl->as<ast_variable_decl>());    return;
    case tree_type_id::ast_function_decl:   this->generate_function_decl(decl->as<ast_function_decl>());    return;
    case tree_type_id::ast_local_decl:      this->generate_local_decl(decl->as<ast_local_decl>());          return;
    default:
        throw std::runtime_error(std::string("unhandled ") + std::string(decl->get_tree_type_name()) + std::string(" in ircode_context::generate_decl"));
    }
}

void ircode_context::generate_variable_decl(ast_variable_decl* var) {
    auto type               = this->generate_type(var->type);
    llvm::GlobalVariable*   globalvar;
    if(var->is_extern_visible) {
        globalvar = new llvm::GlobalVariable(
                *this->module.get(),
                type,
                false,
                llvm::GlobalVariable::LinkageTypes::ExternalLinkage,
                static_cast<llvm::Constant*>(this->generate_expr(var->initial_value)),
                (std::string) var->name);
    }
    else {
        globalvar = new llvm::GlobalVariable(
                *this->module.get(),
                type,
                false,
                llvm::GlobalVariable::LinkageTypes::InternalLinkage,
                static_cast<llvm::Constant*>(this->generate_expr(var->initial_value)),
                (std::string) var->name);
    }
    this->add_declaration(var, globalvar);
}

void ircode_context::generate_function_decl(ast_function_decl* func) {
    auto rtype = this->generate_type(func->return_type);
    std::vector<llvm::Type*>        param_types;
    std::vector<llvm::Twine>        param_names;
    for(auto p: func->parameters) {
        param_types.push_back(this->generate_type(p->type));
        param_names.push_back(llvm::Twine((std::string) p->name));
    }

    llvm::Function* fvalue;
    auto ftype = llvm::FunctionType::get(rtype, llvm::ArrayRef<llvm::Type*>(param_types), false);
    if(func->is_extern_visible) {
        fvalue = llvm::Function::Create(
                ftype,
                llvm::Function::ExternalLinkage,
                llvm::Twine((std::string) func->name),
                this->module.get());
    }
    else {
       fvalue = llvm::Function::Create(
               ftype,
               llvm::GlobalValue::LinkageTypes::InternalLinkage,
               llvm::Twine((std::string) func->name),
               this->module.get());
    }
    uint32_t i = 0;
    for(auto& arg : fvalue->args()) {
        arg.setName(param_names[i]);
        i++;
    }

    this->add_declaration(func, fvalue);
}

void ircode_context::generate_local_decl(ast_local_decl* decl) {
    auto bb = this->ir_builder.GetInsertBlock();
    this->ir_builder.SetInsertPoint(this->_header_bb);

    auto type = this->generate_type(decl->type);
    this->add_declaration(decl, this->ir_builder.CreateAlloca(type, nullptr, (std::string) decl->name));

    this->ir_builder.SetInsertPoint(bb);
}

void ircode_context::generate_function_body(ast_function_decl* decl) {
    auto fvalue = static_cast<llvm::Function*>(this->find(decl));

    this->begin_scope();

    uint32_t i = 0;
    for(auto& arg: fvalue->args()) {
        this->add_declaration(decl->parameters[i], &arg);
        i++;
    }
    this->_header_bb        = llvm::BasicBlock::Create(this->llvm_context, "header", fvalue);

    llvm::BasicBlock*   bb  = llvm::BasicBlock::Create(this->llvm_context, "body", fvalue);

    this->ir_builder.SetInsertPoint(bb);

    this->generate_stmt(decl->body, nullptr, nullptr);

    this->ir_builder.SetInsertPoint(this->_header_bb);
    this->ir_builder.CreateBr(bb);
    this->ir_builder.ClearInsertionPoint();

    this->end_scope();

    //TODO: post function generation
}

void ircode_context::generate_stmt(ast_stmt* stmt, llvm::BasicBlock* continue_target, llvm::BasicBlock* break_target) {
    //TODO: setup and cleanup ???

    switch(stmt->get_tree_type()) {
    case tree_type_id::ast_assign_stmt:     this->generate_assign_stmt(stmt->as<ast_assign_stmt>());                                    break;
    case tree_type_id::ast_block_stmt:      this->generate_block_stmt(stmt->as<ast_block_stmt>(), continue_target, break_target);       break;
    case tree_type_id::ast_break_stmt:      this->generate_break_stmt(break_target);                                                    break;
    case tree_type_id::ast_continue_stmt:   this->generate_continue_stmt(continue_target);                                              break;
    case tree_type_id::ast_expr_stmt:       this->generate_expr_stmt(stmt->as<ast_expr_stmt>());                                        break;
    case tree_type_id::ast_if_stmt:         this->generate_if_stmt(stmt->as<ast_if_stmt>(), continue_target, break_target);             break;
    case tree_type_id::ast_for_stmt:        this->generate_for_stmt(stmt->as<ast_for_stmt>(), continue_target, break_target);           break;
    case tree_type_id::ast_nop_stmt:        /* do nothing */                                                                            break;
    case tree_type_id::ast_return_stmt:     this->generate_return_stmt(stmt->as<ast_return_stmt>());                                    break;
    case tree_type_id::ast_while_stmt:      this->generate_while_stmt(stmt->as<ast_while_stmt>(), continue_target, break_target);       break;
    default:
        throw std::runtime_error(std::string("unhandled ") + std::string(stmt->get_tree_type_name()) + std::string(" in ircode_context::generate_stmt"));
    }
}

void ircode_context::generate_assign_stmt(ast_assign_stmt* stmt) {
    assert(stmt->lhs->is<ast_addressof>());

    auto val        = this->generate_expr(stmt->rhs);
    auto addr       = this->generate_expr(stmt->lhs);

    this->ir_builder.CreateStore(val, addr);
}

void ircode_context::generate_block_stmt(ast_block_stmt* stmt, llvm::BasicBlock* ct, llvm::BasicBlock* bt) {
    this->begin_scope();
    for(auto l: stmt->decls) {
        this->generate_decl(l);
    }
    for(auto s: stmt->stmts) {
        this->generate_stmt(s, ct, bt);
    }
    this->end_scope();
}

void ircode_context::generate_break_stmt(llvm::BasicBlock* bt) {
    this->ir_builder.CreateBr(bt);
}

void ircode_context::generate_continue_stmt(llvm::BasicBlock* ct) {
    this->ir_builder.CreateBr(ct);
}

void ircode_context::generate_expr_stmt(ast_expr_stmt* stmt) {
    this->generate_expr(stmt->expr);
}

void ircode_context::generate_for_stmt(ast_for_stmt* stmt, llvm::BasicBlock* ct, llvm::BasicBlock* bt) {
    //TODO: llvm loop vectorization stuff

    llvm::BasicBlock* prelogue          = llvm::BasicBlock::Create(this->llvm_context);
    llvm::BasicBlock* body              = llvm::BasicBlock::Create(this->llvm_context);
    llvm::BasicBlock* prologue          = llvm::BasicBlock::Create(this->llvm_context);
    llvm::BasicBlock* continue_block    = llvm::BasicBlock::Create(this->llvm_context);

    this->generate_stmt(stmt->init_stmt, ct, bt);
    this->ir_builder.CreateBr(prelogue);

    this->ir_builder.SetInsertPoint(prelogue);
    this->ir_builder.CreateCondBr(this->generate_expr(stmt->condition), body, continue_block);

    this->ir_builder.SetInsertPoint(body);
    this->generate_stmt(stmt->body, prologue, continue_block);
    if(!this->ir_builder.GetInsertBlock()->getTerminator()) {
        this->ir_builder.CreateBr(prologue);
    }

    this->ir_builder.SetInsertPoint(prologue);
    this->generate_stmt(stmt->each_stmt, ct, bt);
    this->ir_builder.CreateBr(prelogue);

    this->ir_builder.SetInsertPoint(continue_block);
}

void ircode_context::generate_if_stmt(ast_if_stmt* stmt, llvm::BasicBlock* ct, llvm::BasicBlock* bt) {
    llvm::BasicBlock* true_block        = llvm::BasicBlock::Create(this->llvm_context);
    llvm::BasicBlock* false_block       = llvm::BasicBlock::Create(this->llvm_context);
    llvm::BasicBlock* continue_block    = llvm::BasicBlock::Create(this->llvm_context);

    this->ir_builder.CreateCondBr(this->generate_expr(stmt->condition), true_block, false_block);

    this->ir_builder.SetInsertPoint(true_block);
    this->generate_stmt(stmt->true_stmt, ct, bt);
    if(!this->ir_builder.GetInsertBlock()->getTerminator()) {
        this->ir_builder.CreateBr(continue_block);
    }

    this->ir_builder.SetInsertPoint(false_block);
    this->generate_stmt(stmt->false_stmt, ct, bt);
    if(!this->ir_builder.GetInsertBlock()->getTerminator()) {
        this->ir_builder.CreateBr(continue_block);
    }

    this->ir_builder.SetInsertPoint(continue_block);
}

void ircode_context::generate_return_stmt(ast_return_stmt* ret) {
    if(ret->expr != nullptr) {
        this->ir_builder.CreateRet(this->generate_expr(ret->expr));
    }
    else {
        this->ir_builder.CreateRetVoid();
    }
}

void ircode_context::generate_while_stmt(ast_while_stmt* stmt, llvm::BasicBlock* ct, llvm::BasicBlock* bt) {
    llvm::BasicBlock*       prologue        = llvm::BasicBlock::Create(this->llvm_context);
    llvm::BasicBlock*       body            = llvm::BasicBlock::Create(this->llvm_context);
    llvm::BasicBlock*       continue_block  = llvm::BasicBlock::Create(this->llvm_context);

    this->ir_builder.CreateBr(prologue);
    this->ir_builder.SetInsertPoint(prologue);
    this->ir_builder.CreateCondBr(this->generate_expr(stmt->condition), body, continue_block);

    this->ir_builder.SetInsertPoint(body);
    this->generate_stmt(stmt->stmt, prologue, continue_block);
    if(!this->ir_builder.GetInsertBlock()->getTerminator()) {
        this->ir_builder.CreateBr(prologue);
    }

    this->ir_builder.SetInsertPoint(continue_block);
}

}
