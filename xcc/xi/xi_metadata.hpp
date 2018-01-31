/*
 * xi_metadata.hpp
 *
 *  Created on: Jan 30, 2018
 *      Author: derick
 */

#ifndef XCC_XI_XI_METADATA_HPP_
#define XCC_XI_XI_METADATA_HPP_

#include "ast_metadata.hpp"
#include "xi_builder.hpp"

namespace xcc {

template<> struct __llvm_io_reader<xi_operator> {
    static inline xi_operator read(llvm::Metadata* md, llvm_metadata_reader& r) {
        return (xi_operator) __llvm_io_int_reader<uint64_t>::read(md, r);
    }
};

struct xi_metadata_reader : public ast_metadata_reader {
public:

    explicit xi_metadata_reader(llvm::LLVMContext&, xi_builder&) noexcept;

protected:

    inline void set_member_data(xi_member_decl* m, ast_type* parent, bool is_static) {
        m->parent                   = parent;
        m->is_static                = is_static;
    }

private:

    //Types
    xi_const_type*                  read_xi_const_type(llvm::MDTuple*);
    xi_auto_type*                   read_xi_auto_type(llvm::MDTuple*);
    xi_reference_type*              read_xi_reference_type(llvm::MDTuple*);
    xi_decl_type*                   read_xi_decl_type(llvm::MDTuple*);
    xi_tuple_type*                  read_xi_tuple_type(llvm::MDTuple*);

    //Declarations
    xi_parameter_decl*              read_xi_parameter_decl(llvm::MDTuple*);
    xi_function_decl*               read_xi_function_decl(llvm::MDTuple*);
    xi_field_decl*                  read_xi_field_decl(llvm::MDTuple*);
    xi_method_decl*                 read_xi_method_decl(llvm::MDTuple*);
    xi_struct_decl*                 read_xi_struct_decl(llvm::MDTuple*);
    xi_operator_function_decl*      read_xi_operator_function_decl(llvm::MDTuple*);
    xi_operator_method_decl*        read_xi_operator_method_decl(llvm::MDTuple*);

    xi_builder&                                             _xi_builder;

};

template<> struct __llvm_io_writer<xi_operator> {
    static inline llvm::Metadata* write(xi_operator op, llvm_metadata_writer& w) {
        return __llvm_io_int_writer<uint64_t>::write((uint64_t) op, w);
    }
};

struct xi_metadata_writer : public ast_metadata_writer {
public:

    explicit xi_metadata_writer(llvm::LLVMContext&, xi_builder&) noexcept;

private:

    //Types
    llvm::MDTuple*                  write_xi_const_type(xi_const_type*);
    llvm::MDTuple*                  write_xi_auto_type(xi_auto_type*);
    llvm::MDTuple*                  write_xi_reference_type(xi_reference_type*);
    llvm::MDTuple*                  write_xi_decl_type(xi_decl_type*);
    llvm::MDTuple*                  write_xi_tuple_type(xi_tuple_type*);

    //Declarations
    llvm::MDTuple*                  write_xi_parameter_decl(xi_parameter_decl*);
    llvm::MDTuple*                  write_xi_function_decl(xi_function_decl*);
    llvm::MDTuple*                  write_xi_field_decl(xi_field_decl*);
    llvm::MDTuple*                  write_xi_method_decl(xi_method_decl*);
    llvm::MDTuple*                  write_xi_struct_decl(xi_struct_decl*);
    llvm::MDTuple*                  write_xi_operator_function_decl(xi_operator_function_decl*);
    llvm::MDTuple*                  write_xi_operator_method_decl(xi_operator_method_decl*);

    xi_builder&                                             _xi_builder;
};

}



#endif /* XCC_XI_XI_METADATA_HPP_ */
