/*
 * xi_name_mangler.hpp
 *
 *  Created on: Feb 6, 2018
 *      Author: derick
 */

#ifndef XCC_XI_XI_NAME_MANGLER_HPP_
#define XCC_XI_XI_NAME_MANGLER_HPP_

#include "ast_builder.hpp"
#include "xi_tree.hpp"

namespace xcc {

struct xi_name_mangler : public ast_default_name_mangler {
public:
    explicit inline xi_name_mangler() noexcept
            : ast_default_name_mangler() {

        this->addmethod(&xi_name_mangler::mangle_xi_namespace_decl);
        this->addmethod(&xi_name_mangler::mangle_xi_function_decl);
        this->addmethod(&xi_name_mangler::mangle_xi_operator_function_decl);
        this->addmethod(&xi_name_mangler::mangle_xi_method_decl);
        this->addmethod(&xi_name_mangler::mangle_xi_operator_method_decl);
        //this->addmethod(&xi_name_mangler::mangle_xi_struct_decl);
    }

private:

    std::string     mangle_xi_namespace_decl(xi_namespace_decl*);
    std::string     mangle_xi_function_decl(xi_function_decl*);
    std::string     mangle_xi_operator_function_decl(xi_operator_function_decl*);
    std::string     mangle_xi_method_decl(xi_method_decl*);
    std::string     mangle_xi_operator_method_decl(xi_operator_method_decl*);
    std::string     mangle_xi_struct_decl(xi_struct_decl*);

};

}



#endif /* XCC_XI_XI_NAME_MANGLER_HPP_ */
