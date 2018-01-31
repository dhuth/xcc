/*
 * xi_metadata.cpp
 *
 *  Created on: Jan 12, 2018
 *      Author: derick
 */

#include "xi_metadata.hpp"
#include "ircodegen.hpp"

namespace xcc {

xi_metadata_reader::xi_metadata_reader(llvm::LLVMContext& context, xi_builder& builder) noexcept
        : ast_metadata_reader(context, builder),
          _xi_builder(builder) {

    //Types
    this->addmethod(&xi_metadata_reader::read_xi_const_type);
    this->addmethod(&xi_metadata_reader::read_xi_auto_type);
    this->addmethod(&xi_metadata_reader::read_xi_reference_type);
    this->addmethod(&xi_metadata_reader::read_xi_decl_type);
    this->addmethod(&xi_metadata_reader::read_xi_tuple_type);

    //Declarations
    this->addmethod(&xi_metadata_reader::read_xi_namespace_decl);
    this->addmethod(&xi_metadata_reader::read_xi_parameter_decl);
    this->addmethod(&xi_metadata_reader::read_xi_function_decl);
    this->addmethod(&xi_metadata_reader::read_xi_field_decl);
    this->addmethod(&xi_metadata_reader::read_xi_method_decl);
    this->addmethod(&xi_metadata_reader::read_xi_struct_decl);
    this->addmethod(&xi_metadata_reader::read_xi_operator_function_decl);
    this->addmethod(&xi_metadata_reader::read_xi_operator_method_decl);
}

xi_const_type* xi_metadata_reader::read_xi_const_type(llvm::MDTuple* md) {
    ast_type*                       type;

    this->read_tuple(md, type);
    return _xi_builder.get_const_type(type);
}

xi_auto_type* xi_metadata_reader::read_xi_auto_type(llvm::MDTuple*) {
    return _xi_builder.get_auto_type();
}

xi_reference_type* xi_metadata_reader::read_xi_reference_type(llvm::MDTuple* md) {
    ast_type*                       type;

    this->read_tuple(md, type);
    return _xi_builder.get_reference_type(type);
}

xi_decl_type* xi_metadata_reader::read_xi_decl_type(llvm::MDTuple* md) {
    ast_decl*                       decl;

    this->read_tuple(md, decl);
    return _xi_builder.get_object_type(decl);
}

xi_tuple_type* xi_metadata_reader::read_xi_tuple_type(llvm::MDTuple* md) {
    list<ast_type>*                 types;

    this->read_tuple(md, types);
    return _xi_builder.get_tuple_type(types);
}

xi_namespace_decl* xi_metadata_reader::read_xi_namespace_decl(llvm::MDTuple* md) {
    std::string                     name;
    std::string                     generated_name;
    source_span                     location;
    list<ast_decl>*                 declarations;

    this->read_tuple(md, name, generated_name, location, declarations);
    auto ns = new xi_namespace_decl(name, declarations);
    this->set_decl_data(ns, generated_name, location);

    return ns;
}

xi_parameter_decl* xi_metadata_reader::read_xi_parameter_decl(llvm::MDTuple* md) {
    std::string                     name;
    std::string                     generated_name;
    source_span                     location;
    ast_type*                       type;

    this->read_tuple(md, name, generated_name, location, type);
    auto p = new xi_parameter_decl(name, type);
    this->set_decl_data(p, generated_name, location);

    return p;
}

xi_function_decl* xi_metadata_reader::read_xi_function_decl(llvm::MDTuple* md) {
    std::string                     name;
    std::string                     generated_name;
    source_span                     location;
    ast_type*                       return_type;
    list<xi_parameter_decl>*        parameters;

    this->read_tuple(md, name, generated_name, location, parameters);
    auto f = new xi_function_decl(name, return_type, parameters, nullptr);
    this->set_decl_data(f, generated_name, location);
    this->set_externable_data(f);

    //TODO: ast function lowered to (?)
    return f;
}

xi_field_decl* xi_metadata_reader::read_xi_field_decl(llvm::MDTuple* md) {
    std::string                     name;
    std::string                     generated_name;
    source_span                     location;
    ast_type*                       parent;
    bool                            is_static;
    ast_type*                       type;

    this->read_tuple(md,
            name,
            generated_name,
            location,
            parent,
            is_static,
            type);
    auto f = new xi_field_decl(name, type);
    this->set_decl_data(f, generated_name, location);
    this->set_member_data(f, parent, is_static);

    return f;
}

xi_method_decl* xi_metadata_reader::read_xi_method_decl(llvm::MDTuple* md) {
    std::string                     name;
    std::string                     generated_name;
    source_span                     location;
    ast_type*                       parent;
    bool                            is_static;
    ast_type*                       return_type;
    list<xi_parameter_decl>*        parameters;

    this->read_tuple(md,
            name,
            generated_name,
            location,
            parent,
            is_static,
            return_type,
            parameters);
    auto m = new xi_method_decl(name, return_type, parameters, nullptr);
    this->set_decl_data(m, generated_name, location);
    this->set_member_data(m, parent, is_static);

    return m;
}

xi_struct_decl* xi_metadata_reader::read_xi_struct_decl(llvm::MDTuple* md) {
    std::string                     name;
    std::string                     generated_name;
    source_span                     location;
    ast_type*                       parent;
    bool                            is_static;
    list<xi_member_decl>*           members;
    list<ast_type>*                 base_types;

    this->read_tuple(md,
            name,
            generated_name,
            location,
            parent,
            is_static,
            base_types,
            members);
    auto s = new xi_struct_decl(name, members, base_types);
    this->set_decl_data(s, generated_name, location);
    this->set_member_data(s, parent, is_static);

    return s;
}

xi_operator_function_decl* xi_metadata_reader::read_xi_operator_function_decl(llvm::MDTuple* md) {
    std::string                                             name;
    std::string                                             generated_name;
    source_span                                             location;
    xi_operator                                             op;
    xi_operator_function_decl::return_type_t*               return_type;
    list<xi_operator_function_decl::parameter_decl_t>*      parameters;

    this->read_tuple(md,
            name,
            generated_name,
            location,
            op,
            return_type,
            parameters);
    auto f = new xi_operator_function_decl(name, op, return_type, parameters, nullptr);
    this->set_decl_data(f, generated_name, location);
    this->set_externable_data(f);

    return f;
}

xi_operator_method_decl* xi_metadata_reader::read_xi_operator_method_decl(llvm::MDTuple* md) {
    std::string                                             name;
    std::string                                             generated_name;
    source_span                                             location;
    ast_type*                                               parent;
    bool                                                    is_static;
    xi_operator                                             op;
    xi_operator_method_decl::return_type_t*                 return_type;
    list<xi_operator_method_decl::parameter_decl_t>*        parameters;

    this->read_tuple(md,
            name,
            generated_name,
            location,
            parent,
            is_static,
            op,
            return_type,
            parameters);
    auto m = new xi_operator_method_decl(name, op, return_type, parameters, nullptr);
    this->set_decl_data(m, generated_name, location);
    this->set_member_data(m, parent, is_static);

    return m;
}

xi_metadata_writer::xi_metadata_writer(llvm::LLVMContext& context, xi_builder& builder) noexcept
        : ast_metadata_writer(context, builder),
          _xi_builder(builder) {

    //Types
    this->addmethod(&xi_metadata_writer::write_xi_const_type);
    this->addmethod(&xi_metadata_writer::write_xi_auto_type);
    this->addmethod(&xi_metadata_writer::write_xi_reference_type);
    this->addmethod(&xi_metadata_writer::write_xi_decl_type);
    this->addmethod(&xi_metadata_writer::write_xi_tuple_type);

    //Declarations
    this->addmethod(&xi_metadata_writer::write_xi_namespace_decl);
    this->addmethod(&xi_metadata_writer::write_xi_parameter_decl);
    this->addmethod(&xi_metadata_writer::write_xi_function_decl);
    this->addmethod(&xi_metadata_writer::write_xi_field_decl);
    this->addmethod(&xi_metadata_writer::write_xi_method_decl);
    this->addmethod(&xi_metadata_writer::write_xi_struct_decl);
    this->addmethod(&xi_metadata_writer::write_xi_operator_function_decl);
    this->addmethod(&xi_metadata_writer::write_xi_operator_method_decl);
}

llvm::MDTuple* xi_metadata_writer::write_xi_const_type(xi_const_type* tp) {
    return this->write_tuple(tp->type);
}

llvm::MDTuple* xi_metadata_writer::write_xi_auto_type(xi_auto_type*) {
    return this->write_tuple();
}

llvm::MDTuple* xi_metadata_writer::write_xi_reference_type(xi_reference_type* rt) {
    return this->write_tuple(rt->type);
}

llvm::MDTuple* xi_metadata_writer::write_xi_decl_type(xi_decl_type* dt) {
    return this->write_tuple(dt->declaration);
}

llvm::MDTuple* xi_metadata_writer::write_xi_tuple_type(xi_tuple_type* tt) {
    return this->write_tuple(tt->types);
}

llvm::MDTuple* xi_metadata_writer::write_xi_namespace_decl(xi_namespace_decl* ns) {
    return this->write_tuple(
            ns->name,
            ns->generated_name,
            ns->source_location,
            ns->declarations);
}

llvm::MDTuple* xi_metadata_writer::write_xi_parameter_decl(xi_parameter_decl* pd) {
    return this->write_tuple(
            pd->name,
            pd->generated_name,
            pd->source_location,
            pd->type);
}

llvm::MDTuple* xi_metadata_writer::write_xi_function_decl(xi_function_decl* fd) {
    return this->write_tuple(
            fd->name,
            fd->generated_name,
            fd->source_location,
            fd->return_type,
            fd->parameters);
    //TODO: ast function lowered to (?)
}

llvm::MDTuple* xi_metadata_writer::write_xi_field_decl(xi_field_decl* fd) {
    return this->write_tuple(
            fd->name,
            fd->generated_name,
            fd->source_location,
            fd->parent,
            fd->is_static,
            fd->type);
    //TODO: initial value (?)
}

llvm::MDTuple* xi_metadata_writer::write_xi_method_decl(xi_method_decl* md) {
    return this->write_tuple(
            md->name,
            md->generated_name,
            md->source_location,
            md->parent,
            md->is_static,
            md->return_type,
            md->parameters);
    //TODO: ast function lowered to (?)
}

llvm::MDTuple* xi_metadata_writer::write_xi_struct_decl(xi_struct_decl* sd) {
    return this->write_tuple(
            sd->name,
            sd->generated_name,
            sd->source_location,
            sd->parent,
            sd->is_static,
            sd->base_types,
            sd->members);
}

llvm::MDTuple* xi_metadata_writer::write_xi_operator_function_decl(xi_operator_function_decl* fd) {
    return this->write_tuple(
            fd->name,
            fd->generated_name,
            fd->source_location,
            fd->op,
            fd->return_type,
            fd->parameters);
    //TODO: ast function lowered to (?)
}

llvm::MDTuple* xi_metadata_writer::write_xi_operator_method_decl(xi_operator_method_decl* md) {
    return this->write_tuple(
            md->name,
            md->generated_name,
            md->source_location,
            md->parent,
            md->is_static,
            md->op,
            md->return_type,
            md->parameters);
    //TODO: ast function lowered to (?)
}

bool xi_builder::read_metadata_pass(ircode_context& ir) noexcept {
    //TODO: ...
    return true;
}

bool xi_builder::write_metadata_pass(ircode_context& ir) noexcept {
    xi_metadata_writer writer(ir.llvm_context, *this);

    writer.write_to_module(ir.module, "declarations", this->global_namespace->declarations);

    return true;
}


}

