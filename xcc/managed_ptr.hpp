/*
 * managed_ptr.hpp
 *
 *  Created on: Jan 23, 2017
 *      Author: derick
 */

#ifndef XPP_MANAGED_PTR_HPP_
#define XPP_MANAGED_PTR_HPP_

#include "config.h"

#include <cstdint>
#include <iomanip>
#include <iostream>

namespace xcc {

void    dump_managed_ptr_list(std::ostream& s);
int32_t __refcount(void* p);
bool    is_pinned(void* addr);


#ifdef DEV_TRACE_MANAGED_PTRS

bool __is_watched(void* p);
void __watch_pointer(void* p);

# ifndef DEV_TRACE_STREAM
# define DEV_TRACE_STREAM std::cout
# endif

#define trace_msg(c, s, m)              if(c) { s << m << std::endl; }
#define trace_ptr_msg(p, m)\
        trace_msg(__is_watched(p),\
                  DEV_TRACE_STREAM,\
                  std::setw(6) << std::hex << (uintptr_t)p << std::setw(4) << std::dec << __refcount(p) << ": " << m << std::flush)

#else

#define trace_msg(...)          /* nothing */
#define trace_ptr_msg(...)      /* nothing */

#endif

struct __ptr_impl {
protected:

    __ptr_impl(void* p,       bool is_managed = false) noexcept;
    __ptr_impl(const void* p, bool is_managed = false) noexcept;
    inline ~__ptr_impl() noexcept = default;

    int32_t incr_ref() const noexcept;
    int32_t decr_ref() const noexcept;

    template<typename T>
    inline void maybe_delete() {
        if(!is_pinned(this->_internal_ptr)) {
            delete reinterpret_cast<T*>(this->_internal_ptr);
        }
    }

    void*                                                       _internal_ptr;

public:

    void pin()       const noexcept;

};

template<typename T>
struct managed_ptr final : __ptr_impl {

    inline managed_ptr()                    noexcept : __ptr_impl((void*) nullptr, false) { }
    inline managed_ptr(T* p)                noexcept : __ptr_impl((void*) p, true)    { this->incr_ref(); }
    inline managed_ptr(T* p, bool managed)  noexcept : __ptr_impl((void*) p, managed) { this->incr_ref(); }

    template<typename TOther,
             typename std::enable_if<std::is_base_of<T, TOther>::value, int>::type>
    inline managed_ptr(TOther* p)           noexcept : __ptr_impl((void*) p, true)    { this->incr_ref(); }

    inline ~managed_ptr() noexcept {
        if(this->decr_ref() == 0) {
            trace_ptr_msg(this->_internal_ptr, "deleting");
            delete reinterpret_cast<T*>(this->_internal_ptr);
        }
        trace_ptr_msg(this->_internal_ptr, "del");
    }

    inline managed_ptr(const managed_ptr<T>& other) noexcept : __ptr_impl(other._internal_ptr) {
        this->incr_ref();
        trace_ptr_msg(this->_internal_ptr, "copy ctor");
    }

#if 0
    template<typename TOther,
             typename std::enable_if<std::is_base_of<T, TOther>::value, int>::type = 0>
    inline managed_ptr(const managed_ptr<TOther>& other) noexcept : __ptr_impl(other._internal_ptr) {
        this->incr_ref();
        //TODO: static cast
        trace_ptr_msg(this->_internal_ptr, "copy ctor of super type");
    }
#endif

#if 0
    inline managed_ptr<T>& operator=(managed_ptr<T>& other) {
        if(this->_internal_ptr != other._internal_ptr) {
            if(this->decr_ref() == 0) {
                //TODO:
                //      Needs some serious testing before deleting here...
                //this->maybe_delete<T>();
            }
            this->_internal_ptr = other._internal_ptr;
        }
        this->incr_ref();
        trace_ptr_msg(this->_internal_ptr, "assign ctor");
        return other;
    }
#endif

#if 0
    template<typename TOther,
             typename std::enable_if<std::is_base_of<T, TOther>::value, int>::type = 0>
    inline TOther* operator=(TOther* p) {
        return (TOther*) this->operator=(managed_ptr<TOther>(p));
    }
#endif

    template<typename TOther,
             typename std::enable_if<std::is_base_of<T, TOther>::value, int>::type = 0>
    inline managed_ptr<TOther>& operator=(managed_ptr<TOther>& other) {
        if(this->_internal_ptr != other._internal_ptr) {
            if(this->decr_ref() == 0) {
                //TODO:
                //      Needs some serious testing before deleting here...
                //this->maybe_delete<T>();
            }
            //TODO: static cast
            this->_internal_ptr = other._internal_ptr;
        }
        trace_ptr_msg(this->_internal_ptr, "assign ctor of super type");
        return other;
    }


    inline const managed_ptr<T>& operator=(const managed_ptr<T>& other) {
        if(this->_internal_ptr != other._internal_ptr) {
            if(this->decr_ref() == 0) {
                //TODO:
                //      Needs some serious testing before deleting here...
                //this->maybe_delete<T>();
            }
            this->_internal_ptr = other._internal_ptr;
        }
        this->incr_ref();
        trace_ptr_msg(this->_internal_ptr, "ctor assign ctor");
        return other;
    }

    inline T& operator*() noexcept {
        return *reinterpret_cast<T*>(this->_internal_ptr);
    }

    inline T& operator*() const noexcept {
        return *reinterpret_cast<T*>(this->_internal_ptr);
    }

    inline operator T*() noexcept {
        return reinterpret_cast<T*>(this->_internal_ptr);
    }

    inline operator T*() const noexcept {
        return reinterpret_cast<T*>(this->_internal_ptr);
    }

    inline T* operator->() noexcept {
        return reinterpret_cast<T*>(this->_internal_ptr);
    }

    inline T* operator->() const noexcept {
        return reinterpret_cast<T*>(this->_internal_ptr);
    }
};

template<typename T> inline managed_ptr<T>      box(T* v)                   { return managed_ptr<T>(v); }
template<typename T> inline T*                  unbox(managed_ptr<T> p)     { return (T*)p;     }
template<typename T>        int32_t             refcount(managed_ptr<T>& p) { return __refcount((void*)(T*)p); }
template<typename T> inline uintptr_t           addr(managed_ptr<T>& p)     { return (uintptr_t)(T*)p; }

#ifdef DEV_TRACE_MANAGED_PTRS

template<typename T> inline void                watch(managed_ptr<T>& p)    { __watch_pointer((void*)(T*)p); }

#else

template<typename T> inline void                watch(managed_ptr<T>& p)    { /* do nothing */ }

#endif

template<typename T>
inline bool is_pinned(managed_ptr<T>& p) {
    return is_pinned((void*)(T*)p);
}

}

#endif /* XPP_MANAGED_PTR_HPP_ */
