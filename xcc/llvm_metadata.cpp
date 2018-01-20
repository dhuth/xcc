/*
 * llvm_metadata.cpp
 *
 *  Created on: Dec 12, 2017
 *      Author: derick
 */

#include "llvm_metadata.hpp"
#include "ircodegen.hpp"

namespace xcc {

llvm_metadata::llvm_metadata(ircode_context& ircode_context)
        : _context(ircode_context.llvm_context),
          _null_value(llvm::Constant::getNullValue(llvm::Type::getInt32PtrTy(_context))),
          _null_metadata(llvm::ValueAsMetadata::get(_null_value)) {
    // do nothing .... yet
}

tree_t* llvm_metadata::read_node(llvm::Metadata* md) {

    // -- check if null --
    if(this->is_null(md)) {
        return nullptr;
    }

    if(this->_read_nodes.find(md) == this->_read_nodes.end()) {

        // -- read header --
        llvm::MDTuple*  tmd         = llvm::dyn_cast<llvm::MDTuple>(md);
        tree_type_id    id          = (tree_type_id) this->parse<uint64_t>(tmd->getOperand(0).get());
        llvm::MDTuple*  data        = llvm::dyn_cast<llvm::MDTuple>(tmd->getOperand(1).get());

        // -- read data --
        readf_t         rdfunc      = this->_read_funcs[id];
        tree_t*         res         = rdfunc(data);

        this->_read_nodes[md]       = res;
        this->_written_nodes[res]   = md;
        this->_managed_references.push_back(box(res));
    }
    return this->_read_nodes[md];
}

llvm::Metadata* llvm_metadata::write_node(tree_t* t) {

    // -- check if null --
    if(t == nullptr) {
        return this->write(nullptr);
    }

    // -- check if written --
    if(this->_written_nodes.find(t) == this->_written_nodes.end()) {

        // -- write data --
        tree_type_id    id          = t->get_tree_type();
        writef_t        wtfunc      = this->_write_funcs[id];
        llvm::MDTuple*  data        = wtfunc(t);

        // -- write header --
        std::vector<llvm::Metadata*> vec = { this->write((uint64_t)id), data };
        llvm::Metadata* md          = llvm::MDTuple::get(_context, vec);

        this->_written_nodes[t]     = md;
        this->_read_nodes[md]       = t;
        this->_managed_references.push_back(box(t));
    }
    return this->_written_nodes[t];
}

}

