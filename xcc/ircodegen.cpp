/*
 * ircodegen.cpp
 *
 *  Created on: Jan 26, 2017
 *      Author: derick
 */

#include <cassert>
#include "ircodegen.hpp"
#include "error.hpp"

#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/Verifier.h>
//#include <llvm/Bitcode/BitcodeWriter.h>

namespace xcc {

llvm::Type* ircode_type_generator::generate_void_type(ast_void_type*) {
    return llvm::Type::getVoidTy(context.llvm_context);
}

llvm::Type* ircode_type_generator::generate_integer_type(ast_integer_type* itype) {
    return llvm::IntegerType::get(context.llvm_context, (uint32_t) itype->bitwidth);
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
    return llvm::FunctionType::get(rtype, llvm::ArrayRef<llvm::Type*>(params), ftype->is_varargs);
}

llvm::Type* ircode_type_generator::generate_record_type(ast_record_type* rtype) {
    std::vector<llvm::Type*> types;
    for(auto m : rtype->field_types) {
        types.push_back(this->visit(m));
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

llvm::Value* ircode_expr_generator::generate_string(ast_string* sexpr) {
    auto atype      = context.generate_type(sexpr->type);
    return context.ir_builder.CreateLoad(atype, context.get_string_data(sexpr));
}

llvm::Value* ircode_expr_generator::generate_record(ast_record* rexpr) {
    std::vector<llvm::Constant*> field_values;

    for(auto expr_fval: rexpr->values) {
        auto fval = context.generate_expr(expr_fval);
        assert(llvm::isa<llvm::Constant>(fval));
        field_values.push_back(llvm::dyn_cast<llvm::Constant>(fval));
    }
    llvm::ArrayRef<llvm::Constant*> field_values_ref(field_values);

    auto rtype      = context.generate_type(rexpr->type);
    assert(llvm::isa<llvm::StructType>(rtype));

    return llvm::ConstantStruct::get((llvm::StructType*) rtype, field_values_ref);
}

llvm::Value* ircode_expr_generator::generate_array(ast_array* arrexpr) {
    std::vector<llvm::Constant*> values;

    for(auto expr_fval: arrexpr->values) {
        auto fval = context.generate_expr(expr_fval);
        assert(llvm::isa<llvm::Constant>(fval));
        values.push_back(llvm::dyn_cast<llvm::Constant>(fval));
    }
    llvm::ArrayRef<llvm::Constant*> values_ref(values);

    auto atype      = context.generate_type(arrexpr->type);
    assert(llvm::isa<llvm::ArrayType>(atype));

    return llvm::ConstantArray::get((llvm::ArrayType*) atype, values_ref);
}

llvm::Value* ircode_expr_generator::generate_cast(ast_cast* cexpr) {
    auto expr = this->visit(cexpr->expr);
    auto type = context.generate_type(cexpr->type);

    switch((ast_op) cexpr->op) {
    case ast_op::bitcast:                           return context.ir_builder.CreateZExtOrTrunc(expr, type);
    case ast_op::trunc:                             return context.ir_builder.CreateTrunc(expr, type);
    case ast_op::zext:                              return context.ir_builder.CreateZExt(expr, type);
    case ast_op::sext:                              return context.ir_builder.CreateSExt(expr, type);
    case ast_op::ftrunc:                            return context.ir_builder.CreateFPTrunc(expr, type);
    case ast_op::fext:                              return context.ir_builder.CreateFPExt(expr, type);
    case ast_op::ftoi:                              return context.ir_builder.CreateFPToSI(expr, type);
    case ast_op::ftou:                              return context.ir_builder.CreateFPToUI(expr, type);
    case ast_op::utof:                              return context.ir_builder.CreateUIToFP(expr, type);
    case ast_op::itof:                              return context.ir_builder.CreateSIToFP(expr, type);
    case ast_op::utop:                              return context.ir_builder.CreateIntToPtr(expr, type);
    case ast_op::ptou:                              return context.ir_builder.CreatePtrToInt(expr, type);
    default:
        //TODO: unhandled error
        break;
    }
    //TODO: error
    return nullptr;
}

#define __mk_inst(mt)          llvm::dyn_cast<llvm::Value>(context.ir_builder.Create##mt(lexpr, rexpr))
llvm::Value* ircode_expr_generator::generate_binary_op(ast_binary_op* expr) {
    auto type = context.generate_type(expr->type);
    auto lexpr = this->visit(expr->lhs);
    auto rexpr = this->visit(expr->rhs);

    switch((ast_op) expr->op) {

    case ast_op::iadd:          return __mk_inst(Add);      //context.ir_builder.CreateAdd(lexpr, rexpr);
    case ast_op::fadd:          return __mk_inst(FAdd);     //context.ir_builder.CreateFAdd(lexpr, rexpr);
    case ast_op::isub:          return __mk_inst(Sub);      //context.ir_builder.CreateSub(lexpr, rexpr);
    case ast_op::fsub:          return __mk_inst(FSub);     //context.ir_builder.CreateFSub(lexpr, rexpr);
    case ast_op::imul:          return __mk_inst(Mul);      //context.ir_builder.CreateMul(lexpr, rexpr);
    case ast_op::fmul:          return __mk_inst(FMul);     //context.ir_builder.CreateFMul(lexpr, rexpr);
    case ast_op::idiv:          return __mk_inst(SDiv);     //context.ir_builder.CreateSDiv(lexpr, rexpr);
    case ast_op::udiv:          return __mk_inst(UDiv);     //context.ir_builder.CreateUDiv(lexpr, rexpr);
    case ast_op::fdiv:          return __mk_inst(FDiv);     //context.ir_builder.CreateFDiv(lexpr, rexpr);
    case ast_op::umod:          return __mk_inst(URem);     //context.ir_builder.CreateURem(lexpr, rexpr);
    case ast_op::imod:          return __mk_inst(SRem);     //context.ir_builder.CreateSRem(lexpr, rexpr);
    case ast_op::fmod:          return __mk_inst(FRem);     //context.ir_builder.CreateFRem(lexpr, rexpr);

    case ast_op::land:          return __mk_inst(And);      //context.ir_builder.CreateAnd(lexpr, rexpr);
    case ast_op::lor:           return __mk_inst(Or);       //context.ir_builder.CreateOr(lexpr, rexpr);
    case ast_op::band:          return __mk_inst(And);      //context.ir_builder.CreateAnd(lexpr, rexpr);
    case ast_op::bor:           return __mk_inst(Or);       //context.ir_builder.CreateOr(lexpr, rexpr);
    case ast_op::bxor:          return __mk_inst(Xor);      //context.ir_builder.CreateXor(lexpr, rexpr);
    case ast_op::bshl:          return __mk_inst(Shl);      //context.ir_builder.CreateShl(lexpr, rexpr);
    case ast_op::bshr:          return __mk_inst(LShr);     //context.ir_builder.CreateLShr(lexpr, rexpr);
    case ast_op::ashr:          return __mk_inst(AShr);     //context.ir_builder.CreateAShr(lexpr, rexpr);

    case ast_op::cmp_eq:        return __mk_inst(ICmpEQ);   //context.ir_builder.CreateICmpEQ(lexpr, rexpr);
    case ast_op::cmp_ne:        return __mk_inst(ICmpNE);   //context.ir_builder.CreateICmpNE(lexpr, rexpr);
    case ast_op::icmp_ult:      return __mk_inst(ICmpULT);  //context.ir_builder.CreateICmpULT(lexpr, rexpr);
    case ast_op::icmp_ule:      return __mk_inst(ICmpULE);  //context.ir_builder.CreateICmpULE(lexpr, rexpr);
    case ast_op::icmp_ugt:      return __mk_inst(ICmpUGT);  //context.ir_builder.CreateICmpUGT(lexpr, rexpr);
    case ast_op::icmp_uge:      return __mk_inst(ICmpUGE);  //context.ir_builder.CreateICmpUGE(lexpr, rexpr);
    case ast_op::icmp_slt:      return __mk_inst(ICmpSLT);  //context.ir_builder.CreateICmpSLT(lexpr, rexpr);
    case ast_op::icmp_sle:      return __mk_inst(ICmpSLE);  //context.ir_builder.CreateICmpSLE(lexpr, rexpr);
    case ast_op::icmp_sgt:      return __mk_inst(ICmpSGT);  //context.ir_builder.CreateICmpSGT(lexpr, rexpr);
    case ast_op::icmp_sge:      return __mk_inst(ICmpSGE);  //context.ir_builder.CreateICmpSGE(lexpr, rexpr);
    case ast_op::fcmp_oeq:      return __mk_inst(FCmpOEQ);  //context.ir_builder.CreateFCmpOEQ(lexpr, rexpr);
    case ast_op::fcmp_one:      return __mk_inst(FCmpONE);  //context.ir_builder.CreateFCmpONE(lexpr, rexpr);
    case ast_op::fcmp_olt:      return __mk_inst(FCmpOLT);  //context.ir_builder.CreateFCmpOLT(lexpr, rexpr);
    case ast_op::fcmp_ole:      return __mk_inst(FCmpOLE);  //context.ir_builder.CreateFCmpOLE(lexpr, rexpr);
    case ast_op::fcmp_ogt:      return __mk_inst(FCmpOGT);  //context.ir_builder.CreateFCmpOGT(lexpr, rexpr);
    case ast_op::fcmp_oge:      return __mk_inst(FCmpOGE);  //context.ir_builder.CreateFCmpOGE(lexpr, rexpr);
    case ast_op::fcmp_ueq:      return __mk_inst(FCmpUEQ);  //context.ir_builder.CreateFCmpUEQ(lexpr, rexpr);
    case ast_op::fcmp_une:      return __mk_inst(FCmpUNE);  //context.ir_builder.CreateFCmpUNE(lexpr, rexpr);
    case ast_op::fcmp_ult:      return __mk_inst(FCmpULT);  //context.ir_builder.CreateFCmpULT(lexpr, rexpr);
    case ast_op::fcmp_ule:      return __mk_inst(FCmpULE);  //context.ir_builder.CreateFCmpULE(lexpr, rexpr);
    case ast_op::fcmp_ugt:      return __mk_inst(FCmpUGT);  //context.ir_builder.CreateFCmpUGT(lexpr, rexpr);
    case ast_op::fcmp_uge:      return __mk_inst(FCmpUGE);  //context.ir_builder.CreateFCmpUGE(lexpr, rexpr);
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
    //throw std::runtime_error(__FILE__":" + std::to_string(__LINE__) + "Not implemented");
    auto addr = context.generate_address(e);
    auto type = context.generate_type(e->type);
    //auto arr_expr = this->visit(e->arr_expr);
    //auto idx_expr = this->visit(e->index_expr);

    return context.ir_builder.CreateLoad(type, addr);
}

llvm::Value* ircode_expr_generator::generate_declref(ast_declref* e) {
    ast_decl* decl = e->declaration;
    return context.ir_builder.CreateLoad(context.find(decl));
}

llvm::Value* ircode_expr_generator::generate_deref(ast_deref* e) {
    return context.ir_builder.CreateLoad(context.generate_type(e->type), this->visit(e->expr));
}

llvm::Value* ircode_expr_generator::generate_memberref(ast_memberref* e) {
    auto obj = context.generate_address(e);
    //std::vector<uint32_t> idxs = { (uint32_t) e->member_index };
    //obj->getType()->dump();
    return context.ir_builder.CreateLoad(context.generate_type(e->type), obj);
}

llvm::Value* ircode_expr_generator::generate_addressof(ast_addressof* e) {
    return context.generate_address(e->expr);
    /*switch(e->expr->get_tree_type()) {
    case tree_type_id::ast_declref:
        return context.find(e->expr->as<ast_declref>()->declaration);
    case tree_type_id::ast_index:
        return context.ir_builder.CreateGEP(context.generate_type(e->type), this->visit(e->as<ast_index>()->arr_expr), this->visit(e->as<ast_index>()->index_expr));
    default:
        throw std::runtime_error(std::string("unhandled ") + std::string(e->expr->get_tree_type_name()) + std::string(" in ircode_expr_generator::generate_addressof"));
    }*/
}

llvm::Value* ircode_expr_generator::generate_invoke(ast_invoke* e) {
    llvm::Value* func = nullptr;
    func = this->visit(e->funcexpr);

    std::vector<llvm::Value*> args;
    for(auto a: e->arguments) {
        args.push_back(this->visit(a));
    }

    return context.ir_builder.CreateCall(func, llvm::ArrayRef<llvm::Value*>(args));
}

llvm::Value* ircode_expr_generator::generate_call(ast_call* e) {
    llvm::Value* func = context.find(e->funcdecl->declaration);
    std::vector<llvm::Value*> args;
    for(auto a: e->arguments) {
        args.push_back(this->visit(a));
    }
    return context.ir_builder.CreateCall(func, llvm::ArrayRef<llvm::Value*>(args));
}

llvm::Value* ircode_expr_generator::generate_stmt_expr(ast_stmt_expr* expr) {
    ast_temp_decl* decl = expr->temp;
    context.generate_temp_decl(decl);

    context.generate_stmt(expr->statement);

    //TODO: Clean up

    return context.find((ast_decl*) expr->temp);
}


llvm::Value* ircode_address_generator::generate_declref(ast_declref* decl) {
    return context.find(decl->declaration);
}

llvm::Value* ircode_address_generator::generate_deref(ast_deref* deref) {
    return context.generate_expr(deref->expr);
}

llvm::Value* ircode_address_generator::generate_memberref(ast_memberref* memref) {
    auto objaddr = this->visit(memref->objexpr);
    auto objtype = context.generate_type(memref->objexpr->type);

    return context.ir_builder.CreateConstGEP2_32(objtype, objaddr, 0, (uint32_t) memref->member_index);
}

llvm::Value* ircode_address_generator::generate_global_string(ast_string* expr) {
    return context.get_string_data(expr);
}

llvm::Value* ircode_address_generator::generate_index(ast_index* expr) {
    auto arr        = this->visit(expr->arr_expr);
    auto idx        = context.generate_expr(expr->index_expr);

    std::vector<llvm::Value*> indecies;
    auto i64 = llvm::Type::getInt32Ty(context.llvm_context);
    indecies.push_back(llvm::ConstantInt::get(i64, 0));
    indecies.push_back(idx);

    return context.ir_builder.CreateGEP(arr, indecies);
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
                *this->module,
                type,
                false,
                llvm::GlobalVariable::LinkageTypes::ExternalLinkage,
                static_cast<llvm::Constant*>(this->generate_expr(var->initial_value)),
                (std::string) var->name);
    }
    else {
        globalvar = new llvm::GlobalVariable(
                *this->module,
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
    std::vector<std::string>        param_names;
    for(auto p: func->parameters) {
        param_types.push_back(this->generate_type(p->type));
        param_names.push_back((std::string) p->name);
    }

    llvm::Function* fvalue;
    auto ftype = llvm::FunctionType::get(rtype, llvm::ArrayRef<llvm::Type*>(param_types), func->is_varargs);
    if(func->is_extern_visible) {
        fvalue = llvm::Function::Create(
                ftype,
                llvm::Function::ExternalLinkage,
                llvm::Twine((std::string) func->generated_name),
                this->module);
    }
    else {
       fvalue = llvm::Function::Create(
               ftype,
               llvm::GlobalValue::LinkageTypes::InternalLinkage,
               llvm::Twine((std::string) func->generated_name),
               this->module);
    }
    uint32_t i = 0;
    for(auto& arg : fvalue->args()) {
        if(!param_names[i].empty()) {
            arg.setName(param_names[i]);
        }
        i++;
    }

    this->add_declaration(func, fvalue);
}

void ircode_context::generate_local_decl(ast_local_decl* decl) {
    auto bb = this->ir_builder.GetInsertBlock();
    this->ir_builder.SetInsertPoint(_header_bb);

    auto type = this->generate_type(decl->type);
    auto dval = this->ir_builder.CreateAlloca(type, nullptr, (std::string) decl->name);
    this->add_declaration(decl, dval);

    this->ir_builder.SetInsertPoint(bb);

    if(decl->init_value != nullptr) {
        auto ival = this->generate_expr(decl->init_value);
        this->ir_builder.CreateStore(ival, dval);
    }
}

void ircode_context::generate_temp_decl(ast_temp_decl* decl) {
    auto bb = this->ir_builder.GetInsertBlock();
    this->ir_builder.SetInsertPoint(_header_bb);

    auto type = this->generate_type(decl->type);
    auto dval = this->ir_builder.CreateAlloca(type);
    this->add_declaration(decl, dval);

    this->ir_builder.SetInsertPoint(bb);

    this->ir_builder.CreateStore(this->generate_expr(decl->value), dval);
}

void ircode_context::generate_function_body(ast_function_decl* decl) {
    auto fvalue = static_cast<llvm::Function*>(this->find(decl));
    if(decl->is_extern) return;

    this->begin_scope();
    this->_header_bb        = llvm::BasicBlock::Create(this->llvm_context, "header", fvalue);
    this->ir_builder.SetInsertPoint(this->_header_bb);

    uint32_t i = 0;
    for(auto& arg: fvalue->args()) {
        this->add_declaration(
                decl->parameters[i],
                this->ir_builder.CreateAlloca(this->generate_type(decl->parameters[i]->type), nullptr, arg.getName()));
        this->ir_builder.CreateStore(&arg, this->find(decl->parameters[i]));
        i++;
    }

    llvm::BasicBlock*   bb  = llvm::BasicBlock::Create(this->llvm_context, "body", fvalue);

    this->ir_builder.SetInsertPoint(bb);

    this->generate_stmt(decl->body, nullptr, nullptr);

    if((!this->ir_builder.GetInsertBlock()->getTerminator()) && decl->return_type->is<ast_void_type>()) {
        this->ir_builder.CreateRetVoid();
    }

    this->ir_builder.SetInsertPoint(this->_header_bb);
    this->ir_builder.CreateBr(bb);
    this->ir_builder.ClearInsertionPoint();

    this->end_scope();

    //TODO: post function generation
    if(llvm::verifyFunction(*fvalue, &llvm::outs())) {
        //TODO: better error handling
        //std::exit(1);
    }
}

void ircode_context::generate_stmt(ast_stmt* stmt) {
    //TODO: setup and cleanup ???

    switch(stmt->get_tree_type()) {
    case tree_type_id::ast_assign_stmt:     this->generate_assign_stmt(stmt->as<ast_assign_stmt>());    break;
    case tree_type_id::ast_expr_stmt:       this->generate_expr_stmt(stmt->as<ast_expr_stmt>());        break;
    case tree_type_id::ast_return_stmt:     this->generate_return_stmt(stmt->as<ast_return_stmt>());    break;
    default:
        __throw_unhandled_tree_type(__FILE__, __LINE__, stmt, "ircode_context::generate_stmt()");
    }
}

void ircode_context::generate_stmtlist(list<ast_stmt>* stmts) {
    for(auto stmt: *stmts) {
        this->generate_stmt(stmt, nullptr, nullptr);
    }
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
        __throw_unhandled_tree_type(__FILE__, __LINE__, stmt, "ircode_context::generate_stmt()");
    }
}

void ircode_context::generate_assign_stmt(ast_assign_stmt* stmt) {
    auto val        = this->generate_expr(stmt->rhs);
    auto addr       = this->generate_address(stmt->lhs);

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

    llvm::Function*   func              = this->ir_builder.GetInsertBlock()->getParent();

    llvm::BasicBlock* prelogue          = llvm::BasicBlock::Create(this->llvm_context);
    llvm::BasicBlock* body              = llvm::BasicBlock::Create(this->llvm_context);
    llvm::BasicBlock* epilogue          = llvm::BasicBlock::Create(this->llvm_context);
    llvm::BasicBlock* continue_block    = llvm::BasicBlock::Create(this->llvm_context);

    this->generate_stmt(stmt->init_stmt, ct, bt);
    this->ir_builder.CreateBr(prelogue);

    prelogue->insertInto(func);
    this->ir_builder.SetInsertPoint(prelogue);
    this->ir_builder.CreateCondBr(this->generate_expr(stmt->condition), body, continue_block);

    body->insertInto(func);
    this->ir_builder.SetInsertPoint(body);
    this->generate_stmt(stmt->body, epilogue, continue_block);
    if(!this->ir_builder.GetInsertBlock()->getTerminator()) {
        this->ir_builder.CreateBr(epilogue);
    }

    epilogue->insertInto(func);
    this->ir_builder.SetInsertPoint(epilogue);
    this->generate_stmt(stmt->each_stmt, ct, bt);
    this->ir_builder.CreateBr(prelogue);

    continue_block->insertInto(func);
    this->ir_builder.SetInsertPoint(continue_block);
}

void ircode_context::generate_if_stmt(ast_if_stmt* stmt, llvm::BasicBlock* ct, llvm::BasicBlock* bt) {
    llvm::Function*   func              = this->ir_builder.GetInsertBlock()->getParent();

    llvm::BasicBlock* true_block        = llvm::BasicBlock::Create(this->llvm_context);
    llvm::BasicBlock* false_block       = llvm::BasicBlock::Create(this->llvm_context);
    llvm::BasicBlock* continue_block    = llvm::BasicBlock::Create(this->llvm_context);

    this->ir_builder.CreateCondBr(this->generate_expr(stmt->condition), true_block, false_block);

    bool false_block_continues = false;
    bool true_block_continues  = false;

    true_block->insertInto(func);
    this->ir_builder.SetInsertPoint(true_block);
    this->generate_stmt(stmt->true_stmt, ct, bt);
    if(!this->ir_builder.GetInsertBlock()->getTerminator()) {
        this->ir_builder.CreateBr(continue_block);
        true_block_continues = true;
    }

    false_block->insertInto(func);
    this->ir_builder.SetInsertPoint(false_block);
    if(stmt->false_stmt != nullptr) {
        this->generate_stmt(stmt->false_stmt, ct, bt);
        if(!this->ir_builder.GetInsertBlock()->getTerminator()) {
            this->ir_builder.CreateBr(continue_block);
            false_block_continues = true;
        }
    }

    if(true_block_continues || false_block_continues) {
        continue_block->insertInto(func);
        this->ir_builder.SetInsertPoint(continue_block);
    }
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
    llvm::Function*         func            = this->ir_builder.GetInsertBlock()->getParent();

    llvm::BasicBlock*       prologue        = llvm::BasicBlock::Create(this->llvm_context);
    llvm::BasicBlock*       body            = llvm::BasicBlock::Create(this->llvm_context);
    llvm::BasicBlock*       continue_block  = llvm::BasicBlock::Create(this->llvm_context);

    this->ir_builder.CreateBr(prologue);
    prologue->insertInto(func);
    this->ir_builder.SetInsertPoint(prologue);
    this->ir_builder.CreateCondBr(this->generate_expr(stmt->condition), body, continue_block);

    body->insertInto(func);
    this->ir_builder.SetInsertPoint(body);
    this->generate_stmt(stmt->stmt, prologue, continue_block);
    if(!this->ir_builder.GetInsertBlock()->getTerminator()) {
        this->ir_builder.CreateBr(prologue);
    }

    continue_block->insertInto(func);
    this->ir_builder.SetInsertPoint(continue_block);
}

llvm::Value* ircode_context::get_string_data(ast_string* sexpr) noexcept {
    if(_string_data.find(sexpr->value) == _string_data.end()) {
        auto name   = std::string(".str.") + std::to_string(_string_data.size());
        auto value  = llvm::ConstantDataArray::getString(this->llvm_context, (std::string) sexpr->value);
        auto gvar   = new llvm::GlobalVariable(
                *this->module,
                value->getType(),
                true,
                llvm::GlobalVariable::PrivateLinkage,
                value,
                name);
        _string_data[(std::string) sexpr->value] = gvar;
        return gvar;
    }
    return _string_data[(std::string) sexpr->value];
}

}
