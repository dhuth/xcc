/*
 * xi_semantic.hpp
 *
 *  Created on: Jan 24, 2018
 *      Author: derick
 */

#ifndef XCC_XI_XI_DOM_HPP_
#define XCC_XI_XI_DOM_HPP_

#include "xi_builder.hpp"
#include <stack>

namespace xcc {

struct dom_qname_resolver final : public xi_preorder_walker<> {
public:

    explicit inline dom_qname_resolver() noexcept {
        this->add(&dom_qname_resolver::resolve_id_type);
    }

private:

    ast_type* resolve_id_type(xi_id_type*, xi_builder&);

};

struct dom_assign_parent_walker final : public xi_preorder_walker<> {
public:

    explicit inline dom_assign_parent_walker() noexcept {
        //...
    }

protected:

    void begin(tree_type_id, ast_tree*, xi_builder&) override final;
    void end(tree_type_id, ast_tree*, xi_builder&) override final;

    void visit_xi_struct_decl(xi_struct_decl*,                          xi_builder&);
    void visit_xi_field_decl(xi_field_decl*,                            xi_builder&);
    void visit_xi_method_decl(xi_method_decl*,                          xi_builder&);
    void visit_xi_operator_method_decl(xi_operator_method_decl*,        xi_builder&);

};

}

#endif /* XCC_XI_XI_DOM_HPP_ */
