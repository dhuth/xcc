/*
 * xi_builder_function.hpp
 *
 *  Created on: Mar 6, 2017
 *      Author: derick
 */

#ifndef XI_XI_BUILDER_FUNCTION_HPP_
#define XI_XI_BUILDER_FUNCTION_HPP_

#include "xi_tree.hpp"
#include "ast_context.hpp"

namespace xcc {

struct xi_function_context : public ast_context {
public:

    xi_function_context(ast_context* parent, xi_function_decl* func);
    virtual ~xi_function_context() = default;

    void insert(const char*, ast_decl*) final override; //TODO: just throw error
    ast_type* get_return_type() final override;

protected:

    ptr<ast_decl> find_first_impl(const char*) final override;
    void find_all_impl(ptr<list<ast_decl>>, const char*) final override;

private:

    ptr<xi_function_decl>                                               _func;

};

}



#endif /* XI_XI_BUILDER_FUNCTION_HPP_ */
