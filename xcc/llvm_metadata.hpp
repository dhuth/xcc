/*
 * llvm_metadata.hpp
 *
 *  Created on: Dec 11, 2017
 *      Author: derick
 */

#ifndef XCC_LLVM_METADATA_HPP_
#define XCC_LLVM_METADATA_HPP_

#include "cppdefs.hpp"
#include "cpp_type_traits_ext.hpp"
#include "tree.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/Metadata.h>
#include <llvm/ADT/APInt.h>
#include <llvm/ADT/APSInt.h>
#include <llvm/ADT/APFloat.h>

#include <set>
#include <tuple>
#include <utility>

namespace xcc {

struct ircode_context;
struct llvm_metadata;

template<typename T>
struct __llvm_io {
    static T                parse(llvm_metadata*, llvm::Metadata*);
    static llvm::Metadata*  write(llvm_metadata*, T);
};

struct llvm_metadata {
public:

    explicit llvm_metadata(ircode_context&);
    virtual ~llvm_metadata() = default;

protected:

    template<typename... TArgs>
    llvm::MDTuple* write_tuple(TArgs&&... args) {
        std::vector<llvm::Metadata*> mdvec = { __llvm_io<TArgs>::write(this, args)... };
        return llvm::MDTuple::get(_context, mdvec);
    }

    template<typename... TArgs>
    void parse_tuple(llvm::MDTuple* t, TArgs&... args) {
        std::tie(args...) = this->parse_tuple_impl<std::tuple<TArgs...>>(t, std::index_sequence_for<TArgs...>());
    }

    template<typename TClass, typename TTreeType>
    void add_write_method(llvm::MDTuple* (TClass::* mtd)(TTreeType*)) {
        auto wfunc = [=](tree_t* t) -> llvm::MDTuple* {
            auto res = (dynamic_cast<TClass*>(this)->*mtd)(t->as<TTreeType>());
            return res;
        };
        this->_write_funcs[tree_type_id_from<TTreeType>()] = writef_t(wfunc);
    }

    template<typename TClass, typename TTreeType>
    void add_read_method(TTreeType* (TClass::* mtd)(llvm::MDTuple*)) {
        auto rfunc = [=](llvm::MDTuple* t) -> tree_t* {
            auto res = (dynamic_cast<TClass*>(this)->*mtd)(t);
            return dynamic_cast<tree_t*>(res);
        };
        this->_read_funcs[tree_type_id_from<TTreeType>()] = readf_t(rfunc);
    }

    template<typename T>
    inline T parse(llvm::Metadata* md) {
        return __llvm_io<T>::parse(this, md);
    }

    template<typename T>
    inline llvm::Metadata* write(T v) {
        return __llvm_io<T>::write(this, v);
    }

private:

    template<typename>  friend struct __llvm_io;
    template<typename>  friend struct __llvm_io_int;
    template<typename>  friend struct __llvm_io_read_tree;
    template<typename>  friend struct __llvm_io_read_value_list;
    template<typename>  friend struct __llvm_io_read_tree_list;

    typedef std::function<tree_t*(llvm::MDTuple*)>                              readf_t;
    typedef std::function<llvm::MDTuple*(tree_t*)>                              writef_t;

    tree_t* read_node(llvm::Metadata* md);
    llvm::Metadata* write_node(tree_t*);

    template<typename T>
    tree_t* read_value_list(llvm::Metadata* md) {
        auto mdlist = llvm::dyn_cast<llvm::MDTuple>(md);
        auto l      = new __tree_list_value<T>();
        for(size_t i = 0; i < mdlist->getNumOperands(); i++) {
            l->append(__llvm_io<T>::parse(this, mdlist->getOperand(i).get()));
        }
        return l;
    }
    template<typename T>
    tree_t* read_tree_list(llvm::Metadata* md) {
        auto mdlist = llvm::dyn_cast<llvm::MDTuple>(md);
        auto l      = new __tree_list_tree<T>();
        for(size_t i = 0; i < mdlist->getNumOperands(); i++) {
            l->append(this->read_node(mdlist->getOperand(i).get())->as<T>());
        }
        return l;
    }

    template<typename T>
    llvm::Metadata* write_list(__tree_list_value<T>* l) {
        std::vector<llvm::Metadata*> vec;
        for(size_t i = 0; i < l->size(); i++) {
            vec.push_back(__llvm_io<T>::write(this, (*l)[i]));
        }
        return llvm::MDTuple::get(_context, vec);
    }
    template<typename T>
    llvm::Metadata* write_list(__tree_list_tree<T>* l) {
        std::vector<llvm::Metadata*> vec;
        for(auto itr = l->begin(); itr != l->end(); itr++) {
            vec.push_back(this->write_node(*itr));
        }
        return llvm::MDTuple::get(_context, vec);
    }

    template<typename TTuple, size_t... I>
    inline TTuple parse_tuple_impl(llvm::MDTuple* t, std::integer_sequence<size_t, I...>) {
        return TTuple((__llvm_io<std::tuple_element_t<I, TTuple>>::parse(this, t->getOperand(I).get()))...);
    }

    inline bool is_null(llvm::Metadata* md) {
        if(llvm::isa<llvm::ValueAsMetadata>(md)) {
            auto value = llvm::dyn_cast<llvm::ValueAsMetadata>(md)->getValue();
            if(llvm::isa<llvm::Constant>(value)) {
                return llvm::dyn_cast<llvm::Constant>(value)->isNullValue();
            }
            return false;
        }
        return false;
    }


    llvm::LLVMContext&                                                          _context;
    llvm::Value*                                                                _null_value;
    llvm::ValueAsMetadata*                                                      _null_metadata;

    std::map<tree_type_id, writef_t>                                            _write_funcs;
    std::map<tree_type_id, readf_t>                                             _read_funcs;

    std::map<tree_t*, llvm::Metadata*>                                          _written_nodes;
    std::map<llvm::Metadata*, tree_t*>                                          _read_nodes;
    std::vector<ptr<tree_t>>                                                    _managed_references;

};


#define __ivalue(md)            llvm::dyn_cast<llvm::ConstantInt>(llvm::dyn_cast<llvm::ConstantAsMetadata>(md)->getValue())
#define __fvalue(md)            llvm::dyn_cast<llvm::ConstantFP>(llvm::dyn_cast<llvm::ConstantAsMetadata>(md)->getValue())

template<>
struct __llvm_io<bool> {
    static inline bool parse(llvm_metadata* md, llvm::Metadata* n) {
        return !__ivalue(n)->isZero();
    }
    static inline llvm::Metadata* write(llvm_metadata* md, bool v) {
        if(v) { return llvm::ConstantAsMetadata::get(llvm::ConstantInt::getTrue(md->_context)); }
        else  { return llvm::ConstantAsMetadata::get(llvm::ConstantInt::getFalse(md->_context)); }
    }
};

template<typename TInt>
struct __llvm_io_int {
    static inline TInt parse(llvm_metadata* md, llvm::Metadata* n) {
        __constif(std::is_signed<TInt>::value) {
            return (TInt) __ivalue(n)->getSExtValue();
        }
        else {
            return (TInt) __ivalue(n)->getZExtValue();
        }
    }

    static inline llvm::Metadata* write(llvm_metadata* md, TInt v) {
        return llvm::ConstantAsMetadata::get(llvm::ConstantInt::get(llvm::IntegerType::get(md->_context, sizeof(TInt)), v, std::is_signed<TInt>::value));
    }
};

template<> struct __llvm_io<uint32_t> : __llvm_io_int<uint32_t> { };
template<> struct __llvm_io<uint64_t> : __llvm_io_int<uint64_t> { };
template<> struct __llvm_io<int32_t>  : __llvm_io_int<int32_t> { };
template<> struct __llvm_io<int64_t>  : __llvm_io_int<int64_t> { };

template<>
struct __llvm_io<float> {
    static inline float parse(llvm_metadata* md, llvm::Metadata* n) {
        return __fvalue(n)->getValueAPF().convertToFloat();
    }
    static inline llvm::Metadata* write(llvm_metadata* md, float f) {
        return llvm::ConstantAsMetadata::get(llvm::ConstantFP::get(llvm::Type::getFloatTy(md->_context), f));
    }
};

template<>
struct __llvm_io<double> {
    static inline double parse(llvm_metadata* md, llvm::Metadata* n) {
        return __fvalue(n)->getValueAPF().convertToDouble();
    }
    static inline llvm::Metadata* write(llvm_metadata* md, double f) {
        return llvm::ConstantAsMetadata::get(llvm::ConstantFP::get(llvm::Type::getDoubleTy(md->_context), f));
    }
};

template<>
struct __llvm_io<std::string> {
    static inline std::string parse(llvm_metadata* md, llvm::Metadata* n) {
        return llvm::dyn_cast<llvm::MDString>(n)->getString();
    }
    static inline llvm::Metadata* write(llvm_metadata* md, std::string v) {
        return llvm::MDString::get(md->_context, v);
    }
};

template<>
struct __llvm_io<nullptr_t> {
    static inline llvm::Metadata* write(llvm_metadata* md, nullptr_t) {
        return md->_null_metadata;
    }
};

template<typename T>
struct __llvm_io<__tree_property_value<T>&> {
    static inline llvm::Metadata* write(llvm_metadata* md, __tree_property_value<T>& r) {
        return __llvm_io<T>::write(md, r);
    }
};

template<typename T>
struct __llvm_io<__tree_property_tree<T>&> {
    static inline llvm::Metadata* write(llvm_metadata* md, __tree_property_tree<T>& r) {
        return md->write_node(r);
    }
};

template<typename T>
struct __llvm_io_read_tree {
    static inline T* parse(llvm_metadata* md, llvm::Metadata* n) {
        return md->read_node(n)->as<T>();
    }
};

template<typename T>
struct __llvm_io<__tree_property_list<T>&> {
    static inline llvm::Metadata* write(llvm_metadata* md, __tree_property_list<T>& r) {
        return md->write_list((typename __tree_property_list<T>::list_t*) r);
    }
};

template<typename T>
struct __llvm_io_read_value_list {
    static inline __tree_list_value<T>* parse(llvm_metadata* md, llvm::Metadata* n) {
        return md->read_value_list<T>(n)->template as<__tree_list_value<T>>();
    }
};

template<typename T>
struct __llvm_io_read_tree_list {
    static inline __tree_list_tree<T>* parse(llvm_metadata* md, llvm::Metadata* n) {
        return md->read_tree_list<T>(n)->template as<__tree_list_tree<T>>();
    }
};

template<typename T> struct __llvm_io<__tree_list_value<T>*> : __llvm_io_read_value_list<T> { };
template<typename T> struct __llvm_io<__tree_list_tree<T>*>  : __llvm_io_read_tree_list<T>  { };
//TODO: more strict check that T is a tree_type
template<typename T> struct __llvm_io<T*>                    : __llvm_io_read_tree<T>       { };


#undef __ivalue
#undef __fvalue
}

#endif /* XCC_LLVM_METADATA_HPP_ */
