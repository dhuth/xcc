/*
 * cpp_type_traits_ext.hpp
 *
 *  Created on: Jan 18, 2018
 *      Author: derick
 */

#ifndef XCC_XPP_CPP_TYPE_TRAITS_EXT_HPP_
#define XCC_XPP_CPP_TYPE_TRAITS_EXT_HPP_

#include <type_traits>
#include "managed_ptr.hpp"

namespace xcc {

template<typename TBase, typename TDerived, typename T = TDerived>
struct enable_if_base_of :
        public std::enable_if<std::is_base_of<TBase, TDerived>::value, T> { };

template<typename TBase, typename TDerived, typename T = TDerived>
using enable_if_base_of_t =
        typename enable_if_base_of<TBase, TDerived, T>::type;

template<typename T>
struct remove_managed_ptr {
    typedef T type;
};

template<typename T>
struct remove_managed_ptr<managed_ptr<T>> {
    typedef T type;
};

template<typename T>
using remove_managed_ptr_t =
        typename remove_managed_ptr<T>::type;

template<typename T>
struct remove_ptr_and_ref {
    typedef T                                               type;
};

template<typename T>
struct remove_ptr_and_ref<managed_ptr<T>> {
    typedef typename remove_ptr_and_ref<T>::type            type;
};

template<typename T>
struct remove_ptr_and_ref<T*> {
    typedef typename remove_ptr_and_ref<T>::type            type;
};

template<typename T>
struct remove_ptr_and_ref<T&> {
    typedef typename remove_ptr_and_ref<T>::type            type;
};

}



#endif /* XCC_XPP_CPP_TYPE_TRAITS_EXT_HPP_ */
