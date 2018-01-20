/*
 * ast_metadata.cpp
 *
 *  Created on: Jan 17, 2018
 *      Author: derick
 */

// TODO: source file location info (And other debug goodies)

#include "ast_metadata.hpp"


namespace xcc {

ast_metadata::ast_metadata(ircode_context& ctx, __ast_builder_impl& builder_impl)
        : llvm_metadata(ctx),
          _ast_builder(builder_impl) {

    this->add_write_method(&ast_metadata::write_void_type);
    this->add_write_method(&ast_metadata::write_integer_type);
    this->add_write_method(&ast_metadata::write_real_type);
    this->add_write_method(&ast_metadata::write_pointer_type);
    this->add_write_method(&ast_metadata::write_array_type);
    this->add_write_method(&ast_metadata::write_function_type);
    this->add_write_method(&ast_metadata::write_record_type);

    this->add_write_method(&ast_metadata::write_namespace_decl);
    this->add_write_method(&ast_metadata::write_variable_decl);
    this->add_write_method(&ast_metadata::write_local_decl);
    this->add_write_method(&ast_metadata::write_parameter_decl);
    this->add_write_method(&ast_metadata::write_function_decl);
    this->add_write_method(&ast_metadata::write_typedef_decl);

    this->add_read_method(&ast_metadata::read_void_type);
    this->add_read_method(&ast_metadata::read_integer_type);
    this->add_read_method(&ast_metadata::read_real_type);
    this->add_read_method(&ast_metadata::read_pointer_type);
    this->add_read_method(&ast_metadata::read_array_type);
    this->add_read_method(&ast_metadata::read_function_type);

    this->add_read_method(&ast_metadata::read_namespace_decl);
    this->add_read_method(&ast_metadata::read_variable_decl);
    this->add_read_method(&ast_metadata::read_local_decl);
    this->add_read_method(&ast_metadata::read_parameter_decl);
    this->add_read_method(&ast_metadata::read_function_decl);
    this->add_read_method(&ast_metadata::read_typedef_decl);

}

// ===== //
// Types //
// ===== //

llvm::MDTuple* ast_metadata::write_void_type(ast_void_type*) {
    return this->write_tuple();
}
ast_void_type* ast_metadata::read_void_type(llvm::MDTuple*) {
    return _ast_builder.get_void_type();
}


llvm::MDTuple* ast_metadata::write_integer_type(ast_integer_type* itype) {
    return this->write_tuple(itype->bitwidth, itype->is_unsigned);
}
ast_integer_type* ast_metadata::read_integer_type(llvm::MDTuple* itype) {
    uint32_t    bitwidth;
    bool        is_unsigned;

    this->parse_tuple(itype, bitwidth, is_unsigned);
    return _ast_builder.get_integer_type(bitwidth, is_unsigned);
}


llvm::MDTuple* ast_metadata::write_real_type(ast_real_type* rtype) {
    return this->write_tuple(rtype->bitwidth);
}
ast_real_type* ast_metadata::read_real_type(llvm::MDTuple* rtype) {
    uint32_t    bitwidth;

    this->parse_tuple(rtype, bitwidth);
    return _ast_builder.get_real_type(bitwidth);
}


llvm::MDTuple* ast_metadata::write_pointer_type(ast_pointer_type* ptype) {
    return this->write_tuple(ptype->element_type);
}
ast_pointer_type* ast_metadata::read_pointer_type(llvm::MDTuple* ptype) {
    ast_type*   eltype;

    this->parse_tuple(ptype, eltype);
    return _ast_builder.get_pointer_type(eltype);
}


llvm::MDTuple* ast_metadata::write_array_type(ast_array_type* atype) {
    return this->write_tuple(atype->element_type, atype->size);
}
ast_array_type* ast_metadata::read_array_type(llvm::MDTuple* atype) {
    ast_type*   eltype;
    uint32_t    size;

    this->parse_tuple(atype, eltype, size);
    return _ast_builder.get_array_type(eltype, size);
}


llvm::MDTuple* ast_metadata::write_function_type(ast_function_type* ftype) {
    return this->write_tuple(ftype->return_type, ftype->parameter_types);
}
ast_function_type* ast_metadata::read_function_type(llvm::MDTuple* ftype) {
    ast_type*       rtype;
    list<ast_type>* param_types;

    this->parse_tuple(ftype, rtype, param_types);
    return _ast_builder.get_function_type(rtype, param_types);
}


llvm::MDTuple* ast_metadata::write_record_type(ast_record_type* rtype) {
    return this->write_tuple(rtype->is_packed, rtype->field_types);
}
ast_record_type* ast_metadata::read_record_type(llvm::MDTuple* rtype) {
    list<ast_type>* ftypes;
    bool            is_packed;

    this->parse_tuple(rtype, ftypes, is_packed);
    return _ast_builder.get_record_type(ftypes);
}

// ============ //
// Declarations //
// ============ //

llvm::MDTuple* ast_metadata::write_namespace_decl(ast_namespace_decl* ns) {
    return this->write_tuple(ns->name, ns->declarations);
}
ast_namespace_decl* ast_metadata::read_namespace_decl(llvm::MDTuple* ns) {
    std::string                 name;
    list<ast_decl>*             declarations;

    this->parse_tuple(ns, name, declarations);
    return new ast_namespace_decl(name, declarations);
}


llvm::MDTuple* ast_metadata::write_variable_decl(ast_variable_decl* var) {
    //TODO: Initial value?
    //TODO: generated name
    return this->write_tuple(var->name, var->is_extern, var->is_extern_visible, var->type);
}
ast_variable_decl* ast_metadata::read_variable_decl(llvm::MDTuple* var) {
    //TODO: Initial value?
    //TODO: generated name
    std::string                 name;
    bool                        is_extern;
    bool                        is_extern_visible;
    ast_type*                   type;

    this->parse_tuple(var, name, is_extern, is_extern_visible, type);
    auto newv = new ast_variable_decl(name, type);
    newv->is_extern         =   is_extern;
    newv->is_extern_visible =   is_extern_visible;
    return newv;
}


//TODO: is this needed?
llvm::MDTuple* ast_metadata::write_local_decl(ast_local_decl* loc) {
    //TODO: Initial value?
    //TODO: generated name
    return this->write_tuple(loc->name, loc->type);
}
//TODO: is this needed?
ast_local_decl* ast_metadata::read_local_decl(llvm::MDTuple* loc) {
    //TODO: Initial value?
    //TODO: generated name
    std::string                 name;
    ast_type*                   type;

    this->parse_tuple(loc, name, type);
    return new ast_local_decl(name, type, nullptr);
}


llvm::MDTuple* ast_metadata::write_parameter_decl(ast_parameter_decl* param) {
    //TODO: generated name
    return this->write_tuple(param->name, param->type);
}
ast_parameter_decl* ast_metadata::read_parameter_decl(llvm::MDTuple* param) {
    //TODO: generated name
    std::string                 name;
    ast_type*                   type;

    this->parse_tuple(param, name, type);
    return new ast_parameter_decl(name, type);
}


llvm::MDTuple* ast_metadata::write_function_decl(ast_function_decl* func) {
    //TODO: Body?
    //TODO: generated name
    return this->write_tuple(
            func->name,
            func->is_c_extern,
            func->is_extern,
            func->is_extern_visible,
            func->return_type,
            func->parameters);
}
ast_function_decl* ast_metadata::read_function_decl(llvm::MDTuple* func) {
    //TODO: Body?
    //TODO: generated name
    std::string                 name;
    bool                        is_c_extern;
    bool                        is_extern;
    bool                        is_extern_visible;
    ast_type*                   return_type;
    list<ast_parameter_decl>*   parameters;

    this->parse_tuple(func, name, is_c_extern, is_extern, is_extern_visible, return_type, parameters);
    auto newf = new ast_function_decl(name, return_type, parameters, nullptr);
    newf->is_c_extern       =   is_c_extern;
    newf->is_extern         =   is_extern;
    newf->is_extern_visible =   is_extern_visible;

    return newf;
}


llvm::MDTuple* ast_metadata::write_typedef_decl(ast_typedef_decl* tdef) {
    //TODO: generated name
    return this->write_tuple(tdef->name, tdef->type);
}
ast_typedef_decl* ast_metadata::read_typedef_decl(llvm::MDTuple* tdef) {
    //TODO: generated name
    std::string                 name;
    ast_type*                   type;

    this->parse_tuple(tdef, name, type);
    return new ast_typedef_decl(name, type);
}
}




