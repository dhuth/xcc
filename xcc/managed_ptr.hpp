/*
 * managed_ptr.hpp
 *
 *  Created on: Jan 23, 2017
 *      Author: derick
 */

#ifndef XPP_MANAGED_PTR_HPP_
#define XPP_MANAGED_PTR_HPP_

#include <cstdint>


namespace xcc {

struct __ptr_impl {
protected:

    __ptr_impl(void* p,       bool is_managed = false) noexcept;
    __ptr_impl(const void* p, bool is_managed = false) noexcept;
    inline ~__ptr_impl() noexcept = default;

    void pin() const noexcept;

    uint32_t incr_ref() const noexcept;
    uint32_t decr_ref() const noexcept;

    void*                                                       _internal_ptr;

};

template<typename T>
struct managed_ptr final : __ptr_impl {

    inline managed_ptr()                   noexcept : __ptr_impl((void*) nullptr, false) { }
    inline managed_ptr(T* p)               noexcept : __ptr_impl((void*) p, false)   { this->incr_ref(); }
    inline managed_ptr(T* p, bool managed) noexcept : __ptr_impl((void*) p, managed) { this->incr_ref(); }
    inline ~managed_ptr() noexcept {
        if(this->decr_ref() == 0) {
            delete reinterpret_cast<T*>(this->_internal_ptr);
        }
    }

    inline managed_ptr(const managed_ptr<T>& other) noexcept : __ptr_impl(other._internal_ptr) {
        this->incr_ref();
    }
    //inline managed_ptr(const managed_ptr<T>&& other) noexcept : __ptr_impl(other._internal_ptr) {
    //    this->incr_ref();
    //}
    inline managed_ptr<T>& operator=(managed_ptr<T>&& other) {
        if(this->decr_ref() == 0) {
            delete reinterpret_cast<T*>(this->_internal_ptr);
        }
        this->_internal_ptr = other._internal_ptr;
        this->incr_ref();
        return other;
    }

    inline operator T*() const noexcept {
        return reinterpret_cast<T*>(this->_internal_ptr);
    }

    inline T* operator->() const noexcept {
        return reinterpret_cast<T*>(this->_internal_ptr);
    }
};

template<typename T> inline managed_ptr<T>      box(T* v)                { return managed_ptr<T>(v); }
template<typename T> inline T*                  unbox(managed_ptr<T> p)  { return (T*)p;     }

}

#endif /* XPP_MANAGED_PTR_HPP_ */
