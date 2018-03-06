/*
 * xi_type.hpp
 *
 *  Created on: Feb 28, 2018
 *      Author: derick
 */

#ifndef XCC_XI_XI_TYPE_HPP_
#define XCC_XI_XI_TYPE_HPP_

#include "ast_type.hpp"

namespace xcc {

struct xi_type_provider : public ast_type_provider {
protected:

    ast_sametype_func*  create_sametype_func(const ast_builder_impl_t&) noexcept override;
    ast_widens_func*    create_widens_func(const ast_builder_impl_t&)   noexcept override;
    //ast_maxtype_func*       create_maxtype_func(const ast_builder_impl_t&)   noexcept;
    ast_cast_func*      create_cast_func(const ast_builder_impl_t&)     noexcept override;
    ast_typeset*        create_typeset(const ast_builder_impl_t&)       noexcept override;
};

}


#endif /* XCC_XI_XI_TYPE_HPP_ */
