/*
 * xi_semantic.hpp
 *
 *  Created on: Jan 24, 2018
 *      Author: derick
 */

#ifndef XCC_XI_XI_DOM_HPP_
#define XCC_XI_XI_DOM_HPP_

#include "xi_builder.hpp"
#include "error.hpp"

namespace xcc {

struct dom_qname_resolver final : public xi_preorder_walker<> {
public:

    explicit inline dom_qname_resolver()
            : xi_preorder_walker<>() {
        this->add(&dom_qname_resolver::resolve_id_type);
    }

private:

    ast_type* resolve_id_type(xi_id_type*, xi_builder&);

};


}

#endif /* XCC_XI_XI_DOM_HPP_ */
