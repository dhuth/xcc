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
#include <iterator>
#include <initializer_list>
#include <functional>
#include <map>
#include <type_traits>
#include <vector>

#include "cppdefs.hpp"
#include "managed_ptr.hpp"


namespace xcc {

template<typename T>
using ptr = managed_ptr<T>;


/* ==================== *
 * Forward declarations *
 * ==================== */

struct tree_type;
struct __tree_base;
struct __tree_property_base;
template<typename> struct __tree_list_base;
template<typename> struct __tree_list_tree;
template<typename> struct __tree_list_value;
template<typename> struct __tree_list_type_traits;
template<typename> struct __tree_property_tree;
template<typename> struct __tree_property_list;
template<typename> struct __tree_property_value;
template<typename> struct __tree_property_type_traits;


/* ==================== *
 * Type Trait Utilities *
 * ==================== */

template<typename T>
struct __is_tree : std::is_base_of<__tree_base, T> { };

template<typename T>
using __is_tree_type = typename std::enable_if<__is_tree<T>::value, int>::type;

/* ==================== *
 * List Trait Utilities *
 * ==================== */

template<typename TList> struct __tree_list_type_traits {
    typedef typename TList::iterator_t                      iterator_t;
    typedef typename TList::const_iterator_t                const_iterator_t;
    typedef typename TList::list_t                          list_t;
    typedef typename TList::list_ptr_t                      list_ptr_t;
    typedef typename TList::element_t                       element_t;
};

template<typename T> struct __list_type_selector {
    typedef typename std::conditional<std::is_base_of<__tree_base, T>::value,
                            __tree_list_tree<T>,
                            __tree_list_value<T>>::type         type;
};

template<typename T>
using list = typename __list_type_selector<T>::type;

/* ======================== *
 * Property Trait Utilities *
 * ======================== */

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
    dummy,
    tree_list,
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

    inline static   constexpr size_t count() noexcept { return (size_t)tree_type_id::__type_count; }
    inline static __constexpr bool   is_base_of(const tree_type_id bt, const tree_type_id dt) noexcept {
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

#ifdef TREE_TYPE
#error "TREE_TYPE Already defined"
#endif
#define TREE_TYPE(name, ...)                                    struct name;
#include "all_tree_types.def.hpp"
#undef  TREE_TYPE

template<tree_type_id Id>
struct __tree_id_to_type { };

#ifdef TREE_TYPE
#error "TREE_TYPE Already defined"
#endif
#define TREE_TYPE(name, ...) template<> struct __tree_id_to_type<tree_type_id::name>{ typedef xcc::name type; };
#include "all_tree_types.def.hpp"
#undef TREE_TYPE

template<tree_type_id id> struct tree_type_selector { };

#ifdef TREE_TYPE
#error "TREE_TYPE Already defined"
#endif
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
        // do nothing
    }

    inline void pin() const noexcept {
        box(this).pin();
    }

protected:

    template<typename T>
    friend struct __tree_property_tree;

    template<typename TElement>
    friend struct __tree_list_tree;

    template<typename TBaseType, typename... TParamTypes>
    friend struct __dispatch_tree_walker_base;

    /**
     * Used by __tree_property_tree constructor to insert new children
     */
    inline size_t __append_child(__tree_base* v) noexcept {
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

    // copy constructor
    inline __extend_tree(const base_type& other) : TBase(other) { }

};

template<tree_type_id tp, typename base = __tree_base>
using extend_tree = __extend_tree<tp, base>;

template<typename T, __is_tree_type<T> = 0>
constexpr tree_type_id __get_tree_type_id() { return T::type_id; }


template<typename TElement>
struct __tree_list_base : public __extend_tree<tree_type_id::tree_list, __tree_base> {
protected:

    inline __tree_list_base()                                          noexcept : base_type() { }

};

template<typename TElement>
struct __tree_list_value final : public __tree_list_base<TElement> {
private:

    typedef          __tree_list_base<TElement>                 base_list_t;
    typedef          __tree_list_value<TElement>                list_t;
    typedef          list_t*                                    list_ptr_t;

public:

    inline          __tree_list_value()                                          noexcept : base_list_t()                  { }
    inline explicit __tree_list_value(TElement f)                                noexcept : base_list_t()                  { this->_list.push_back(f); }
    inline          __tree_list_value(TElement f, const list_ptr_t r)            noexcept : base_list_t(), _list(r->_list) { this->_list.insert(this->_list.begin(), f); }
    inline          __tree_list_value(const list_ptr_t f, TElement l)            noexcept : base_list_t(), _list(f->_list) { this->_list.push_back(l); }
    inline explicit __tree_list_value(std::vector<TElement>& vec)                noexcept : base_list_t() {
        for(auto el: vec) {
            this->_list.push_back(el);
        }
    }
    inline __tree_list_value(std::initializer_list<TElement> init_list) noexcept : list_t()     {
        for(auto el: init_list) {
            this->_list.push_back(el);
        }
    }

    virtual ~__tree_list_value() = default;

    inline TElement& operator[](size_t index) noexcept {
        return this->_list[index];
    }

    inline void append(TElement& el) noexcept {
        this->_list.push_back(el);
    }

    inline void append(TElement&& el) noexcept {
        this->_list.push_back(el);
    }

    inline typename std::vector<TElement>::size_type size() const noexcept {
        return this->_list.size();
    }

protected:

    std::vector<TElement>                                               _list;

};

template<typename TElement>
struct __tree_list_tree final : public __tree_list_base<ptr<TElement>> {
public:

    // Traits
    typedef __tree_list_base<ptr<TElement>>             base_list_t;
    typedef __tree_list_tree<TElement>                  list_t;
    typedef list_t*                                     list_ptr_t;
    typedef ptr<list_t>                                 list_box_t;
    typedef TElement*                                   element_t;
    typedef ptr<TElement>                               element_box_t;

    struct iterator {
        size_t                                  index;
        std::vector<ptr<__tree_base>>&          vector;

        inline iterator operator++()          noexcept { ++index; return *this; }
        inline iterator operator++(int)       noexcept { ++index; return {index-1, vector}; }

        inline element_t operator*()          noexcept { return (element_t) unbox(vector[index]); }
        inline element_t operator*()    const noexcept { return (element_t) unbox(vector[index]); }

        inline bool operator==(const iterator& other) {
            return (other.index == this->index) && (other.vector == this->vector);
        }

        inline bool operator!=(const iterator& other) {
            return (other.index != this->index) || (other.vector != this->vector);
        }
    };
    typedef const iterator                              const_iterator;
    typedef iterator                                    iterator_t;
    typedef const_iterator                              const_iterator_t;

    inline __tree_list_tree()                                           noexcept : base_list_t()           { }
    inline __tree_list_tree(element_t f)                                noexcept : base_list_t()           { this->_child_nodes.push_back(box((__tree_base*) f)); }
    inline __tree_list_tree(element_t f, const list_ptr_t r)            noexcept : list_t(r->_child_nodes) { this->_child_nodes.insert(this->_child_nodes.begin(), box((__tree_base*)f)); }
    inline __tree_list_tree(const list_ptr_t f, element_t l)            noexcept : list_t(f->_child_nodes) { this->_child_nodes.push_back(box((__tree_base*) l)); }
    inline __tree_list_tree(std::vector<element_t>& vec)                noexcept : base_list_t() {
        for(auto el : vec) {
            this->_child_nodes.push_back(box((__tree_base*) el));
        }
    }
    inline __tree_list_tree(std::vector<ptr<__tree_base>>& vec) {
        for(auto el: vec) {
            this->_child_nodes.push_back(el);
        }
    }
    inline __tree_list_tree(std::initializer_list<element_t> init_list) noexcept : list_t() {
        for(auto el : init_list) {
            this->_child_nodes.push_back(box((__tree_base*) el));
        }
    }

    virtual ~__tree_list_tree() = default;


    inline       iterator begin()       noexcept { return {0,                         this->_child_nodes}; }
    inline       iterator end()         noexcept { return {this->_child_nodes.size(), this->_child_nodes}; }
    inline const_iterator begin() const noexcept { return {0,                         this->_child_nodes}; }
    inline const_iterator end()   const noexcept { return {this->_child_nodes.size(), this->_child_nodes}; }

    inline element_t operator[](size_t index) {
        return (element_t) unbox(this->_child_nodes[index]);
    }

    template<typename TFindElement,
             typename std::enable_if<std::is_base_of<TElement, TFindElement>::value, int>::type = 0>
    inline       iterator find(TFindElement* el) {
        auto itr = this->begin();
        while(itr != this->end()) {
            if(*itr == dynamic_cast<element_t>(el)) {
                return itr;
            }
            ++itr;
        }
        return this->end();
    }

    inline void append(element_t& el) {
        this->_child_nodes.push_back(box((__tree_base*) el));
    }

    inline void append(element_t&& el) {
        this->_child_nodes.push_back(box((__tree_base*) el));
    }

    inline void prepend(element_t& el) {
        this->_child_nodes.insert(this->_child_nodes.begin(), box((__tree_base*) el));
    }

    inline void prepend(element_t&& el) {
        this->_child_nodes.insert(this->_child_nodes.begin(), box((__tree_base*) el));
    }

    inline typename std::vector<ptr<__tree_base>>::size_type size() const noexcept {
        return this->_child_nodes.size();
    }
};

template<typename T> inline typename __tree_list_tree<T>::      iterator begin(__tree_list_tree<T>* lptr)       noexcept { return lptr->begin(); }
template<typename T> inline typename __tree_list_tree<T>::      iterator end(__tree_list_tree<T>* lptr)         noexcept { return lptr->end();   }
template<typename T> inline typename __tree_list_tree<T>::const_iterator begin(const __tree_list_tree<T>* lptr) noexcept { return lptr->begin(); }
template<typename T> inline typename __tree_list_tree<T>::const_iterator end(const __tree_list_tree<T>* lptr)   noexcept { return lptr->end();   }
template<typename T> inline typename __tree_list_tree<T>::      iterator begin(ptr<__tree_list_tree<T>>& lptr)  noexcept { return lptr->begin(); }
template<typename T> inline typename __tree_list_tree<T>::      iterator end(ptr<__tree_list_tree<T>>& lptr)    noexcept { return lptr->end(); }

template<typename T>
inline T* first(__tree_list_tree<T>* l) {
    return *(begin(l));
}

template<typename T>
inline T* first(ptr<__tree_list_tree<T>>& l) {
    return *(begin(l));
}

template<typename T>
inline T* first(__tree_property_list<T>& p) {
    return p[0];
}

template<typename T>
inline __tree_list_tree<T>* rest(__tree_list_tree<T>* l) {
    __tree_list_tree<T>* nl = new __tree_list_tree<T>();
    for(int i = 1; i < l->size(); i++) {
        nl->append((*l)[i]);
    }
    return nl;
}

template<typename TSrcEl,
         typename TDestEl>
inline __tree_list_tree<TDestEl>* __map(__tree_list_tree<TSrcEl>* slist, std::function<TDestEl*(TSrcEl*)> f) {
    __tree_list_tree<TDestEl>* dlist = new __tree_list_tree<TDestEl>();
    for(auto el: *slist) {
        dlist->append(f(el));
    }
    return dlist;
}

template<typename TSrcEl,
         typename TDestEl>
inline __tree_list_value<TDestEl>* __map(__tree_list_value<TSrcEl>* slist, std::function<TDestEl(TSrcEl)> f) {
    __tree_list_value<TDestEl>* dlist = new __tree_list_value<TDestEl>();
    for(auto el: *slist) {
        dlist->append(f(el));
    }
    return dlist;
}

template<typename TSrcEl,
         typename TFunc,
         typename _TDestEl = std::remove_pointer_t<std::result_of_t<TFunc(TSrcEl*)>>>
inline ptr<__tree_list_tree<_TDestEl>> map(ptr<__tree_list_tree<TSrcEl>>& slist, TFunc f) {
    return box(__map(unbox(slist), std::function<_TDestEl*(TSrcEl*)>(f)));
}

template<typename TSrcEl,
         typename TFunc,
         typename _TDestEl = std::remove_pointer_t<std::result_of_t<TFunc(TSrcEl*)>>>
inline ptr<__tree_list_tree<_TDestEl>> map(__tree_list_tree<TSrcEl>* slist, TFunc f) {
    return box(__map(slist, std::function<_TDestEl*(TSrcEl*)>(f)));
}

template<typename TSrcEl,
         typename TFunc,
         typename _TDestEl = std::remove_pointer_t<std::result_of_t<TFunc(TSrcEl*)>>>
inline ptr<__tree_list_tree<_TDestEl>> map(__tree_property_list<TSrcEl>& slist, TFunc f) {
    return box(__map((__tree_list_tree<TSrcEl>*) slist, std::function<_TDestEl*(TSrcEl*)>(f)));
}

template<typename TTreeType,
         typename TPred>
inline ptr<__tree_list_tree<TTreeType>> filter(__tree_list_tree<TTreeType>* slist, TPred pred) {
    __tree_list_tree<TTreeType>* dlist = new __tree_list_tree<TTreeType>();
    auto f = std::function<bool(TTreeType*)>(pred);
    for(auto el: *slist) {
        if(f(el)) {
            dlist->append(el);
        }
    }
    return box(dlist);
}

template<typename TTreeType,
         typename TPred>
inline ptr<__tree_list_tree<TTreeType>> filter(ptr<__tree_list_tree<TTreeType>>& slist, TPred pred) {
    return filter(unbox(slist), pred);
}

template<typename TTreeType,
         typename TPred>
inline ptr<__tree_list_tree<TTreeType>> filter(__tree_property_list<TTreeType>& slist, TPred pred) {
    return filter((__tree_list_tree<TTreeType>*) slist, pred);
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
            : __tree_property_base(), _parent(parent), _index(parent->__append_child(nullptr)) { }
    inline explicit __tree_property_tree(__tree_base* parent, TTreeType* value) noexcept
            : __tree_property_base(), _parent(parent), _index(parent->__append_child(value)) { }

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
        return this->__get() == p.__get();
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

    inline list_t* operator=(list_t* v) noexcept {
        this->__set(v);
        return v;
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

    inline explicit __tree_property_value(__tree_base*)                      noexcept : _value()      { }
    inline explicit __tree_property_value(__tree_base*, const TValue& value) noexcept : _value(value) { }

    inline operator const TValue&() const noexcept {
        return this->_value;
    }

    template<typename T, typename std::enable_if<std::is_convertible<TValue, T>::value, int>::type = 0>
    inline operator T() const noexcept {
        return (T)this->_value;
    }

    template<typename T, typename std::enable_if<std::is_convertible<TValue, T>::value, int>::type = 0>
    inline operator const T&() const noexcept {
        return (const T&)this->_value;
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

    virtual TReturnType handle_null_tree(TParamTypes... args) {
        throw std::runtime_error("Null tree in dispatch handler");
    }
    virtual TReturnType handle_unregisterd_tree_type(tree_type_id id, TParamTypes... args) {
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
            this->handle_null_tree(args...);
        }
        else {
            tree_type_id id = t->get_tree_type();
            if(this->_function_map.find(id) == this->_function_map.end()) {
                this->handle_unregisterd_tree_type(id, args...);
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




/* ==================================== *
 * Dynamic Dispatch Translator / Walker *
 * ==================================== */

template<typename TBaseType, typename... TParamTypes>
struct __dispatch_tree_walker_base {
public:

    inline __dispatch_tree_walker_base()
            : _use_ignore(false),
              _use_include(false) {
        //...
    }
    virtual ~__dispatch_tree_walker_base() = default;


    template<typename TTreeType,
             __is_tree_type<TTreeType> = 0>
    using visit_func_t = void(*)(TTreeType*, TParamTypes...);

    template<typename TReturnType,
             typename TTreeType,
             typename std::enable_if<std::is_base_of<TReturnType, TTreeType>::value, int>::type = 0,
             __is_tree_type<TTreeType> = 0>
    using translate_func_t = TReturnType*(*)(TTreeType*, TParamTypes...);

    template<typename TReturnType,
             typename TTreeType,
             typename TClassType,
             typename std::enable_if<std::is_base_of<TReturnType, TTreeType>::value, int>::type = 0,
             typename std::enable_if<std::is_base_of<__dispatch_tree_walker_base<TBaseType, TParamTypes...>, TClassType>::value, int>::type = 0,
             __is_tree_type<TTreeType> = 0>
    using translate_method_t = TReturnType*(TClassType::*)(TTreeType*, TParamTypes...);

    template<typename TTreeType,
             typename TClassType,
             typename std::enable_if<std::is_base_of<__dispatch_tree_walker_base<TBaseType, TParamTypes...>, TClassType>::value, int>::type = 0,
             __is_tree_type<TTreeType> = 0>
    using visit_method_t = void(TClassType::*)(TTreeType*, TParamTypes...);




    template<typename TTreeType>
    inline void add(visit_func_t<TTreeType> func) {
        auto wfunc = [=](__tree_base**, __tree_base* node, TParamTypes... args) {
            func(node->as<TTreeType>(), args...);
        };
        this->_functions[__get_tree_type_id<TTreeType>()] = wfunc;
    }

    template<typename TReturnType, typename TTreeType>
    inline void add(translate_func_t<TReturnType, TTreeType> func) {
        auto wfunc = [=](__tree_base** retv, __tree_base* node, TParamTypes... args) {
            *retv = dynamic_cast<__tree_base*>(
                    func(node->as<TTreeType>(), args...));
        };
        this->_functions[__get_tree_type_id<TTreeType>()] = wfunc;
    }

    template<typename TReturnType, typename TTreeType, typename TClassType>
    inline void add(translate_method_t<TReturnType, TTreeType, TClassType> mtd) {
        auto wfunc = [=](__tree_base** retv, __tree_base* node, TParamTypes... args) {
            *retv = dynamic_cast<__tree_base*>(
                    (dynamic_cast<TClassType*>(this)->*mtd)(node->as<TTreeType>(), args...));
        };
        this->_functions[__get_tree_type_id<TTreeType>()] = wfunc;
    }

    template<typename TTreeType, typename TClassType>
    inline void add(visit_method_t<TTreeType, TClassType> mtd) {
        auto wfunc = [=](__tree_base** retv, __tree_base* node, TParamTypes... args) {
            (dynamic_cast<TClassType*>(this)->*mtd)(node->as<TTreeType>(), args...);
        };
        this->_functions[__get_tree_type_id<TTreeType>()] = wfunc;
    }

    virtual void begin(tree_type_id, TBaseType*, TParamTypes...)     { }
    virtual void end(tree_type_id,   TBaseType*, TParamTypes...)     { }
    virtual void postvisit(tree_type_id, TBaseType*, TParamTypes...) { }

    TBaseType* visit(TBaseType* t, TParamTypes... args) {
        return dynamic_cast<TBaseType*>(this->visit_internal(dynamic_cast<__tree_base*>(t), args...));
    }

    inline void set(TBaseType* t, TBaseType* v) noexcept {
        this->_visited[t] = box((__tree_base*)v);
    }

    inline void ignore(std::vector<tree_type_id>&& ignore_ids) {
        this->_use_ignore = true;
        this->_ignore = ignore_ids;
    }

    inline void include(std::vector<tree_type_id>&& include_ids) {
        this->_use_include = true;
        this->_include = include_ids;
        this->_include.push_back(tree_type_id::tree_list);
    }

    inline void reset() noexcept {
        this->_visited.clear();
    }


private:

    __tree_base* visit_internal(__tree_base* t, TParamTypes... args) {
        if(t != nullptr) {

            if(this->_use_ignore) {
                for(auto ignore_id: this->_ignore) {
                    if(t->is(ignore_id)) {
                        return t;
                    }
                }
            }

            if(this->_use_include) {
                bool is_included = false;
                for(auto include_id: this->_include) {
                    if(t->is(include_id)) {
                        is_included = true;
                        break;
                    }
                }
                if(!is_included) {
                    return t;
                }
            }

            if(this->_visited.find(t) == this->_visited.end()) {
                auto new_t = this->visit_impl(t, args...);
                if(new_t != nullptr && t->is<TBaseType>()) {
                    this->postvisit(new_t->get_tree_type(), dynamic_cast<TBaseType*>(new_t), args...);
                }
                this->_visited[t] = new_t;
                return new_t;
            }
            return this->_visited[t];
        }
        return nullptr;
    }

protected:

    virtual __tree_base* visit_impl(__tree_base*, TParamTypes...) = 0;
    inline void walk_children(__tree_base* t, TParamTypes... args) {
        if(t->is<TBaseType>()) {
            this->begin(t->get_tree_type(), dynamic_cast<TBaseType*>(t), args...);
        }
        for(size_t i = 0; i < t->_child_nodes.size(); i++) {
            auto child = unbox(t->_child_nodes[i]);
            if(child != nullptr) {
                t->_child_nodes[i] = box(this->visit_internal(child, args...));
            }
        }
        if(t->is<TBaseType>()) {
            this->end(t->get_tree_type(), dynamic_cast<TBaseType*>(t), args...);
        }
    }

    inline __tree_base* do_visit(__tree_base* t, TParamTypes... args) {
        auto tpid = t->get_tree_type();
        if(this->_functions.find(tpid) != this->_functions.end()) {
            __tree_base* retval = t;
            this->_functions[tpid](&retval, t, args...);
            return retval;
        }
        return t;
    }

    typedef std::function<void(__tree_base**,__tree_base*,TParamTypes...)>
                                                                        fwalk_t;

    std::map<tree_type_id, fwalk_t>                                     _functions;
    std::map<__tree_base*, __tree_base*>                                _visited;
    std::vector<tree_type_id>                                           _ignore;
    std::vector<tree_type_id>                                           _include;
    bool                                                                _use_ignore;
    bool                                                                _use_include;

};

template<typename TBaseType, typename... TParamTypes>
struct dispatch_preorder_tree_walker : public __dispatch_tree_walker_base<TBaseType, TParamTypes...> {
public:

    virtual ~dispatch_preorder_tree_walker() = default;

protected:

    virtual __tree_base* visit_impl(__tree_base* t, TParamTypes... args) final override {
        auto new_t = this->do_visit(t, args...);
        this->walk_children(new_t, args...);
        return new_t;
    }
};

template<typename TBaseType, typename... TParamTypes>
struct dispatch_postorder_tree_walker : public __dispatch_tree_walker_base<TBaseType, TParamTypes...> {
public:

    virtual ~dispatch_postorder_tree_walker() = default;

protected:

    virtual __tree_base* visit_impl(__tree_base* t, TParamTypes... args) final override {
        this->walk_children(t, args...);
        auto new_t = this->do_visit(t, args...);
        return new_t;
    }
};

template<typename T, typename... TArgs>
using dispatch_visitor = typename __dispatch_visitor_selector<T, TArgs...>::type;

template<typename T>
using property = typename __property_type_selector<T>::type;

}

#endif /* XCC_TREE_HPP_ */
