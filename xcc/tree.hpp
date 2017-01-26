/*
 * tree.hpp
 *
 *  Created on: Jan 19, 2017
 *      Author: derick
 */

#ifndef XCC_TREE_HPP_
#define XCC_TREE_HPP_

#include <cassert>
#include <cstdint>
#include <cstdlib>

//#include <algorithm>
#include <initializer_list>
#include <functional>
#include <map>
//#include <memory>
#include <type_traits>
//#include <tuple>
//#include <list>
#include <vector>

#include "managed_ptr.hpp"


namespace xcc {

template<typename T>
using ptr = managed_ptr<T>;

template<typename T> inline ptr<T>   __box(T*& v)       { return ptr<T>(v); }
template<typename T> inline T*       __unbox(ptr<T> p)  { return (T*)p;     }


/* ============================= *
 * Forward declarations and tags *
 * ============================= */

struct tree_type;
struct __tree_base;
struct __tree_property_base;
template<typename> struct __tree_list_base;
template<typename> struct __tree_list_tree;
template<typename> struct __tree_list_value;


/* ==================== *
 * Type Trait Utilities *
 * ==================== */

template<typename T>
struct __is_tree : std::is_base_of<__tree_base, T> { };

template<typename T>
using __is_tree_type = typename std::enable_if<__is_tree<T>::value, int>::type;

template<typename T> struct __list_type_selector {
    typedef typename std::conditional<std::is_base_of<__tree_base, T>::value,
                            __tree_list_tree<T>,
                            __tree_list_value<T>>::type         type;
};

template<typename> struct __tree_property_tree;
template<typename> struct __tree_property_list;
template<typename> struct __tree_property_value;

template<typename T> struct __property_type_tree_selector {
    typedef __tree_property_tree<T>                             type;
};

template<typename T> struct __property_type_tree_selector<__tree_list_tree<T>> {
    typedef __tree_property_list<T>                             type;
};

template<typename T> struct __property_type_tree_selector<__tree_list_value<T>> {
    typedef __tree_property_list<T>                             type;
};

template<typename T> struct __property_type_other_selector {
    typedef __tree_property_value<T>                            type;
};

template<typename T> struct __property_type_other_selector<__tree_list_base<T>> {
    typedef __tree_property_tree<T>                              type;
};

template<typename T> struct __property_type_selector {
    typedef typename std::conditional<std::is_base_of<__tree_base, T>::value,
                            typename __property_type_tree_selector<T>::type,
                            typename __property_type_other_selector<T>::type>::type type;
};



/* ==================== *
 * Tree Type Management *
 * ==================== */

extern const tree_type __all_tree_types[];


enum class tree_type_id : uint64_t {
    tree,
#ifdef TREE_TYPE
#error "TREE_TYPE Already defined"
#endif
#define TREE_TYPE(name, ...)                                    name,
#include "all_tree_types.def.hpp"
#undef  TREE_TYPE
    __type_count
};


struct tree_type {
    tree_type_id                                                id;
    tree_type_id                                                base_id;
    const char*                                                 name;

    inline static constexpr size_t count() noexcept { return (size_t)tree_type_id::__type_count; }
    inline static           bool   is_base_of(const tree_type_id bt, const tree_type_id dt) noexcept {
        tree_type_id tt = dt;
        while(tt != tree_type_id::tree) {
            if(bt == tt) {
                return true;
            }
            tt = __all_tree_types[(size_t)tt].base_id;
        }
        return false;
    }
};




/* =============== *
 * Tree Base Class *
 * =============== */

/** Base type for all tree types */
struct __tree_base {
public:

    inline __tree_base()                noexcept : _type(tree_type_id::tree) { }
    inline __tree_base(tree_type_id id) noexcept : _type(id)                 { }
    virtual ~__tree_base() = default;

    inline bool is(const tree_type_id tp) const noexcept { return tree_type::is_base_of(tp, this->_type); }
    template<typename T, __is_tree_type<T> = 0>
    inline bool is() const noexcept { return this->is(T::type_id); }

    template<typename T, __is_tree_type<T> = 0>
    inline T* as() const noexcept { return dynamic_cast<T*>(const_cast<__tree_base*>(this)); }

    inline void pin() noexcept {
        //TODO: memory management
    }
    inline tree_type_id get_tree_type() const { return this->_type; }

private:

    __tree_base(const __tree_base&) = delete;
    __tree_base(const __tree_base&&) = delete;

    friend __tree_property_base;

    inline size_t append_child(__tree_base* v) noexcept {
        size_t idx = this->_child_nodes.size();
        this->_child_nodes.push_back(ptr<__tree_base>(v));
        return idx;
    }

    const tree_type_id                                          _type;
    std::vector<ptr<__tree_base>>                               _child_nodes;

};

template<tree_type_id VType, typename TBase = __tree_base, __is_tree_type<TBase> = 0>
struct __extend_tree : public TBase {
public:

    typedef __extend_tree<VType, TBase>                         base_type;
    static const tree_type_id                                   type_id = VType;

    template<typename... TArgs>
    inline __extend_tree(TArgs... args): TBase(VType, args...) { }
    template<typename... TArgs>
    inline __extend_tree(tree_type_id tp, TArgs... args): TBase(tp, args...) { }

};

template<typename T, __is_tree_type<T> = 0>
constexpr tree_type_id __get_tree_type_id() { return T::type_id; }


template<typename TElement>
struct __tree_list_base : public __tree_base {
protected:

    typedef          __tree_list_base<TElement>                 list_t;
    typedef          list_t*                                    list_ptr_t;

    inline __tree_list_base()                                          noexcept : __tree_base() { }
    inline __tree_list_base(TElement f)                                noexcept : __tree_base() { this->_list.push_back(f); }
    inline __tree_list_base(TElement f, const list_ptr_t r)            noexcept : __tree_base() { this->_list.push_back(f); this->append_list(r); }
    inline __tree_list_base(const list_ptr_t f, TElement l)            noexcept : __tree_base() { this->append_list(f); this->_list.push_back(l); }
    inline __tree_list_base(const list_ptr_t f)                        noexcept : __tree_base() { this->append_list(f); }

    void append_list(const list_ptr_t other) noexcept {
        for(auto el: other->_list) {
            this->_list.push_back(el);
        }
    }

public:

    inline typename std::vector<TElement>::size_type size() const noexcept {
        return this->_list.size();
    }

protected:

    std::vector<TElement>                                       _list;

};

template<typename TElement>
struct __tree_list_value final : public __tree_list_base<TElement> {
protected:

    typedef          __tree_list_base<TElement>                 list_t;
    typedef          list_t*                                    list_ptr_t;

public:

    inline __tree_list_value()                                          noexcept : list_t()     { }
    inline __tree_list_value(TElement f)                                noexcept : list_t(f)    { }
    inline __tree_list_value(TElement f, const list_ptr_t r)            noexcept : list_t(f, r) { }
    inline __tree_list_value(const list_ptr_t f, TElement l)            noexcept : list_t(f, l) { }
    inline __tree_list_value(std::initializer_list<TElement> init_list) noexcept : list_t()     {
        for(auto el: init_list) {
            this->_list.push_back(el);
        }
    }

    inline TElement& operator[](size_t index) noexcept {
        return this->_list[index];
    }

};

template<typename TElement>
struct __tree_list_tree final : public __tree_list_base<ptr<TElement>> {
protected:

    typedef __tree_list_base<ptr<TElement>>             list_t;
    typedef list_t*                                     list_ptr_t;
    //typedef ptr<TElement>                               box_t;


public:

    typedef TElement*                                   element_t;

    struct iterator {
        size_t                                  index;
        std::vector<ptr<TElement>>&             vector;

        inline iterator operator++()          noexcept { ++index; return *this; }
        inline iterator operator++(int)       noexcept { ++index; return {index-1, vector}; }

        inline element_t operator*()          noexcept { return __unbox(vector[index]); }
        inline element_t operator*()    const noexcept { return __unbox(vector[index]); }

        inline bool operator==(const iterator& other) {
            return (other.index == this->index) && (other.vector == this->vector);
        }

        inline bool operator!=(const iterator& other) {
            return (other.index != this->index) || (other.vector != this->vector);
        }
    };
    typedef const iterator                              const_iterator;

    inline __tree_list_tree()                                           noexcept : list_t()            { }
    inline __tree_list_tree(element_t f)                                noexcept : list_t(__box(f))    { }
    inline __tree_list_tree(element_t f, const list_ptr_t r)            noexcept : list_t(__box(f), r) { }
    inline __tree_list_tree(const list_ptr_t f, element_t l)            noexcept : list_t(f, __box(l)) { }
    inline __tree_list_tree(std::initializer_list<element_t> init_list) noexcept : list_t() {
        for(auto el : init_list) {
            this->_list.push_back(__box(el));
        }
    }


    inline       iterator begin()       noexcept { return {0,                  this->_list}; }
    inline       iterator end()         noexcept { return {this->_list.size(), this->_list}; }
    inline const_iterator begin() const noexcept { return {0,                  this->_list}; }
    inline const_iterator end()   const noexcept { return {this->_list.size(), this->_list}; }

    inline element_t operator[](size_t index) {
        return __unbox(this->_list[index]);
    }


};





/* =============== *
 * Property Access *
 * =============== */

template<typename T>
struct dispatch_visitor;

struct __tree_property_base {
public:

    inline __tree_property_base(__tree_base* parent, __tree_base* value) noexcept
            : _parent(parent), _index(parent->append_child(value)) { }
    inline __tree_property_base(__tree_base* parent) noexcept
            : _parent(parent), _index(parent->append_child(nullptr)) { }

    inline __tree_base* __get()                   const noexcept { return __unbox(this->_parent->_child_nodes[this->_index]); }
    inline void         __set(__tree_base* value)       noexcept {        this->_parent->_child_nodes[this->_index] = __box<__tree_base>(value); }

private:

    __tree_property_base(const __tree_property_base&) = delete;
    __tree_property_base(const __tree_property_base&&) = delete;

    __tree_base*                                                _parent;
    size_t                                                      _index;

};

template<typename TTreeType>
struct __tree_property_tree : public __tree_property_base {
public:

    typedef TTreeType*                                          param_type_t;

    inline __tree_property_tree(__tree_base* parent)                   noexcept : __tree_property_base(parent)        { }
    inline __tree_property_tree(__tree_base* parent, TTreeType* value) noexcept : __tree_property_base(parent, value) { }

    inline operator TTreeType*() const noexcept {
        return dynamic_cast<TTreeType*>(this->__get());
    }

    template<typename TTargetType, typename std::enable_if<std::is_base_of<TTreeType, TTargetType>::value, int>::type = 0>
    inline operator TTargetType*() const noexcept {
        return dynamic_cast<TTargetType*>(this->__get());
    }

    inline decltype(nullptr) operator=(decltype(nullptr) null) {
        this->__set(null);
        return null;
    }

    template<typename TFromType, typename std::enable_if<std::is_base_of<TTreeType, TFromType>::value, int>::type = 0>
    inline TFromType* operator=(TFromType* value) noexcept {
        this->__set(value);
        return value;
    }

    inline TTreeType* operator->() const noexcept {
        return dynamic_cast<TTreeType*>(this->__get());
    }

    inline bool operator==(decltype(nullptr)) {
        return this->__get() == nullptr;
    }
    template<typename TOther, __is_tree_type<TOther> = 0>
    inline bool operator==(const __tree_property_tree<TOther>& p) {
        return this->__get() == p.get();
    }
};

template<typename TTreeListElement>
struct __tree_property_list final : public __tree_property_tree<typename __list_type_selector<TTreeListElement>::type> {
public:

    typedef typename __list_type_selector<TTreeListElement>::type list_t;
    typedef typename list_t::element_t                            element_t;

    inline __tree_property_list(__tree_base* parent)                noexcept : __tree_property_tree<list_t>(parent) { }
    inline __tree_property_list(__tree_base* parent, list_t* value) noexcept : __tree_property_tree<list_t>(parent, value) { }

    inline element_t operator[](size_t index) const noexcept {
        return (**this)[index];
    }

    inline typename list_t::      iterator begin()       noexcept { return dynamic_cast<list_t*>(this->__get())->begin(); }
    inline typename list_t::      iterator end()         noexcept { return dynamic_cast<list_t*>(this->__get())->end();   }
    inline typename list_t::const_iterator begin() const noexcept { return dynamic_cast<list_t*>(this->__get())->begin(); }
    inline typename list_t::const_iterator end()   const noexcept { return dynamic_cast<list_t*>(this->__get())->end();   }

};

template<typename TValue>
struct __tree_property_value {
public:

    typedef const TValue&                                       param_type_t;

    inline __tree_property_value(__tree_base*)                      noexcept : _value()      { }
    inline __tree_property_value(__tree_base*, const TValue& value) noexcept : _value(value) { }

    inline operator const TValue&() const noexcept {
        return this->_value;
    }

    template<typename T, typename std::enable_if<std::is_convertible<TValue, T>::value, int>::type = 0>
    inline operator T() const noexcept {
        return (T)this->_value;
    }

    inline const TValue& operator=(const TValue& v) {
        this->_value = v;
        return v;
    }

    inline TValue* operator->() {
        return &this->_value;
    }

private:

    TValue                                                      _value;

};



/* =============================== *
 * Dynamic dispatch tree traversal *
 * =============================== */

template<typename TReturnType>
struct dispatch_visitor {
public:

    dispatch_visitor() = default;
    virtual ~dispatch_visitor() = default;

    inline TReturnType visit(__tree_base* t) {
        if(t == nullptr) {
            return this->handle_null_tree();
        }
        tree_type_id id = t->get_tree_type();
        if(this->_function_map.find(id) == this->_function_map.end()) {
            return this->handle_unregisterd_tree_type(id);
        }
        return this->_function_map[id](t);
    }
    template<typename T>
    inline TReturnType visit(__tree_property_tree<T>& p) {
        __tree_base* t = p.__get();
        return this->visit(t);
    }

protected:

    template<typename TFuncReturnType,
             typename TClassType,
             typename TTreeType,
             typename std::enable_if<std::is_convertible<TFuncReturnType, TReturnType>::value, int>::type = 0,
             typename std::enable_if<std::is_base_of<dispatch_visitor<TReturnType>, TClassType>::value, int>::type = 0,
             __is_tree_type<TTreeType> = 0>
    using dispatch_method_type = TFuncReturnType(TClassType::*)(TTreeType*);
    template<typename TFuncReturnType,
             typename TTreeType,
             typename std::enable_if<std::is_convertible<TFuncReturnType, TReturnType>::value, int>::type = 0,
             __is_tree_type<TTreeType> = 0>
    using dispatch_function_type = TFuncReturnType(*)(TTreeType*);

    template<typename TFuncReturnType, typename TClassType, typename TTreeType>
    void addmethod(dispatch_method_type<TFuncReturnType, TClassType, TTreeType> mtd) {
        auto func = std::bind(mtd, dynamic_cast<TClassType*>(this), std::placeholders::_1);
        auto wrapf = [=](__tree_base* t) -> TReturnType {
            return (TReturnType) func(dynamic_cast<TTreeType*>(t));
        };
        this->_function_map[__get_tree_type_id<TTreeType>()] = wrapf;
    }

    virtual TReturnType handle_null_tree() {
        throw std::runtime_error("Null tree in dispatch handler");
    }
    virtual TReturnType handle_unregisterd_tree_type(tree_type_id id) {
        throw std::runtime_error(std::string("No registered function for type id ") + __all_tree_types[(uint64_t) id].name);
    }

private:

    std::map<tree_type_id, std::function<TReturnType(__tree_base*)>>
                                                                _function_map;

};


template<tree_type_id tp, typename base = __tree_base>
using extend_tree = __extend_tree<tp, base>;

template<typename T>
using property = typename __property_type_selector<T>::type;

template<typename T>
using list = typename __list_type_selector<T>::type;


template<typename T> inline typename __tree_list_tree<T>::      iterator begin(__tree_list_tree<T>* lptr)       noexcept { return lptr->begin(); }
template<typename T> inline typename __tree_list_tree<T>::      iterator end(__tree_list_tree<T>* lptr)         noexcept { return lptr->end();   }
template<typename T> inline typename __tree_list_tree<T>::const_iterator begin(const __tree_list_tree<T>* lptr) noexcept { return lptr->begin(); }
template<typename T> inline typename __tree_list_tree<T>::const_iterator end(const __tree_list_tree<T>* lptr)   noexcept { return lptr->end();   }

}

#endif /* XCC_TREE_HPP_ */
