/*
 * ast_metadata.cpp
 *
 *  Created on: Jan 17, 2018
 *      Author: derick
 */

#include "ast_metadata.hpp"


namespace xcc {

ast_metadata_writer::ast_metadata_writer(llvm::LLVMContext& llvm_context, __ast_builder_impl& builder) noexcept
        : llvm_metadata_writer(llvm_context),
          _ast_builder(builder) {

    // Types
    this->addmethod(&ast_metadata_writer::write_void_type);
    this->addmethod(&ast_metadata_writer::write_integer_type);
    this->addmethod(&ast_metadata_writer::write_real_type);
    this->addmethod(&ast_metadata_writer::write_pointer_type);
    this->addmethod(&ast_metadata_writer::write_array_type);
    this->addmethod(&ast_metadata_writer::write_function_type);
    this->addmethod(&ast_metadata_writer::write_record_type);

    // Declarations
    this->addmethod(&ast_metadata_writer::write_namespace_decl);
    this->addmethod(&ast_metadata_writer::write_variable_decl);
    this->addmethod(&ast_metadata_writer::write_parameter_decl);
    this->addmethod(&ast_metadata_writer::write_function_decl);
    this->addmethod(&ast_metadata_writer::write_typedef_decl);

}

llvm::MDTuple* ast_metadata_writer::write_void_type(ast_void_type*) {
    return this->write_tuple();
}

llvm::MDTuple* ast_metadata_writer::write_integer_type(ast_integer_type* i) {
    return this->write_tuple(i->bitwidth, i->is_unsigned);
}

llvm::MDTuple* ast_metadata_writer::write_real_type(ast_real_type* r) {
    return this->write_tuple(r->bitwidth);
}

llvm::MDTuple* ast_metadata_writer::write_pointer_type(ast_pointer_type* p) {
    return this->write_tuple(p->element_type);
}

llvm::MDTuple* ast_metadata_writer::write_array_type(ast_array_type* a) {
    return this->write_tuple(a->element_type, a->size);
}

llvm::MDTuple* ast_metadata_writer::write_function_type(ast_function_type* f) {
    return this->write_tuple(f->return_type, f->parameter_types);
}

llvm::MDTuple* ast_metadata_writer::write_record_type(ast_record_type* r) {
    return this->write_tuple(r->field_types, r->is_packed);
}

llvm::MDTuple* ast_metadata_writer::write_namespace_decl(ast_namespace_decl* n) {
    return this->write_tuple(
            n->name,
            n->generated_name,
            n->source_location,
            n->declarations);
}

llvm::MDTuple* ast_metadata_writer::write_variable_decl(ast_variable_decl* v) {
    return this->write_tuple(
            v->name,
            v->generated_name,
            v->source_location,
            v->type);
}

llvm::MDTuple* ast_metadata_writer::write_parameter_decl(ast_parameter_decl* p) {
    return this->write_tuple(
            p->name,
            p->generated_name,
            p->source_location,
            p->type);
}

llvm::MDTuple* ast_metadata_writer::write_function_decl(ast_function_decl* f) {
    return this->write_tuple(
            f->name,
            f->generated_name,
            f->source_location,
            f->return_type,
            f->parameters);
}

llvm::MDTuple* ast_metadata_writer::write_typedef_decl(ast_typedef_decl* t) {
    return this->write_tuple(
            t->name,
            t->generated_name,
            t->source_location,
            t->type);
}


ast_metadata_reader::ast_metadata_reader(llvm::LLVMContext& llvm_context, __ast_builder_impl& builder) noexcept
        : llvm_metadata_reader(llvm_context),
          _ast_builder(builder) {

    // Types
    this->addmethod(&ast_metadata_reader::read_void_type);
    this->addmethod(&ast_metadata_reader::read_integer_type);
    this->addmethod(&ast_metadata_reader::read_real_type);
    this->addmethod(&ast_metadata_reader::read_pointer_type);
    this->addmethod(&ast_metadata_reader::read_array_type);
    this->addmethod(&ast_metadata_reader::read_function_type);
    this->addmethod(&ast_metadata_reader::read_record_type);

    // Declarations
    this->addmethod(&ast_metadata_reader::read_namespace_decl);
    this->addmethod(&ast_metadata_reader::read_variable_decl);
    this->addmethod(&ast_metadata_reader::read_parameter_decl);
    this->addmethod(&ast_metadata_reader::read_function_decl);
    this->addmethod(&ast_metadata_reader::read_typedef_decl);
}

ast_void_type* ast_metadata_reader::read_void_type(llvm::MDTuple*) {
    return _ast_builder.get_void_type();
}

ast_integer_type* ast_metadata_reader::read_integer_type(llvm::MDTuple* md) {
    uint32_t            bitwidth;
    bool                is_unsigned;

    this->read_tuple(md, bitwidth, is_unsigned);
    return _ast_builder.get_integer_type(bitwidth, is_unsigned);
}

ast_real_type* ast_metadata_reader::read_real_type(llvm::MDTuple* md) {
    uint32_t            bitwidth;

    this->read_tuple(md, bitwidth);
    return _ast_builder.get_real_type(bitwidth);
}

ast_pointer_type* ast_metadata_reader::read_pointer_type(llvm::MDTuple* md) {
    ast_type*           t;

    this->read_tuple(md, t);
    return _ast_builder.get_pointer_type(t);
}

ast_array_type* ast_metadata_reader::read_array_type(llvm::MDTuple* md) {
    ast_type*           t;
    uint32_t            size;

    this->read_tuple(md, t, size);
    return _ast_builder.get_array_type(t, size);
}

ast_function_type* ast_metadata_reader::read_function_type(llvm::MDTuple* md) {
    ast_type*           ret;
    list<ast_type>*     args;

    this->read_tuple(md, ret, args);
    return _ast_builder.get_function_type(ret, args);
}

ast_record_type* ast_metadata_reader::read_record_type(llvm::MDTuple* md) {
    list<ast_type>*     types;
    bool                is_packed;

    this->read_tuple(md, types, is_packed);
    return _ast_builder.get_record_type(types);
}

ast_namespace_decl* ast_metadata_reader::read_namespace_decl(llvm::MDTuple* md) {
    std::string                 name;
    std::string                 generated_name;
    source_span                 location;
    list<ast_decl>*             declarations;

    this->read_tuple(md, name, generated_name, location, declarations);

    auto ns = new ast_namespace_decl(name, declarations);
    this->set_decl_data(ns, generated_name, location);

    return ns;
}

ast_variable_decl* ast_metadata_reader::read_variable_decl(llvm::MDTuple* md) {
    std::string                 name;
    std::string                 generated_name;
    source_span                 location;
    ast_type*                   type;

    this->read_tuple(md, name, generated_name, location, type);

    auto v = new ast_variable_decl(name, type);
    this->set_decl_data(v, generated_name, location);
    this->set_externable_data(v);

    return v;
}

ast_parameter_decl* ast_metadata_reader::read_parameter_decl(llvm::MDTuple* md) {
    std::string                 name;
    std::string                 generated_name;
    source_span                 location;
    ast_type*                   type;

    this->read_tuple(md, name, generated_name, location, type);
    auto p = new ast_parameter_decl(name, type);
    this->set_decl_data(p, generated_name, location);

    return p;
}

ast_function_decl* ast_metadata_reader::read_function_decl(llvm::MDTuple* md) {
    std::string                 name;
    std::string                 generated_name;
    source_span                 location;
    ast_type*                   return_type;
    list<ast_parameter_decl>*   parameters;

    this->read_tuple(md, name, generated_name, location, return_type, parameters);
    auto f = new ast_function_decl(name, return_type, parameters, nullptr);
    this->set_decl_data(f, generated_name, location);
    this->set_externable_data(f);
    f->is_c_extern              = true;

    return f;
}

ast_typedef_decl* ast_metadata_reader::read_typedef_decl(llvm::MDTuple* md) {
    std::string                 name;
    std::string                 generated_name;
    source_span                 location;
    ast_type*                   type;

    this->read_tuple(md, name, generated_name, location, type);
    auto d = new ast_typedef_decl(name, type);
    this->set_decl_data(d, generated_name, location);

    return d;
}
}



