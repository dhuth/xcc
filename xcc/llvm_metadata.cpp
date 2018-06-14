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

tree_type_id llvm_metadata_reader::read_tree_type_id(llvm::Metadata* md) noexcept {
    return (tree_type_id) this->read<uint64_t>(md);
}

tree_t* llvm_metadata_reader::read_node(llvm::Metadata* md) noexcept {
    if(this->is_null(md)) {
        return nullptr;
    }
    if(_read_metadata.find(md) == _read_metadata.end()) {
        // unpack tree node
        llvm::MDTuple*  md_tuple    = llvm::dyn_cast<llvm::MDTuple>(md);
        tree_type_id    id          = this->read_tree_type_id(md_tuple->getOperand(0).get());
        llvm::MDTuple*  tree_body   = llvm::dyn_cast<llvm::MDTuple>(md_tuple->getOperand(1).get());

        auto read_tree = _read_functions[id](tree_body);
        _read_metadata[md] = read_tree;
        return read_tree;
    }
    return _read_metadata[md];
}

llvm::Metadata* llvm_metadata_writer::write_node(tree_t* t) noexcept {
    if(t == nullptr) {
        return this->get_null();
    }
    if(_written_nodes.find(t) == _written_nodes.end()) {
        // write body
        tree_type_id    id          = t->get_tree_type();
        llvm::MDTuple*  tree_body   = _write_functions[id](t);

        // pack tree node
        std::vector<llvm::Metadata*> md_vec = { this->write_tree_type_id(id), tree_body };
        return llvm::MDTuple::get(this->llvm_context, md_vec);
    }
    return _written_nodes[t];
}

llvm::Metadata* llvm_metadata_writer::write_tree_type_id(tree_type_id id) noexcept {
    return this->write<uint64_t>((uint64_t)id);
}

void llvm_metadata_writer::write_to_named_metadata(llvm::NamedMDNode* md_named_node, tree_t* t) {
    auto md_tree_node  = this->write_node(t);
    assert(md_tree_node == nullptr || llvm::isa<llvm::MDNode>(md_tree_node));
    md_named_node->addOperand(llvm::dyn_cast<llvm::MDNode>(md_tree_node));
}

void llvm_metadata_writer::write_to_named_metadata(llvm::Module& m, std::string name, tree_t* t) {
    auto md_named_node = m.getOrInsertNamedMetadata(name);
    this->write_to_named_metadata(md_named_node, t);
}

}

