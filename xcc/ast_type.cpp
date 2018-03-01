/*
 * ast_type.cpp
 *
 *  Created on: Feb 28, 2018
 *      Author: derick
 */

#include "ast_type.hpp"

namespace xcc {

void ast_type_provider::initialize(const ast_builder_impl_t& b) noexcept {
    _sametype_ptr       = this->create_sametype_func(b);
    _widens_ptr         = this->create_widens_func(b);
    _maxtype_ptr        = this->create_maxtype_func(b);
    _cast_ptr           = this->create_cast_func(b);
    _typeset_ptr        = this->create_typeset(b);
}

ast_sametype_func* ast_type_provider::get_sametype_func() const noexcept {
    return _sametype_ptr;
}
ast_widens_func* ast_type_provider::get_widens_func() const noexcept {
    return _widens_ptr;
}
ast_maxtype_func* ast_type_provider::get_maxtype_func() const noexcept {
    return _maxtype_ptr;
}
ast_cast_func* ast_type_provider::get_cast_func() const noexcept {
    return _cast_ptr;
}
ast_typeset* ast_type_provider::get_typeset() const noexcept {
    return _typeset_ptr;
}


ast_sametype_func* ast_type_provider::create_sametype_func(const ast_builder_impl_t& b) noexcept {
    return new ast_sametype_func(b);
}
ast_widens_func* ast_type_provider::create_widens_func(const ast_builder_impl_t& b) noexcept {
    return new ast_widens_func(b);
}
ast_maxtype_func* ast_type_provider::create_maxtype_func(const ast_builder_impl_t& b) noexcept {
    return new ast_maxtype_func(b);
}
ast_cast_func* ast_type_provider::create_cast_func(const ast_builder_impl_t& b) noexcept {
    return new ast_cast_func(b);
}
ast_typeset* ast_type_provider::create_typeset(const ast_builder_impl_t& b) noexcept {
    return new ast_typeset(_sametype_ptr);
}

}


