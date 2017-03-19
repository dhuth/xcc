/*
 * xi_mangler.hpp
 *
 *  Created on: Mar 18, 2017
 *      Author: derick
 */

#ifndef XI_XI_MANGLER_HPP_
#define XI_XI_MANGLER_HPP_

#include "xi_tree.hpp"
#include "ast_builder.hpp"

//itanium-cxx-abi.github.io/cxx-abi/abi.html

namespace xcc {

struct itanium_cxxabi_mangler : public ast_default_name_mangler {
public:

    inline itanium_cxxabi_mangler() noexcept : ast_default_name_mangler() {
        this->addmethod(&itanium_cxxabi_mangler::mangle_xi_function_decl);
        this->addmethod(&itanium_cxxabi_mangler::mangle_xi_const_type);
        this->addmethod(&itanium_cxxabi_mangler::mangle_xi_ref_type);
        this->addmethod(&itanium_cxxabi_mangler::mangle_xi_object_type);
    }

    std::string                                     mangle_xi_function_decl(xi_function_decl* decl);

    std::string                                     mangle_xi_ref_type(xi_ref_type*);
    std::string                                     mangle_xi_const_type(xi_const_type*);
    std::string                                     mangle_xi_object_type(xi_object_type*);
};



}



#endif /* XI_XI_MANGLER_HPP_ */
