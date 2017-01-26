/*
 * managed_ptr.cpp
 *
 *  Created on: Jan 23, 2017
 *      Author: derick
 */


#include "managed_ptr.hpp"
#include <map>
#include <utility>

namespace xcc {

struct __managed_ptr_data {
    bool                                                        is_managed;
    int32_t                                                     refcount;
};

std::map<uintptr_t, __managed_ptr_data>                         __managed_pointers;

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
        __managed_pointers[address].is_managed = true;
    }
}

uint32_t __ptr_impl::incr_ref() const noexcept {
    uintptr_t address = (uintptr_t) this->_internal_ptr;
    if(this->_internal_ptr != nullptr && __managed_pointers[address].is_managed) {
        return ++__managed_pointers[address].refcount;
    }
    return 1;
}

uint32_t __ptr_impl::decr_ref() const noexcept {
    uintptr_t address = (uintptr_t) this->_internal_ptr;
    if(this->_internal_ptr != nullptr && __managed_pointers[address].is_managed) {
        return --__managed_pointers[address].refcount;
    }
    return 1;
}

}

