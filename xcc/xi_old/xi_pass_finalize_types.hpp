/*
 * xi_pass_finalize_types.hpp
 *
 *  Created on: Mar 8, 2017
 *      Author: derick
 */

#ifndef XI_XI_PASS_FINALIZE_TYPES_HPP_
#define XI_XI_PASS_FINALIZE_TYPES_HPP_

#include "xi_pass.hpp"

namespace xcc {


struct xi_finalize_types_pass : public xi_postorder_pass {
public:

    inline xi_finalize_types_pass(xi_builder& builder)
            : xi_postorder_pass(builder) {
        this->add(&xi_finalize_types_pass::finalize_struct);

        //this->include({tree_type_id::ast_decl, tree_type_id::ast_type});
    }

    void finalize_struct(xi_struct_decl*);
    void finalize_class(xi_class_decl*);
    void finalize_mixin(xi_mixin_decl*);

};


}



#endif /* XI_XI_PASS_FINALIZE_TYPES_HPP_ */
