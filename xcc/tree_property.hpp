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

#include <string>

namespace xcc {


template<typename>  struct __property_value;
template<typename>  struct __property_value_base;

template<typename T>
struct __select_strong_property_type {
    typedef std::conditional_t<is_tree<T>::value,
            strong_ref<T>,
            __property_value<T>>                            type;
};


template<typename _Value>
struct __property_value {
public:

    inline __property_value(tree_t*) noexcept
            : _value() {
        // do nothing
    }

    inline __property_value(tree_t*, const _Value& v) noexcept
            : _value(v) {
        // do nothing
    }
    inline __property_value(tree_t*, _Value&& v) noexcept
            : _value(v) {
        // do nothing
    }
    inline __property_value(const _Value& v) noexcept
            : _value(v) {
        // do nothing
    }
    inline __property_value(_Value&& v) noexcept
            : _value(v) {
        // do nothing
    }

    inline operator _Value() const noexcept {
        return _value;
    }

    inline _Value* operator->() noexcept {
        return &_value;
    }

    inline const _Value* operator->() const noexcept {
        return &_value;
    }

    inline bool operator==(_Value v) {
        return _value == v;
    }

    inline bool operator!=(_Value v) {
        return _value != v;
    }

private:

    _Value                                                  _value;

};


template<typename T>
using property = typename __select_strong_property_type<T>::type;

template<typename T>
using property_value = __property_value<T>;


}

#endif /* XCC_TREE_PROPERTY_HPP_ */
