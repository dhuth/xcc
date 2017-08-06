/*
 * managed_ptr.cpp
 *
 *  Created on: Jan 23, 2017
 *      Author: derick
 */


#include "managed_ptr.hpp"
#include <algorithm>
#include <iomanip>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace xcc {

struct __managed_ptr_data {
    bool                                                        is_managed;
    int32_t                                                     refcount;
};

std::map<uintptr_t, __managed_ptr_data>                         __managed_pointers;

#ifdef DEV_TRACE_MANAGED_PTRS
std::vector<uintptr_t>                                          __watched_pointers;

bool __is_watched(void* p) {
    return std::find(__watched_pointers.begin(), __watched_pointers.end(), (uintptr_t)p) != __watched_pointers.end();
}

void __watch_pointer(void* p) {
    __watched_pointers.push_back((uintptr_t) p);
}

#endif

void dump_managed_ptr_list(std::ostream& s) {
    s << std::string(18, '-') << '\n';
    for(auto p: __managed_pointers) {
        s << std::hex       << std::setw(6) << p.first;
        s << std::boolalpha << std::setw(6) << p.second.is_managed;
        s << std::dec       << std::setw(6) << p.second.refcount;
        s << '\n';
    }
}

int32_t __refcount(void* p) {
    return __managed_pointers[(uintptr_t) p].refcount;
}

__ptr_impl::__ptr_impl(void* p, bool is_managed) noexcept: _internal_ptr(p) {
    if(p != nullptr) {
        uintptr_t address = (uintptr_t) p;
        if(__managed_pointers.find(address) == __managed_pointers.end()) {
            __managed_pointers[address] = {is_managed, 0};
        }
    }
}

void __ptr_impl::pin() const noexcept {
    if(this->_internal_ptr != nullptr) {
        uintptr_t address = (uintptr_t) this->_internal_ptr;
        __managed_pointers[address].is_managed = false;
    }
}

int32_t __ptr_impl::incr_ref() const noexcept {
    uintptr_t address = (uintptr_t) this->_internal_ptr;
    if(this->_internal_ptr != nullptr && __managed_pointers[address].is_managed) {
        return ++__managed_pointers[address].refcount;
    }
    return 1;
}

int32_t __ptr_impl::decr_ref() const noexcept {
    uintptr_t address = (uintptr_t) this->_internal_ptr;
    if(this->_internal_ptr != nullptr && __managed_pointers[address].is_managed) {
        return --__managed_pointers[address].refcount;
    }
    return 1;
}

bool is_pinned(void* p) {
    if(p == nullptr) return false;

    uintptr_t address = (uintptr_t)p;
    if(__managed_pointers.find(address) != __managed_pointers.end()) {
        return !__managed_pointers[address].is_managed;
    }

    return false;
}

}

