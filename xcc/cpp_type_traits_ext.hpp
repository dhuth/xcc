/*
 * cpp_type_traits_ext.hpp
 *
 *  Created on: Jan 18, 2018
 *      Author: derick
 */

#ifndef XCC_XPP_CPP_TYPE_TRAITS_EXT_HPP_
#define XCC_XPP_CPP_TYPE_TRAITS_EXT_HPP_

#include <type_traits>
#include <functional>
#include <tuple>
#include "managed_ptr.hpp"

namespace xcc {

template<typename... _Tpls>
struct __tuple_cat_type;

template<>
struct __tuple_cat_type<> {
    typedef std::tuple<>                                    type;
};

template<typename... _T>
struct __tuple_cat_type<std::tuple<_T...>> {
    typedef std::tuple<_T...>                               type;
};

template<typename... _Tels1, typename... _Tels2, typename... _Rest>
struct __tuple_cat_type<std::tuple<_Tels1...>, std::tuple<_Tels2...>, _Rest...> {
    typedef typename __tuple_cat_type<std::tuple<_Tels1..., _Tels2...>, _Rest...>::type
                                                            type;
};

template<typename... T>
using cat_tuple_t = typename __tuple_cat_type<T...>::type;



template<typename T, size_t S>
struct __make_tuple_n {
    typedef cat_tuple_t<std::tuple<T>, typename __make_tuple_n<T, S-1>::type>
                                                            type;
};

template<typename T>
struct __make_tuple_n<T, 0> {
    typedef std::tuple<>                                    type;
};

template<typename T, size_t S>
using tuplen_t = typename __make_tuple_n<T, S>::type;



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
