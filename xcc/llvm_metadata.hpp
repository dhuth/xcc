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
#include "source.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/Metadata.h>
#include <llvm/ADT/APInt.h>
#include <llvm/ADT/APSInt.h>
#include <llvm/ADT/APFloat.h>

#include <map>
#include <tuple>
#include <utility>
#include <vector>

namespace xcc {

struct llvm_metadata_reader;
struct llvm_metadata_writer;

template<typename T> struct __llvm_io_reader {
    static inline T read(llvm::Metadata*, llvm_metadata_reader&);
};
template<typename T> struct __llvm_io_writer {
    static inline llvm::Metadata* write(T, llvm_metadata_writer&);
};



/* ------------------------ *
 * LLVM metadata base class *
 * ------------------------ */

struct __llvm_metadata_io_base {
public:

    explicit inline __llvm_metadata_io_base(llvm::LLVMContext& llvm_context) noexcept
            : llvm_context(llvm_context),
              _null(llvm::ConstantAsMetadata::get(llvm::Constant::getNullValue(llvm::Type::getInt64Ty(this->llvm_context)))),
              _false(llvm::ConstantAsMetadata::get(llvm::ConstantInt::getFalse(this->llvm_context))),
              _true(llvm::ConstantAsMetadata::get(llvm::ConstantInt::getTrue(this->llvm_context))) {
        //...
    }

    bool                        is_null(llvm::Metadata*) const noexcept;
    llvm::Metadata*             get_null() const noexcept;
    llvm::Metadata*             get_true() const noexcept;
    llvm::Metadata*             get_false() const noexcept;

    llvm::LLVMContext&                                      llvm_context;

private:

    llvm::Metadata*                                         _null;
    llvm::Metadata*                                         _false;
    llvm::Metadata*                                         _true;

};



/* -------------------------- *
 * LLVM metadata reader class *
 * -------------------------- */

struct llvm_metadata_reader : public __llvm_metadata_io_base {
public:

    explicit inline llvm_metadata_reader(llvm::LLVMContext& llvm_context) noexcept
            : __llvm_metadata_io_base(llvm_context) {
        // do nohting
    }

protected:

    template<typename _TClass,
             typename _TReturnType>
    void addmethod(_TReturnType*(_TClass::* mtd)(llvm::MDTuple*)) noexcept {
        static_assert(is_tree<_TReturnType>::value, "return type must be a pointer to a tree node");

        auto rfunc = [=](llvm::MDTuple* t)->tree_t* {
            return (static_cast<_TClass*>(this)->*mtd)(t);
        };
        _read_functions[tree_type_id_from<_TReturnType>()] = rfunc;
    }

    template<typename... T>
    void read_tuple(llvm::Metadata* md, T&... args) noexcept {
        auto md_tuple = llvm::dyn_cast<llvm::MDTuple>(md);
        std::tie(args...) = this->__read_tuple_impl<std::tuple<T...>>(md_tuple, std::index_sequence_for<T...>());
    }

    template<typename T>
    inline T read(llvm::Metadata* md) noexcept {
        return __llvm_io_reader<T>::read(md, *this);
    }

private:

    typedef std::function<tree_t*(llvm::MDTuple*)>          readf;

    template<typename> friend struct __llvm_io_reader;
    template<typename> friend struct __llvm_io_tree_reader;

    template<size_t I, typename T>
    inline T __read_tuple_item_impl(llvm::MDTuple* md_tuple) noexcept {
        return this->read<T>(md_tuple->getOperand(I).get());
    }

    template<typename _Tuple, size_t... I>
    inline _Tuple __read_tuple_impl(llvm::MDTuple* md_tuple, std::integer_sequence<size_t, I...>) {
        return _Tuple(this->__read_tuple_item_impl<I, std::tuple_element_t<I, _Tuple>>(md_tuple)...);
    }

    tree_t* read_node(llvm::Metadata*) noexcept;

    template<typename T>
    list<T>* read_list(llvm::Metadata* md) {
        auto md_tuple = llvm::dyn_cast<llvm::MDTuple>(md);
        list<T>* olist = new list<T>();
        for(size_t i = 0; i < md_tuple->getNumOperands(); i++) {
            olist->append(this->read<typename list<T>::element_t>(md_tuple->getOperand(i).get()));
        }
        return olist;
    }

    std::map<tree_type_id, readf>                           _read_functions;

};



/* -------------------------- *
 * LLVM metadata writer class *
 * -------------------------- */

struct llvm_metadata_writer : public __llvm_metadata_io_base {
public:

    explicit inline llvm_metadata_writer(llvm::LLVMContext& llvm_context) noexcept
            : __llvm_metadata_io_base(llvm_context) {
        // do nothing
    }

protected:

    template<typename _TClass,
             typename _TTreeType>
    void addmethod(llvm::MDTuple*(_TClass::* mtd)(_TTreeType*)) noexcept {
        static_assert(is_tree<_TTreeType>::value, "argument to method must a pointer to a tree node");

        auto wfunc = [=](tree_t* t)->llvm::MDTuple* {
            //TODO: maybe handle null tree?
            return (static_cast<_TClass*>(this)->*mtd)(t->as<_TTreeType>());
        };
        _write_functions[tree_type_id_from<_TTreeType>()];
    }

    template<typename... T>
    llvm::MDTuple* write_tuple(T&&... args) noexcept {
        std::vector<llvm::Metadata*> tvec = { this->write(std::forward<T>(args))... };
        return llvm::MDTuple::get(this->llvm_context, tvec);
    }

    template<typename T>
    inline llvm::Metadata* write(T& v) noexcept {
        return __llvm_io_writer<typename std::remove_reference<T>::type>::write(v, *this);
    }

private:

    typedef std::function<llvm::MDTuple*(tree_t*)>          writef;

    template<typename> friend struct __llvm_io_writer;
    template<typename> friend struct __llvm_io_tree_writer;

    llvm::Metadata* write_node(tree_t* t) noexcept;

    template<typename T>
    llvm::Metadata* write_list(__tree_list_tree<T>* tlist) noexcept {
        std::vector<llvm::Metadata*> vec;
        for(auto t: (*tlist)) {
            vec.push_back(this->write(t));
        }
        return llvm::MDTuple::get(this->llvm_context, vec);
    }

    std::map<tree_type_id, writef>                          _write_functions;
};



/* ------------ *
 * LLVM readers *
 * ------------ */

#define __as_const_metadata_value(md)                       llvm::dyn_cast<llvm::ConstantAsMetadata>(md)
#define __get_const_t_value(t, cmd)                         llvm::dyn_cast<t>(cmd->getValue())
#define __get_api_value(md)                                 __get_const_t_value(llvm::ConstantInt, __as_const_metadata_value(md))->getValue()
#define __get_apsi_value(md)                 (llvm::APSInt) __get_const_t_value(llvm::ConstantInt, __as_const_metadata_value(md))->getValue()
#define __get_apf_value(md)                                 __get_const_t_value(llvm::ConstantFP,  __as_const_metadata_value(md))->getValueAPF()

template<> struct __llvm_io_reader<llvm::APInt> {
    static inline llvm::APInt read(llvm::Metadata* md, llvm_metadata_reader&) {
        return __get_api_value(md);
    }
};

template<> struct __llvm_io_reader<llvm::APSInt> {
    static inline llvm::APSInt read(llvm::Metadata* md, llvm_metadata_reader&) {
        return __get_apsi_value(md);
    }
};

template<> struct __llvm_io_reader<llvm::APFloat> {
    static inline llvm::APFloat read(llvm::Metadata* md, llvm_metadata_reader&) {
        return __get_apf_value(md);
    }
};

template<> struct __llvm_io_reader<std::string> {
    static inline std::string read(llvm::Metadata* md, llvm_metadata_reader&) {
        return llvm::dyn_cast<llvm::MDString>(md)->getString();
    }
};

template<typename T> struct __llvm_io_int_reader {
    static inline T read(llvm::Metadata* md, llvm_metadata_reader&) {
        if(std::is_signed<T>::value) {
            return (T) __get_api_value(md).getSExtValue();
        }
        else {
            return (T) __get_api_value(md).getZExtValue();
        }
    }
};

template<> struct __llvm_io_reader< uint8_t> : public __llvm_io_int_reader< uint8_t> { };
template<> struct __llvm_io_reader<uint16_t> : public __llvm_io_int_reader<uint16_t> { };
template<> struct __llvm_io_reader<uint32_t> : public __llvm_io_int_reader<uint32_t> { };
template<> struct __llvm_io_reader<uint64_t> : public __llvm_io_int_reader<uint64_t> { };
template<> struct __llvm_io_reader<  int8_t> : public __llvm_io_int_reader<  int8_t> { };
template<> struct __llvm_io_reader< int16_t> : public __llvm_io_int_reader< int16_t> { };
template<> struct __llvm_io_reader< int32_t> : public __llvm_io_int_reader< int32_t> { };
template<> struct __llvm_io_reader< int64_t> : public __llvm_io_int_reader< int64_t> { };

template<> struct __llvm_io_reader<float> {
    static inline float read(llvm::Metadata* md, llvm_metadata_reader&) {
        return __get_apf_value(md).convertToFloat();
    }
};

template<> struct __llvm_io_reader<double> {
    static inline double read(llvm::Metadata* md, llvm_metadata_reader&) {
        return __get_apf_value(md).convertToDouble();
    }
};

template<> struct __llvm_io_reader<bool> {
    static inline bool read(llvm::Metadata* md, llvm_metadata_reader& r) {
        return !__get_const_t_value(llvm::Constant, __as_const_metadata_value(md))->isZeroValue();
    }
};

template<> struct __llvm_io_reader<source_location> {
    static inline source_location read(llvm::Metadata* md, llvm_metadata_reader& r) {
        std::string             filename;
        uint32_t                line;
        uint32_t                column;

        r.read_tuple(md, filename, line, column);
        source_location         rvalue;
        rvalue.filename         = filename;
        rvalue.line_number      = line;
        rvalue.column_number    = column;
        return rvalue;
    }
};

template<> struct __llvm_io_reader<source_span> {
    static inline source_span read(llvm::Metadata* md, llvm_metadata_reader& r) {
        source_location     beg;
        source_location     end;

        r.read_tuple(md, beg, end);
        return { beg, end };
    }
};

template<> struct __llvm_io_reader<tree_type_id> {
    static inline tree_type_id read(llvm::Metadata* md, llvm_metadata_reader& r) {
        return (tree_type_id) __llvm_io_reader<uint64_t>::read(md, r);
    }
};

template<typename T>
struct __llvm_io_tree_reader {
    static inline T* read(llvm::Metadata* md, llvm_metadata_reader& r) {
        if(!r.is_null(md)) {
            return r.read_node(md)->as<T>();
        }
        return nullptr;
    }
};

template<typename T>
struct __llvm_io_tree_reader<__tree_list_value<T>> {
    static inline __tree_list_value<T>* read(llvm::Metadata* md, llvm_metadata_reader& r) {
        return r.read_list<T>(md);
    }
};

template<typename T>
struct __llvm_io_tree_reader<__tree_list_tree<T>> {
    static inline __tree_list_tree<T>* read(llvm::Metadata* md, llvm_metadata_reader& r) {
        return r.read_list<T>(md);
    }
};

template<typename T> struct __llvm_io_reader<T*> : public __llvm_io_tree_reader<T> { };



/* ------------ *
 * LLVM writers *
 * ------------ */

#define __mk_const_api(v, ctx)                              llvm::ConstantInt::get(ctx, v)
#define __mk_const_apsi(v, ctx)                             llvm::ConstantInt::get(ctx, v)
#define __mk_const_apf(v, ctx)                              llvm::ConstantFP::get(ctx, v)
#define __mk_const_as_md(c)                                 llvm::ConstantAsMetadata::get(c)

template<> struct __llvm_io_writer<llvm::APInt> {
    static inline llvm::Metadata* write(llvm::APInt v, llvm_metadata_writer& w) {
        return __mk_const_as_md(__mk_const_api(v, w.llvm_context));
    }
};

template<> struct __llvm_io_writer<llvm::APSInt> {
    static inline llvm::Metadata* write(llvm::APSInt v, llvm_metadata_writer& w) {
        return __mk_const_as_md(__mk_const_apsi(v, w.llvm_context));
    }
};

template<> struct __llvm_io_writer<llvm::APFloat> {
    static inline llvm::Metadata* write(llvm::APFloat v, llvm_metadata_writer& w) {
        return __mk_const_as_md(__mk_const_apf(v, w.llvm_context));
    }
};

template<> struct __llvm_io_writer<std::string> {
    static inline llvm::Metadata* write(std::string v, llvm_metadata_writer& w) {
        return llvm::MDString::get(w.llvm_context, v);
    }
};

template<typename T> struct __llvm_io_int_writer {
    static inline llvm::Metadata* write(T v, llvm_metadata_writer& w) {
        if(std::is_unsigned<T>::value) {
            return __mk_const_as_md(llvm::ConstantInt::get(llvm::IntegerType::get(w.llvm_context, sizeof(T)), (uint64_t)v, false));
        }
        else {
            return __mk_const_as_md(llvm::ConstantInt::getSigned(llvm::IntegerType::get(w.llvm_context, sizeof(T)), (int64_t)v));
        }
    }
};

template<> struct __llvm_io_writer< uint8_t> : public __llvm_io_int_writer< uint8_t> { };
template<> struct __llvm_io_writer<uint16_t> : public __llvm_io_int_writer<uint16_t> { };
template<> struct __llvm_io_writer<uint32_t> : public __llvm_io_int_writer<uint32_t> { };
template<> struct __llvm_io_writer<uint64_t> : public __llvm_io_int_writer<uint64_t> { };
template<> struct __llvm_io_writer<  int8_t> : public __llvm_io_int_writer<  int8_t> { };
template<> struct __llvm_io_writer< int16_t> : public __llvm_io_int_writer< int16_t> { };
template<> struct __llvm_io_writer< int32_t> : public __llvm_io_int_writer< int32_t> { };
template<> struct __llvm_io_writer< int64_t> : public __llvm_io_int_writer< int64_t> { };

template<> struct __llvm_io_writer<float> {
    static inline llvm::Metadata* write(float v, llvm_metadata_writer& w) {
        auto fvalue = llvm::ConstantFP::get(llvm::Type::getFloatTy(w.llvm_context), v);
        return __mk_const_as_md(fvalue);
    }
};

template<> struct __llvm_io_writer<double> {
    static inline llvm::Metadata* write(double v, llvm_metadata_writer& w) {
        auto dvalue = llvm::ConstantFP::get(llvm::Type::getDoubleTy(w.llvm_context), v);
        return __mk_const_as_md(dvalue);
    }
};

template<> struct __llvm_io_writer<bool> {
    static inline llvm::Metadata* write(bool v, llvm_metadata_writer& w) {
        if(v)       return w.get_true();
        else        return w.get_false();
    }
};

template<> struct __llvm_io_writer<tree_type_id> {
    static inline llvm::Metadata* write(tree_type_id i, llvm_metadata_writer& w) {
        return __llvm_io_writer<uint64_t>::write((uint64_t) i, w);
    }
};

template<> struct __llvm_io_writer<source_location> {
    static inline llvm::Metadata* write(source_location l, llvm_metadata_writer& w) {
        return w.write_tuple(l.filename, l.line_number, l.column_number);
    }
};

template<> struct __llvm_io_writer<source_span> {
    static inline llvm::Metadata* write(source_span s, llvm_metadata_writer& w) {
        return w.write_tuple(s.first, s.last);
    }
};

template<> struct __llvm_io_writer<nullptr_t> {
    static inline llvm::Metadata* write(nullptr_t, llvm_metadata_writer& w) {
        return w.get_null();
    }
};

template<typename T>
struct __llvm_io_tree_writer {
    static inline llvm::Metadata* write(T* t, llvm_metadata_writer& w) {
        if(t == nullptr) {
            return w.get_null();
        }
        return w.write_node(t);
    }
};

template<typename T> struct __llvm_io_writer<T*> : public __llvm_io_tree_writer<T> { };

template<typename T>
struct __llvm_io_writer<__tree_property_value<T>> {
    static inline llvm::Metadata* write(__tree_property_value<T>& p, llvm_metadata_writer& w) {
        return __llvm_io_writer<T>::write((T) p, w);
    }
};

template<typename T>
struct __llvm_io_writer<__tree_property_tree<T>> {
    static inline llvm::Metadata* write(__tree_property_tree<T>& p, llvm_metadata_writer& w) {
        return __llvm_io_tree_writer<T>::write((T*) p, w);
    }
};

template<typename T>
struct __llvm_io_writer<__tree_property_list<T>> {
    typedef typename __tree_property_list<T>::list_t        list_t;
    static inline llvm::Metadata* write(__tree_property_list<T>& p, llvm_metadata_writer& w) {
        return w.write_list((list_t*) p);
    }
};

#undef  __as_const_metadata_value
#undef  __get_const_t_value
#undef  __get_api_value
#undef  __get_apsi_value
#undef  __get_apf_value
#undef  __mk_const_api
#undef  __mk_const_apsi
#undef  __mk_const_apf
#undef  __mk_const_as_md

}

#endif /* XCC_LLVM_METADATA_HPP_ */
