/*
 * codegen.hpp
 *
 *  Created on: Jan 26, 2017
 *      Author: derick
 */

#ifndef IRCODEGEN_HPP_
#define IRCODEGEN_HPP_

#include <memory>

#include "ast.hpp"
#include "frontend.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Constant.h>

namespace xcc {

struct ircode_context;

struct ircode_type_generator : public dispatch_visitor<llvm::Type*> {
public:

    inline ircode_type_generator(ircode_context& context) noexcept
            : context(context) {
        this->addmethod(&ircode_type_generator::generate_void_type);
        this->addmethod(&ircode_type_generator::generate_integer_type);
        this->addmethod(&ircode_type_generator::generate_real_type);
        this->addmethod(&ircode_type_generator::generate_pointer_type);
        this->addmethod(&ircode_type_generator::generate_array_type);
        this->addmethod(&ircode_type_generator::generate_function_type);
        this->addmethod(&ircode_type_generator::generate_record_type);
    }

    inline llvm::Type* operator()(ast_type* tp) { return this->visit(tp); }

private:

    llvm::Type*                                     generate_void_type(ast_void_type*);
    llvm::Type*                                     generate_integer_type(ast_integer_type*);
    llvm::Type*                                     generate_real_type(ast_real_type*);
    llvm::Type*                                     generate_pointer_type(ast_pointer_type*);
    llvm::Type*                                     generate_array_type(ast_array_type*);
    llvm::Type*                                     generate_function_type(ast_function_type*);
    llvm::Type*                                     generate_record_type(ast_record_type*);

    ircode_context&                                                     context;
};

struct ircode_expr_generator : public dispatch_visitor<llvm::Value*> {
public:

    inline ircode_expr_generator(ircode_context& context) noexcept
            : context(context) {
        this->addmethod(&ircode_expr_generator::generate_integer);
        this->addmethod(&ircode_expr_generator::generate_real);
        this->addmethod(&ircode_expr_generator::generate_cast);
        this->addmethod(&ircode_expr_generator::generate_binary_op);
        this->addmethod(&ircode_expr_generator::generate_unary_op);
        this->addmethod(&ircode_expr_generator::generate_index);
        this->addmethod(&ircode_expr_generator::generate_declref);
        this->addmethod(&ircode_expr_generator::generate_memberref);
        this->addmethod(&ircode_expr_generator::generate_deref);
        this->addmethod(&ircode_expr_generator::generate_addressof);
        this->addmethod(&ircode_expr_generator::generate_invoke);
    }

    inline llvm::Value* operator()(ast_expr* expr) { return this->visit(expr); }

private:

    llvm::Value*                                    generate_integer(ast_integer*);
    llvm::Value*                                    generate_real(ast_real*);
    llvm::Value*                                    generate_cast(ast_cast*);
    llvm::Value*                                    generate_binary_op(ast_binary_op*);
    llvm::Value*                                    generate_unary_op(ast_unary_op*);
    llvm::Value*                                    generate_index(ast_index*);
    llvm::Value*                                    generate_declref(ast_declref*);
    llvm::Value*                                    generate_deref(ast_deref*);
    llvm::Value*                                    generate_memberref(ast_memberref*);
    llvm::Value*                                    generate_addressof(ast_addressof*);
    llvm::Value*                                    generate_invoke(ast_invoke*);

    ircode_context&                                                     context;
};

struct ircode_context final {
public:

    inline ircode_context(std::string module_name) noexcept
        : llvm_context(),
          ir_builder(this->llvm_context),
          _local_scope(new local_scope(nullptr)),
          generate_expr(*this),
          generate_type(*this) {
        this->module = llvm::make_unique<llvm::Module>(module_name, this->llvm_context);
    }
    inline ~ircode_context() {
        delete this->_local_scope;
    }

    void generate(translation_unit& tu, const char* outfile);
    void generate_decl(ast_decl* decl);

    llvm::LLVMContext                                                   llvm_context;
    llvm::IRBuilder<>                                                   ir_builder;
    std::unique_ptr<llvm::Module>                                       module;
    ircode_expr_generator                                               generate_expr;
    ircode_type_generator                                               generate_type;

//private:

    struct local_scope {
        inline local_scope(local_scope* prev): prev(prev) { }
        ~local_scope() = default;

        local_scope*                                                    prev;
        std::map<ast_decl*, llvm::Value*>                               named_values;

        inline llvm::Value* operator[](ast_decl* decl) {
            auto itr = this->named_values.find(decl);
            if(itr == this->named_values.end()) {
                if(this->prev != nullptr) {
                    return (*prev)[decl];
                }
                //TODO: error
                return nullptr;
            }
            return itr->second;
        }
    };

    inline void begin_scope() {
        this->_local_scope = new local_scope(this->_local_scope);
    }
    inline void end_scope() {
        auto old_scope = this->_local_scope;
        this->_local_scope = old_scope->prev;
        delete old_scope;
    }
    inline void add_declaration(ast_decl* decl, llvm::Value* value) {
        this->_local_scope->named_values[decl] = value;
    }
    inline llvm::Value* find(ast_decl* decl) {
        return (*this->_local_scope)[decl];
    }

private:

    void                                                                generate_function_decl(ast_function_decl*);
    void                                                                generate_variable_decl(ast_variable_decl*);
    void                                                                generate_local_decl(ast_local_decl*);

    void                                                                generate_function_body(ast_function_decl*);
    void                                                                generate_stmt(ast_stmt*, llvm::BasicBlock* break_target, llvm::BasicBlock* continue_target);

    void                                                                generate_expr_stmt(ast_expr_stmt*);
    void                                                                generate_assign_stmt(ast_assign_stmt*);
    void                                                                generate_block_stmt(ast_block_stmt*, llvm::BasicBlock*, llvm::BasicBlock*);
    void                                                                generate_if_stmt(ast_if_stmt*, llvm::BasicBlock*, llvm::BasicBlock*);
    void                                                                generate_while_stmt(ast_while_stmt*, llvm::BasicBlock*, llvm::BasicBlock*);
    void                                                                generate_for_stmt(ast_for_stmt*, llvm::BasicBlock*, llvm::BasicBlock*);
    void                                                                generate_return_stmt(ast_return_stmt*);
    void                                                                generate_continue_stmt(llvm::BasicBlock*);
    void                                                                generate_break_stmt(llvm::BasicBlock*);

    std::map<ast_decl*, llvm::Value*>                                   _named_declarations;
    local_scope*                                                        _local_scope;
    llvm::BasicBlock*                                                   _header_bb;

};

}



#endif /* IRCODEGEN_HPP_ */
