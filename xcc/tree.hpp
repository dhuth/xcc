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

#include <algorithm>
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

#define TREE_TYPE(name, ...)                                    struct name;
#include "all_tree_types.def.hpp"
#undef  TREE_TYPE

template<tree_type_id id> struct tree_type_selector { };
#define TREE_TYPE(name, ...)                                    template<> struct tree_type_selector<tree_type_id::name> { typedef name type; };
#include "all_tree_types.def.hpp"
#undef  TREE_TYPE



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
    inline tree_type_id get_tree_type() const { return this->_type; }

    inline const char* get_tree_type_name() const { return __all_tree_types[(size_t) this->_type].name; };

    inline __tree_base(const __tree_base& other) noexcept
            : _type(other._type) {
        for(auto p: other._child_nodes) {
            this->_child_nodes.push_back(p);
        }
    }

    inline void pin() const noexcept {
        box(this).pin();
    }

private:

    template<typename T>
    friend struct __tree_property_tree;

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
struct __tree_list_base : public __extend_tree<tree_type_id::tree_list, __tree_base> {
protected:

    typedef          __tree_list_base<TElement>                 list_t;
    typedef          list_t*                                    list_ptr_t;

    inline __tree_list_base()                                          noexcept : base_type() { }
    inline __tree_list_base(TElement f)                                noexcept : base_type() { this->_list.push_back(f); }
    inline __tree_list_base(TElement f, const list_ptr_t r)            noexcept : base_type() { this->_list.push_back(f); this->append_list(r); }
    inline __tree_list_base(const list_ptr_t f, TElement l)            noexcept : base_type() { this->append_list(f); this->_list.push_back(l); }
    inline __tree_list_base(const list_ptr_t f)                        noexcept : base_type() { this->append_list(f); }

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
    inline __tree_list_value(std::vector<TElement> vec)                 noexcept : list_t() {
        for(auto el: vec) {
            this->_list.push_back(el);
        }
    }
    inline __tree_list_value(std::initializer_list<TElement> init_list) noexcept : list_t()     {
        for(auto el: init_list) {
            this->_list.push_back(el);
        }
    }

    inline TElement& operator[](size_t index) noexcept {
        return this->_list[index];
    }

    inline void append(TElement& el) noexcept {
        this->_list.push_back(el);
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

        inline element_t operator*()          noexcept { return unbox(vector[index]); }
        inline element_t operator*()    const noexcept { return unbox(vector[index]); }

        inline bool operator==(const iterator& other) {
            return (other.index == this->index) && (other.vector == this->vector);
        }

        inline bool operator!=(const iterator& other) {
            return (other.index != this->index) || (other.vector != this->vector);
        }
    };
    typedef const iterator                              const_iterator;

    inline __tree_list_tree()                                           noexcept : list_t()            { }
    inline __tree_list_tree(element_t f)                                noexcept : list_t(box(f))      { }
    inline __tree_list_tree(element_t f, const list_ptr_t r)            noexcept : list_t(box(f), r)   { }
    inline __tree_list_tree(const list_ptr_t f, element_t l)            noexcept : list_t(f, box(l))   { }
    inline __tree_list_tree(std::vector<element_t> vec)                 noexcept : list_t() {
        for(auto el : vec) {
            this->_list.push_back(box(el));
        }
    }
    inline __tree_list_tree(std::initializer_list<element_t> init_list) noexcept : list_t() {
        for(auto el : init_list) {
            this->_list.push_back(box(el));
        }
    }


    inline       iterator begin()       noexcept { return {0,                  this->_list}; }
    inline       iterator end()         noexcept { return {this->_list.size(), this->_list}; }
    inline const_iterator begin() const noexcept { return {0,                  this->_list}; }
    inline const_iterator end()   const noexcept { return {this->_list.size(), this->_list}; }

    inline element_t operator[](size_t index) {
        return unbox(this->_list[index]);
    }

    inline void append(element_t& el) {
        this->_list.push_back(box(el));
    }

    inline void append(element_t&& el) {
        this->_list.push_back(box(el));
    }


};

template<typename T> inline typename __tree_list_tree<T>::      iterator begin(__tree_list_tree<T>* lptr)       noexcept { return lptr->begin(); }
template<typename T> inline typename __tree_list_tree<T>::      iterator end(__tree_list_tree<T>* lptr)         noexcept { return lptr->end();   }
template<typename T> inline typename __tree_list_tree<T>::const_iterator begin(const __tree_list_tree<T>* lptr) noexcept { return lptr->begin(); }
template<typename T> inline typename __tree_list_tree<T>::const_iterator end(const __tree_list_tree<T>* lptr)   noexcept { return lptr->end();   }

template<typename TToType,
         typename TFromType>
inline __tree_list_tree<TToType>* map(__tree_list_tree<TFromType>* lptr, std::function<TToType*(TFromType*)> f) {
    auto dest_list = new __tree_list_tree<TToType>();
    for(auto itr = begin(lptr); itr != end(lptr); itr++) {
        auto el = *itr;
        dest_list->append(f(el));
    }
    return dest_list;
}

template<typename TToType,
         typename TFromType>
inline __tree_list_tree<TToType>* map(ptr<__tree_list_tree<TFromType>> lptr, std::function<TToType*(TFromType*)> f) {
    return map<TToType, TFromType>(lptr, f);
}

template<typename TFilterType, typename TBaseType>
inline __tree_list_tree<TFilterType>* filter(__tree_list_tree<TBaseType>* lptr) {
    auto dest_list = new __tree_list_tree<TFilterType>();
    for(auto itr = begin(lptr); itr != end(lptr); itr++) {
        auto el = *itr;
        if(el != nullptr && el->template is<TFilterType>()) {
            dest_list->append(el->template as<TFilterType>());
        }
    }
    return dest_list;
}

template<typename TFilterType, typename TBaseType>
inline __tree_list_tree<TFilterType>* filter(ptr<__tree_list_tree<TBaseType>> lptr) {
    return filter<TFilterType, TBaseType>(unbox(lptr));
}


/* =============== *
 * Property Access *
 * =============== */

struct __tree_property_base {
public:
    // Anything ???

};

template<typename TTreeType>
struct __tree_property_tree : public __tree_property_base {
public:

    typedef TTreeType*                                          param_type_t;

    inline explicit __tree_property_tree(__tree_base* parent) noexcept
            : __tree_property_base(), _parent(parent), _index(parent->append_child(nullptr)) { }
    inline explicit __tree_property_tree(__tree_base* parent, TTreeType* value) noexcept
            : __tree_property_base(), _parent(parent), _index(parent->append_child(value)) { }

    inline explicit __tree_property_tree(const __tree_property_tree<TTreeType>& other) noexcept
            : __tree_property_base(), _parent(other.shift_address(this)), _index(other._index) { }

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

    template<typename TFromType, typename std::enable_if<std::is_base_of<TTreeType, TFromType>::value, int>::type = 0>
    inline ptr<TFromType> operator=(ptr<TFromType> value) noexcept {
        this->__set(unbox(value));
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

protected:

    inline __tree_base* __get()                   const noexcept { return unbox(this->_parent->_child_nodes[this->_index]); }
    inline void         __set(__tree_base* value)       noexcept {        this->_parent->_child_nodes[this->_index] = box<__tree_base>(value); }

private:

    inline __tree_base* shift_address(const __tree_property_tree<TTreeType>* newaddress) const noexcept {
        std::ptrdiff_t ptrdiff = ((std::intptr_t) this) - ((std::intptr_t)this->_parent);
        return (__tree_base*)(((std::intptr_t)newaddress) - ptrdiff);
    }

    __tree_base*                                                _parent;
    size_t                                                      _index;


};

template<typename TTreeListElement>
struct __tree_property_list final : public __tree_property_tree<typename __list_type_selector<TTreeListElement>::type> {
public:

    typedef typename __list_type_selector<TTreeListElement>::type list_t;
    typedef typename list_t::element_t                            element_t;

    inline explicit __tree_property_list(__tree_base* parent)                noexcept : __tree_property_tree<list_t>(parent) { }
    inline explicit __tree_property_list(__tree_base* parent, list_t* value) noexcept : __tree_property_tree<list_t>(parent, value) { }

    inline element_t operator[](size_t index) const noexcept {
        return (**this)[index];
    }

    inline list_t* operator=(list_t* v) {
        this->__set(v);
        return v;
    }

    inline typename list_t::      iterator begin()       noexcept { return dynamic_cast<list_t*>(this->__get())->begin(); }
    inline typename list_t::      iterator end()         noexcept { return dynamic_cast<list_t*>(this->__get())->end();   }
    inline typename list_t::const_iterator begin() const noexcept { return dynamic_cast<list_t*>(this->__get())->begin(); }
    inline typename list_t::const_iterator end()   const noexcept { return dynamic_cast<list_t*>(this->__get())->end();   }

};

template<typename TToType,
         typename TFromType>
inline typename __list_type_selector<TToType>::type* map(__tree_property_list<TFromType>& prop, std::function<TToType*(TFromType*)> f) {
    auto dest_list = new typename __list_type_selector<TToType>::type();
    try {
        for(auto itr = prop.begin(); itr != prop.end(); ++itr) {
            auto el = *itr;
            dest_list->append(f(el));
        }
    }
    catch (std::exception& e) {
        delete dest_list;
        throw e;
    }
    return dest_list;
}

template<typename TValue>
struct __tree_property_value {
public:

    typedef const TValue&                                       param_type_t;

    inline explicit __tree_property_value(__tree_base*)                      noexcept : _value()      { }
    inline explicit __tree_property_value(__tree_base*, const TValue& value) noexcept : _value(value) { }

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



/* ======================== *
 * Dynamic dispatch visitor *
 * ======================== */

template<typename TReturnType, typename... TParamTypes>
struct __dispatch_visitor_base {
protected:

    __dispatch_visitor_base() = default;
    virtual ~__dispatch_visitor_base() = default;

protected:

    virtual TReturnType handle_null_tree() {
        throw std::runtime_error("Null tree in dispatch handler");
    }
    virtual TReturnType handle_unregisterd_tree_type(tree_type_id id) {
        throw std::runtime_error(std::string("No registered function for type id ") + __all_tree_types[(uint64_t) id].name);
    }

    typedef std::function<TReturnType(__tree_base*, TParamTypes...)>
                                                                    dispatch_wrapper_t;

    std::map<tree_type_id, dispatch_wrapper_t>                  _function_map;
};

template<typename    TReturnType,
         typename... TParamTypes>
struct __dispatch_visitor_base_notvoid : public __dispatch_visitor_base<TReturnType, TParamTypes...>{
public:

    __dispatch_visitor_base_notvoid() = default;
    virtual ~__dispatch_visitor_base_notvoid() = default;

    template<typename T>
    inline TReturnType visit(T* t, TParamTypes... args) {
        if(t == nullptr) {
            return this->handle_null_tree();
        }
        else {
            tree_type_id id = t->get_tree_type();
            if(this->_function_map.find(id) == this->_function_map.end()) {
                return this->handle_unregisterd_tree_type(id);
            }
            return this->_function_map[id](t, args...);
        }
    }

    template<typename T>
    inline TReturnType visit(__tree_property_tree<T>& p, TParamTypes... args) {
        return this->visit((__tree_base*)p, args...);
    }

protected:

    template<typename TFuncReturnType,
             typename TTreeType>
    using dispatch_function_type = TFuncReturnType(*)(TTreeType*, TParamTypes...);

    template<typename TFuncReturnType,
             typename TClassType,
             typename TTreeType>
    using dispatch_method_type  = TFuncReturnType(TClassType::*)(TTreeType*, TParamTypes...);

    template<typename TFuncReturnType,
             typename TTreeType>
    void addfunction(dispatch_function_type<TFuncReturnType, TTreeType> func) {
        auto wfunc = [=](__tree_base* t, TParamTypes... args) {
            return func(t->as<TTreeType>(), args...);
        };
        this->_function_map[__get_tree_type_id<TTreeType>()] = wfunc;
    }

    template<typename TFuncReturnType,
             typename TClassType,
             typename TTreeType>
    void addmethod(dispatch_method_type<TFuncReturnType, TClassType, TTreeType> func) {
        auto wfunc = [=](__tree_base* t, TParamTypes... args) -> TReturnType {
            return (TReturnType) (dynamic_cast<TClassType*>(this)->*func)(t->as<TTreeType>(), args...);
        };
        this->_function_map[__get_tree_type_id<TTreeType>()] = wfunc;
    }
};

template<typename... TParamTypes>
struct __dispatch_visitor_base_void : public __dispatch_visitor_base<void, TParamTypes...>{
public:

    __dispatch_visitor_base_void() = default;
    virtual ~__dispatch_visitor_base_void() = default;

    inline void visit(__tree_base* t, TParamTypes... args) {
        if(t == nullptr) {
            this->handle_null_tree();
        }
        else {
            tree_type_id id = t->get_tree_type();
            if(this->_function_map.find(id) == this->_function_map.end()) {
                this->handle_unregisterd_tree_type(id);
            }
            this->_function_map[id](t, args...);
        }
    }
    template<typename T>
    inline void visit(__tree_property_tree<T>& p, TParamTypes... args) {
        return this->visit((__tree_base*)p, args...);
    }

protected:

    template<typename TTreeType>
    using dispatch_function_type = void(*)(TTreeType*, TParamTypes...);

    template<typename TClassType,
             typename TTreeType>
    using dispatch_method_type  = void(TClassType::*)(TTreeType*, TParamTypes...);

    template<typename TTreeType>
    void addfunction(dispatch_function_type<TTreeType> func) {
        auto wfunc = [=](__tree_base* t, TParamTypes... args) {
            return func(t->as<TTreeType>(), args...);
        };
        this->_function_map[__get_tree_type_id<TTreeType>()] = wfunc;
    }

    template<typename TClassType,
             typename TTreeType>
    void addmethod(dispatch_method_type<TClassType, TTreeType> func) {
        auto wfunc = [=](__tree_base* t, TParamTypes... args) {
            return (dynamic_cast<TClassType*>(this)->*func)(t->as<TTreeType>(), args...);
        };
        this->_function_map[__get_tree_type_id<TTreeType>()] = wfunc;
    }
};


template<typename T, typename... TArgs>
struct __dispatch_visitor_selector {
    typedef __dispatch_visitor_base_notvoid<T, TArgs...> type;
};

template<typename... TArgs>
struct __dispatch_visitor_selector<void, TArgs...> {
    typedef __dispatch_visitor_base_void<TArgs...>         type;
};



template<tree_type_id tp, typename base = __tree_base>
using extend_tree = __extend_tree<tp, base>;

template<typename T, typename... TArgs>
using dispatch_visitor = typename __dispatch_visitor_selector<T, TArgs...>::type;

template<typename T>
using property = typename __property_type_selector<T>::type;

template<typename T>
using list = typename __list_type_selector<T>::type;

}

#endif /* XCC_TREE_HPP_ */
