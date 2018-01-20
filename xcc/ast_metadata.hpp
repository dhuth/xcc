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

struct ast_metadata : public llvm_metadata {
public:

    template<typename TAstBuilderImpl>
    explicit ast_metadata(ircode_context& ircode_context, TAstBuilderImpl& builder_impl)
            : ast_metadata(ircode_context, (__ast_builder_impl&)builder_impl) {
        // do nothing
    }
    virtual ~ast_metadata() = default;

private:

    explicit ast_metadata(ircode_context&, __ast_builder_impl&);

    llvm::MDTuple* write_void_type(ast_void_type*);
    llvm::MDTuple* write_integer_type(ast_integer_type*);
    llvm::MDTuple* write_real_type(ast_real_type*);
    llvm::MDTuple* write_array_type(ast_array_type*);
    llvm::MDTuple* write_pointer_type(ast_pointer_type*);
    llvm::MDTuple* write_function_type(ast_function_type*);
    llvm::MDTuple* write_record_type(ast_record_type*);

    llvm::MDTuple* write_namespace_decl(ast_namespace_decl*);
    llvm::MDTuple* write_variable_decl(ast_variable_decl*);
    llvm::MDTuple* write_parameter_decl(ast_parameter_decl*);
    llvm::MDTuple* write_local_decl(ast_local_decl*);
    llvm::MDTuple* write_function_decl(ast_function_decl*);
    llvm::MDTuple* write_typedef_decl(ast_typedef_decl*);

    ast_void_type*          read_void_type(llvm::MDTuple*);
    ast_integer_type*       read_integer_type(llvm::MDTuple*);
    ast_real_type*          read_real_type(llvm::MDTuple*);
    ast_array_type*         read_array_type(llvm::MDTuple*);
    ast_pointer_type*       read_pointer_type(llvm::MDTuple*);
    ast_function_type*      read_function_type(llvm::MDTuple*);
    ast_record_type*        read_record_type(llvm::MDTuple*);

    ast_namespace_decl*     read_namespace_decl(llvm::MDTuple*);
    ast_variable_decl*      read_variable_decl(llvm::MDTuple*);
    ast_local_decl*         read_local_decl(llvm::MDTuple*);
    ast_parameter_decl*     read_parameter_decl(llvm::MDTuple*);
    ast_function_decl*      read_function_decl(llvm::MDTuple*);
    ast_typedef_decl*       read_typedef_decl(llvm::MDTuple*);

    __ast_builder_impl&                                     _ast_builder;
};


}



#endif /* XCC_AST_METADATA_HPP_ */
