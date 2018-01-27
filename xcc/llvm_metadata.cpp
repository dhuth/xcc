/*
 * llvm_metadata.cpp
 *
 *  Created on: Dec 12, 2017
 *      Author: derick
 */

#include "llvm_metadata.hpp"
#include "ircodegen.hpp"

namespace xcc {

bool __llvm_metadata_io_base::is_null(llvm::Metadata* md) const noexcept {
    if(llvm::isa<llvm::ConstantAsMetadata>(md)) {
        return llvm::dyn_cast<llvm::ConstantAsMetadata>(md)->getValue()->isNullValue();
    }
    return false;
}

llvm::Metadata* __llvm_metadata_io_base::get_null() const noexcept {
    return _null;
}

llvm::Metadata* __llvm_metadata_io_base::get_true() const noexcept {
    return _true;
}

llvm::Metadata* __llvm_metadata_io_base::get_false() const noexcept {
    return _false;
}

tree_t* llvm_metadata_reader::read_node(llvm::Metadata* md) noexcept {
    if(this->is_null(md)) {
        return nullptr;
    }

    // unpack tree node
    llvm::MDTuple*  md_tuple    = llvm::dyn_cast<llvm::MDTuple>(md);
    tree_type_id    id          = (tree_type_id)    this->read<uint64_t>(md_tuple->getOperand(0).get());
    llvm::MDTuple*  tree_body   = llvm::dyn_cast<llvm::MDTuple>(md_tuple->getOperand(1).get());

    // read body
    // TODO: check if read already

    return _read_functions[id](tree_body);
}

llvm::Metadata* llvm_metadata_writer::write_node(tree_t* t) noexcept {
    if(t == nullptr) {
        return this->get_null();
    }

    // write body
    tree_type_id    id          = t->get_tree_type();
    // TODO: check if written already

    llvm::MDTuple*  tree_body   = _write_functions[id](t);

    // pack tree node
    std::vector<llvm::Metadata*> md_vec = { this->write(id), tree_body };
    return llvm::MDTuple::get(this->llvm_context, md_vec);
}

}

