/*
 * xi_builder_struct.hpp
 *
 *  Created on: Mar 6, 2017
 *      Author: derick
 */

#ifndef XI_XI_BUILDER_STRUCT_HPP_
#define XI_XI_BUILDER_STRUCT_HPP_

#include "../xi_old/xi_tree.hpp"
#include "ast_context.hpp"

namespace xcc {


struct xi_struct_context : public ast_context {
public:

    xi_struct_context(ast_context* parent, xi_struct_decl* struct_decl);
    virtual ~xi_struct_context() = default;

    void insert(const char*, ast_decl*) final override;
    ast_type* get_return_type();

protected:

    ptr<ast_decl> find_first_impl(const char*) final override;
    void find_all_impl(ptr<list<ast_decl>>, const char*) final override;

    ptr<xi_struct_decl>                                                 _struct;
};

}



#endif /* XI_XI_BUILDER_STRUCT_HPP_ */
