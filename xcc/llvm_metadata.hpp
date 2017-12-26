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

#include <tuple>

namespace xcc {

struct llvm_metadata {
public:

    inline llvm_metadata(llvm::LLVMContext& context)
            : _context(context) {
        // ...
    }
    virtual ~llvm_metadata() = default;

protected:

    typedef std::function<llvm::Metadata*(tree_t*, llvm::LLVMContext&)>
                                                                wfunc_t;
    typedef std::function<tree_t*(llvm::Metadata*, llvm::LLVMContext&)>
                                                                rfunc_t;

    template<typename TReturnType,
             typename TTreeType,
             typename TClassType,
             typename std::enable_if<std::is_base_of<llvm::Metadata, TReturnType>::value, int>::type = 0>
    void add_write_method(TReturnType*(TClassType::*mptr)(TTreeType*, llvm::LLVMContext&)) {
        auto func = [=](tree_t* t, llvm::LLVMContext& context) -> llvm::Metadata* {
            return llvm::dyn_cast<llvm::Metadata>((dynamic_cast<TClassType*>(this)->*mptr)(t, context));
        };
        this->_write_functions[__get_tree_type_id<TTreeType>()] = func;
    }

    template<typename TReturnType,
             typename TMetadataType,
             typename TClassType>
    void add_read_method(TReturnType*(TClassType::*mptr)(TMetadataType*, llvm::LLVMContext&)) {
        auto func = [=](llvm::Metadata* md, llvm::LLVMContext& context) -> tree_t* {
            return (tree_t*) (dynamic_cast<TClassType*>(this)->*mptr)(llvm::dyn_cast<TMetadataType>(md), context);
        };
        this->_read_functions[__get_tree_type_id<TReturnType>()] = func;
    }

private:

    tree_t*             read_impl(llvm::Metadata*);
    llvm::Metadata*     write_impl(tree_t*);

    llvm::LLVMContext&                                          _context;
    std::map<tree_type_id, wfunc_t>                             _write_functions;
    std::map<tree_type_id, rfunc_t>                             _read_functions;

};

}

#endif /* XCC_LLVM_METADATA_HPP_ */
