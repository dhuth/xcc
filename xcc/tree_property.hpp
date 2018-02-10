/*
 * tree_property.hpp
 *
 *  Created on: Feb 9, 2018
 *      Author: derick
 */

#ifndef XCC_TREE_PROPERTY_HPP_
#define XCC_TREE_PROPERTY_HPP_

#include "tree_base.hpp"
#include "tree_reference.hpp"
#include "tree_list.hpp"
#include "tree_algorithms.hpp"

namespace xcc {

struct __tree_property_base;
template<typename> struct __tree_property_tree;
template<typename> struct __tree_property_list;
template<typename> struct __tree_property_value;
template<typename> struct __tree_property_type_traits;

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
        return static_cast<TTreeType*>(this->__get());
    }

    inline operator ptr<TTreeType>() const noexcept {
        return box(static_cast<TTreeType*>(this->__get()));
    }

    template<typename TTargetType, typename std::enable_if<std::is_base_of<TTreeType, TTargetType>::value, int>::type = 0>
    inline operator TTargetType*() const noexcept {
        return static_cast<TTargetType*>(this->__get());
    }

    inline decltype(nullptr) operator=(decltype(nullptr) null) {
        this->__set(null);
        return null;
    }

    template<typename TFromType,
             enable_if_base_of_t<TTreeType, TFromType, int> = 0>
    inline TFromType* operator=(TFromType* value) noexcept {
        this->__set(value);
        return value;
    }

    template<typename TFromType,
             enable_if_base_of_t<TTreeType, TFromType, int> = 0>
    inline ptr<TFromType> operator=(ptr<TFromType> value) noexcept {
        this->__set(unbox(value));
        return value;
    }

    template<typename TFromType,
             enable_if_base_of_t<TTreeType, TFromType, int> = 0>
    inline TFromType* operator=(const __tree_property_tree<TFromType>& p) noexcept {
        TFromType* v = (TFromType*) p;
        this->__set(v);
        return v;
    }

    inline TTreeType* operator->() const noexcept {
        return static_cast<TTreeType*>(this->__get());
    }

    inline bool operator==(decltype(nullptr)) {
        return this->__get() == nullptr;
    }

    template<typename TOther>
    inline enable_if_tree_t<TOther, bool> operator==(const __tree_property_tree<TOther>& p) {
        return this->__get() == p.__get();
    }

    template<typename TOther>
    inline enable_if_tree_t<TOther, bool> operator!=(const __tree_property_tree<TOther>& p) {
        return this->__get() != p.__get();
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

    typedef typename __list_type_selector<TTreeListElement>::type   list_t;
    typedef typename list_t::element_t                              element_t;
    typedef typename list_t::iterator_t                             iterator_t;

    inline explicit __tree_property_list(__tree_base* parent)                noexcept : __tree_property_tree<list_t>(parent) { }
    inline explicit __tree_property_list(__tree_base* parent, list_t* value) noexcept : __tree_property_tree<list_t>(parent, value) { }

    template<typename TOtherElType,
             typename enable_if_base_of<TTreeListElement, TOtherElType, int>::type = 0>
    inline explicit __tree_property_list(__tree_base* parent, __tree_list_tree<TOtherElType>* value) noexcept
            : __tree_property_tree<list_t>(parent, copy_list_as<TTreeListElement>(value)) {
        // do nothing
    }

    inline element_t operator[](size_t index) const noexcept {
        return (**this)[index];
    }

    inline list_t* operator=(list_t* v) noexcept {
        this->__set(v);
        return v;
    }

    inline typename list_t::      iterator_t begin()       noexcept { return static_cast<list_t*>(this->__get())->begin(); }
    inline typename list_t::      iterator_t end()         noexcept { return static_cast<list_t*>(this->__get())->end();   }

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

    inline bool operator==(const __tree_property_value<TValue>& other) const noexcept {
        return this->_value == other._value;
    }

    inline bool operator==(const TValue&& other) const noexcept {
        return this->_value == other;
    }

    inline bool operator!=(const __tree_property_value<TValue>& other) const noexcept {
        return !this->operator==(other);
    }

    inline bool operator!=(const TValue&& other) const noexcept {
        return this->_value != other;
    }

    inline TValue* operator->() {
        return &this->_value;
    }

private:

    TValue                                                      _value;

};

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

template<typename T> struct __property_type_selector {
    typedef typename std::conditional<std::is_base_of<__tree_base, T>::value,
                            typename __property_type_tree_selector<T>::type,
                            typename __property_type_other_selector<T>::type>::type type;
};

template<typename T>
using property = typename __property_type_selector<T>::type;


template<typename T> inline typename __tree_property_list<T>::list_t::iterator_t begin(__tree_property_list<T>& lref) noexcept { return lref.begin(); }
template<typename T> inline typename __tree_property_list<T>::list_t::iterator_t end(__tree_property_list<T>& lref)   noexcept { return lref.end(); }

}

#endif /* XCC_TREE_PROPERTY_HPP_ */
