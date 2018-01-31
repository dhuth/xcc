/*
 * ast_metadata.hpp
 *
 *  Created on: Jan 12, 2018
 *      Author: derick
 */

#ifndef XCC_AST_METADATA_HPP_
#define XCC_AST_METADATA_HPP_

#include "ast_builder.hpp"
#include "llvm_metadata.hpp"

namespace xcc {

struct ast_metadata_reader : public llvm_metadata_reader {
public:

    explicit ast_metadata_reader(llvm::LLVMContext&, __ast_builder_impl&) noexcept;

protected:

    inline void set_decl_data(ast_decl* decl, std::string& generated_name, source_span& location) noexcept {
        decl->generated_name    = generated_name;
        decl->source_location   = location;
    }
    //TODO: is_extern_visible might be an option
    inline void set_externable_data(ast_externable* e) {
        e->is_extern            = true;
        e->is_extern_visible    = true;
    }

private:

    // Types
    ast_void_type*              read_void_type(llvm::MDTuple*);
    ast_integer_type*           read_integer_type(llvm::MDTuple*);
    ast_real_type*              read_real_type(llvm::MDTuple*);
    ast_pointer_type*           read_pointer_type(llvm::MDTuple*);
    ast_array_type*             read_array_type(llvm::MDTuple*);
    ast_function_type*          read_function_type(llvm::MDTuple*);
    ast_record_type*            read_record_type(llvm::MDTuple*);

    // Declarations
    ast_namespace_decl*         read_namespace_decl(llvm::MDTuple*);
    ast_variable_decl*          read_variable_decl(llvm::MDTuple*);
    ast_parameter_decl*         read_parameter_decl(llvm::MDTuple*);
    ast_function_decl*          read_function_decl(llvm::MDTuple*);
    ast_typedef_decl*           read_typedef_decl(llvm::MDTuple*);

    __ast_builder_impl&                                     _ast_builder;

};

struct ast_metadata_writer : public llvm_metadata_writer {
public:

    explicit ast_metadata_writer(llvm::LLVMContext&, __ast_builder_impl&) noexcept;

private:

    // Types
    llvm::MDTuple*              write_void_type(ast_void_type*);
    llvm::MDTuple*              write_integer_type(ast_integer_type*);
    llvm::MDTuple*              write_real_type(ast_real_type*);
    llvm::MDTuple*              write_pointer_type(ast_pointer_type*);
    llvm::MDTuple*              write_array_type(ast_array_type*);
    llvm::MDTuple*              write_function_type(ast_function_type*);
    llvm::MDTuple*              write_record_type(ast_record_type*);

    // Declarations
    llvm::MDTuple*              write_namespace_decl(ast_namespace_decl*);
    llvm::MDTuple*              write_variable_decl(ast_variable_decl*);
    llvm::MDTuple*              write_parameter_decl(ast_parameter_decl*);
    llvm::MDTuple*              write_function_decl(ast_function_decl*);
    llvm::MDTuple*              write_typedef_decl(ast_typedef_decl*);

    __ast_builder_impl&                                     _ast_builder;

};

}



#endif /* XCC_AST_METADATA_HPP_ */
