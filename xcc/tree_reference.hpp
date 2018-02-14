/*
 * refernce.hpp
 *
 *  Created on: Feb 9, 2018
 *      Author: derick
 */

#ifndef XCC_TREE_REFERENCE_HPP_
#define XCC_TREE_REFERENCE_HPP_

#include "tree_base.hpp"

namespace xcc {

using tvec_t = std::vector<ptr<__tree_base>>;

template<typename>  struct __vec_reference;
template<typename>  struct __vec_reference_impl_base;
template<typename>  struct __pick_reference_impl;

template<typename T>    using reference             = __vec_reference<T>;
template<typename T>    using reference_impl_base   = __vec_reference_impl_base<T>;

template<typename _TreeType>
struct __reference_base {
public:

    typedef _TreeType*                                      pointer_t;
    inline __reference_base() = default;
    __reference_base(const __reference_base&) = delete;
    __reference_base(__reference_base&&) = delete;

};

template<typename _TreeType>
struct __vec_reference_impl_base : public __reference_base<_TreeType> {
public:

    inline explicit __vec_reference_impl_base(tvec_t& vec) noexcept
            : _vector(vec),
              _index(vec.size()){
        _vector.push_back(box<__tree_base>(nullptr));
    }

    inline explicit __vec_reference_impl_base(tvec_t& vec, _TreeType* value) noexcept
            : _vector(vec),
              _index(vec.size()) {
        _vector.push_back(value);
    }

    inline operator _TreeType*() const noexcept {
        return this->__get();
    }

    template<typename _TargetType,
             enable_if_base_of_t<_TreeType, _TargetType, int> = 0>
    inline operator _TargetType*() const noexcept {
        return static_cast<_TargetType*>(this->__get());
    }

    inline operator ptr<_TreeType>() const noexcept {
        return box(this->__get());
    }

    inline nullptr_t operator=(nullptr_t) noexcept {
        this->__set(nullptr);
        return nullptr;
    }

    template<typename _FromType,
             enable_if_base_of_t<_TreeType, _FromType, int> = 0>
    inline _FromType* operator=(_FromType* t) noexcept {
        this->__set(t);
        return t;
    }

    template<typename _FromType,
             enable_if_base_of_t<_TreeType, _FromType, int> = 0>
    inline ptr<_FromType> operator=(ptr<_FromType> p) noexcept {
        this->__set(unbox(p));
        return p;
    }

    template<typename _FromType,
             enable_if_base_of_t<_TreeType, _FromType, int> = 0>
    inline _FromType* operator=(const reference<_FromType>& p) noexcept {
        _FromType* t = (_FromType*) p;
        this->__set(t);
        return t;
    }

    inline _TreeType* operator->() const noexcept {
        return this->__get();
    }

    inline bool operator==(nullptr_t) const noexcept {
        return this->_get() == nullptr;
    }

    inline bool operator==(const reference<_TreeType>& p) const noexcept {
        return this->__get() == p.__get();
    }

    inline bool operator!=(const reference<_TreeType>& p) const noexcept {
        return this->__get() != p.__get();
    }

protected:

    _TreeType* __get() const noexcept {
        return static_cast<_TreeType*>(unbox(_vector[_index]));
    }

    void __set(_TreeType* t) noexcept {
        _vector[_index] = box<__tree_base>(t);
    }

private:

    tvec_t&                                                 _vector;
    size_t                                                  _index;

};


template<typename _TreeType>
struct __pick_reference_impl {
public:
    typedef __vec_reference_impl_base<_TreeType>            type;
};


template<typename _TreeType>
struct __vec_reference : public __pick_reference_impl<_TreeType>::type {
public:

    typedef typename __pick_reference_impl<_TreeType>::type __impl_t;

    explicit inline __vec_reference(tvec_t& vec) noexcept
            : __impl_t(vec) {
        // do nothing
    }
    explicit inline __vec_reference(tvec_t& vec, _TreeType* t) noexcept
            : __impl_t(vec, t) {
        // do nothing
    }

    using __impl_t::operator=;

};


template<typename _TreeType, tvec_t __tree_base::* _VecMember>
struct __bound_vec_reference : public __vec_reference<_TreeType> {
public:

    explicit inline __bound_vec_reference(tree_t* parent) noexcept
            : __vec_reference<_TreeType>(parent->*_VecMember) {
        //...
    }
    explicit inline __bound_vec_reference(tree_t* parent, _TreeType* v) noexcept
            : __vec_reference<_TreeType>(parent->*_VecMember, v) {
        //...
    }

    using __vec_reference<_TreeType>::operator=;

};


template<typename T>    using strong_ref = __bound_vec_reference<T, &__tree_base::_strong_references>;
template<typename T>    using weak_ref = __bound_vec_reference<T, &__tree_base::_weak_references>;

}


#endif /* XCC_TREE_REFERENCE_HPP_ */
