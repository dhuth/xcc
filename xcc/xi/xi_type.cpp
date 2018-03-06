/*
 * xi_type.cpp
 *
 *  Created on: Mar 5, 2018
 *      Author: derick
 */

#include "xi_type.hpp"
#include "xi_type_func.hpp"

namespace xcc {

ast_cast_func* xi_type_provider::create_cast_func(const ast_builder_impl_t& b) noexcept {
    return new xi_cast_func(b);
}

ast_sametype_func* xi_type_provider::create_sametype_func(const ast_builder_impl_t& b) noexcept {
    return new xi_sametype_func(b);
}

ast_widens_func* xi_type_provider::create_widens_func(const ast_builder_impl_t& b) noexcept {
    return new xi_widens_func(b);
}

ast_typeset* xi_type_provider::create_typeset(const ast_builder_impl_t& b) noexcept {
    return new ast_typeset(this->get_sametype_func());
}

}



