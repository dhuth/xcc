/*
 * xi_builder.hpp
 *
 *  Created on: Feb 1, 2017
 *      Author: derick
 */

#ifndef XI_XI_BUILDER_HPP_
#define XI_XI_BUILDER_HPP_

#include "ast_builder.hpp"

namespace xcc {

struct xi_builder : public ast_builder<> {
    xi_builder() = default;
    virtual ~xi_builder() = default;
};


}



#endif /* XI_XI_BUILDER_HPP_ */
