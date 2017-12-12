/*
 * llvm_metadata.hpp
 *
 *  Created on: Dec 11, 2017
 *      Author: derick
 */

#ifndef XCC_LLVM_METADATA_HPP_
#define XCC_LLVM_METADATA_HPP_

#include "tree.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Metadata.h>
#include <llvm/IR/Module.h>

namespace xcc {

struct llvm_metadata;

struct metadata_writer : public dispatch_visitor<llvm::Metadata*, llvm_metadata&> {
    //...
};


struct metadata_reader;


struct llvm_metadata {
public:

    inline llvm_metadata(llvm::Module& m, llvm::LLVMContext& c, metadata_writer& w, metadata_reader& r) noexcept
            : _module(m),
              _context(c),
              _writer(w),
              _reader(r) {
        /* do nothing */
    }

    inline llvm::Metadata* tometadata(tree_t* t) {
    }

private:

    llvm::Module&                                               _module;
    llvm::LLVMContext&                                          _context;
    metadata_writer&                                            _writer;
    metadata_reader&                                            _reader;

};

}

#endif /* XCC_LLVM_METADATA_HPP_ */
