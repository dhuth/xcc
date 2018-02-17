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

template<typename>  struct __reference;
template<typename>  struct __reference_impl_base;
template<typename>  struct __pick_reference_impl;

template<typename T>    using reference             = __reference<T>;
template<typename T>    using reference_impl_base   = __reference_impl_base<T>;

template<typename _TreeType>
struct __reference_base {
public:

    typedef std::function<_TreeType*()>                     getfunc_t;
    typedef std::function<void(_TreeType*)>                 setfunc_t;

    typedef _TreeType*                                      pointer_t;
    explicit inline __reference_base(getfunc_t get, setfunc_t set) noexcept
            : get(get),
              set(set) {
        // do nothing
    }
    __reference_base(const __reference_base&) = delete;
    __reference_base(__reference_base&&) = delete;

    static getfunc_t getbyindex(tvec_t& vec, size_t idx) noexcept {
        return [=,&vec]() -> _TreeType* {
            return static_cast<_TreeType*>(unbox(vec[idx]));
        };
    }

    static setfunc_t setbyindex(tvec_t& vec, size_t idx) noexcept {
        return [=,&vec](_TreeType* t) {
            vec[idx] = box<tree_t>(t);
        };
    }

protected:

    getfunc_t                                               get;
    setfunc_t                                               set;

};


template<typename _TreeType>
struct __reference_impl_base : public __reference_base<_TreeType> {
public:

    typedef typename __reference_base<_TreeType>::getfunc_t getfunc_t;
    typedef typename __reference_base<_TreeType>::setfunc_t setfunc_t;

    inline explicit __reference_impl_base(getfunc_t get, setfunc_t set) noexcept
            : __reference_base<_TreeType>(get, set) {
    }

    template<typename _TargetType,
             enable_if_base_of_t<_TargetType, _TreeType, int> = 0>
    inline operator _TargetType*() const noexcept {
        return static_cast<_TargetType*>(this->get());
    }

    inline operator ptr<_TreeType>() const noexcept {
        return box(this->get());
    }

    inline nullptr_t operator=(nullptr_t) noexcept {
        this->set(nullptr);
        return nullptr;
    }

    template<typename _FromType,
             enable_if_base_of_t<_TreeType, _FromType, int> = 0>
    inline _FromType* operator=(_FromType* t) noexcept {
        this->set(t);
        return t;
    }

    template<typename _FromType,
             enable_if_base_of_t<_TreeType, _FromType, int> = 0>
    inline ptr<_FromType> operator=(ptr<_FromType> p) noexcept {
        this->set(unbox(p));
        return p;
    }

    template<typename _FromType,
             enable_if_base_of_t<_TreeType, _FromType, int> = 0>
    inline _FromType* operator=(const reference<_FromType>& p) noexcept {
        _FromType* t = (_FromType*) p;
        this->set(t);
        return t;
    }

    inline _TreeType* operator->() const noexcept {
        return this->get();
    }

    inline bool operator==(nullptr_t) const noexcept {
        return this->get() == nullptr;
    }

    inline bool operator==(const reference<_TreeType>& p) const noexcept {
        return this->get() == p.get();
    }

    inline bool operator!=(nullptr_t) const noexcept {
        return this->get() != nullptr;
    }

    inline bool operator!=(const reference<_TreeType>& p) const noexcept {
        return this->get() != p.get();
    }

protected:

//    _TreeType* __get() const noexcept {
//        return static_cast<_TreeType*>(unbox(_vector[_index]));
//    }

//    void __set(_TreeType* t) noexcept {
//        _vector[_index] = box<__tree_base>(t);
//    }

private:

//    tvec_t&                                                 _vector;
//    size_t                                                  _index;

};


template<typename _TreeType>
struct __pick_reference_impl {
public:
    typedef __reference_impl_base<_TreeType>            type;
};


template<typename _TreeType>
struct __reference : public __pick_reference_impl<_TreeType>::type {
public:

    typedef typename __reference_base<_TreeType>::getfunc_t getfunc_t;
    typedef typename __reference_base<_TreeType>::setfunc_t setfunc_t;
    typedef typename __pick_reference_impl<_TreeType>::type __impl_t;

    explicit inline __reference(getfunc_t get, setfunc_t set) noexcept
            : __impl_t(get, set) {
        // do nothing
    }

    using __impl_t::operator=;

};


template<typename _TreeType, tvec_t __tree_base::* _VecMember>
struct __bound_vec_reference : public __reference<_TreeType> {
public:

    explicit inline __bound_vec_reference(tree_t* parent) noexcept
            : __reference<_TreeType>(
                    __reference_base<_TreeType>::getbyindex(parent->*_VecMember, (parent->*_VecMember).size()),
                    __reference_base<_TreeType>::setbyindex(parent->*_VecMember, (parent->*_VecMember).size())){
        (parent->*_VecMember).push_back(nullptr);
    }
    explicit inline __bound_vec_reference(tree_t* parent, _TreeType* v) noexcept
            : __reference<_TreeType>(
                    __reference_base<_TreeType>::getbyindex(parent->*_VecMember, (parent->*_VecMember).size()),
                    __reference_base<_TreeType>::setbyindex(parent->*_VecMember, (parent->*_VecMember).size())) {
        (parent->*_VecMember).push_back(box<tree_t>(v));
    }

    using __reference<_TreeType>::operator=;

};


template<typename T>    using strong_ref = __bound_vec_reference<T, &__tree_base::_strong_references>;
template<typename T>    using weak_ref = __bound_vec_reference<T, &__tree_base::_weak_references>;

}


#endif /* XCC_TREE_REFERENCE_HPP_ */
