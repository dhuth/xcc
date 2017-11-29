/*
 * xi_lower.cpp
 *
 *  Created on: Nov 28, 2017
 *      Author: derick
 */

#include "xi_builder.hpp"

namespace xcc {


struct xi_lower_walker : public xi_postorder_walker {
public:

    inline xi_lower_walker() noexcept
            : xi_postorder_walker() {
        /* do nothing */
    }
};


void xi_builder::lower() noexcept {

    xi_lower_walker lowerf;
    lowerf.visit(this->global_namespace, *this);
}



}
