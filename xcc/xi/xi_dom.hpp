/*
 * xi_semantic.hpp
 *
 *  Created on: Jan 24, 2018
 *      Author: derick
 */

#ifndef XCC_XI_XI_DOM_HPP_
#define XCC_XI_XI_DOM_HPP_

#include "xi_walker.hpp"
#include "xi_tree.hpp"

namespace xcc {

struct dom_qname_resolver final : public xi_preorder_walker<> {
public:

    explicit inline dom_qname_resolver() noexcept {
        this->add(&dom_qname_resolver::resolve_id_type);
    }

private:

    ast_type* resolve_id_type(xi_id_type*, xi_builder&);

};


struct dom_nesting_walker final : public xi_preorder_walker<> {
public:

    explicit inline dom_nesting_walker() noexcept {
        this->add(&dom_nesting_walker::visit_xi_namespace_decl);
        this->add(&dom_nesting_walker::visit_xi_member_decl);
        this->add(&dom_nesting_walker::visit_xi_function_decl);
    }

protected:

//    void begin(tree_type_id, ast_tree*, xi_builder&) override final;
//    void end(tree_type_id, ast_tree*, xi_builder&) override final;

    void visit_xi_namespace_decl(xi_namespace_decl*,                    xi_builder&);
    void visit_xi_member_decl(xi_member_decl*,                          xi_builder&);
    void visit_xi_function_decl(xi_function_decl*,                      xi_builder&);

};


struct dom_swap_decl_walker final : public xi_preorder_walker<> {
public:

    explicit inline dom_swap_decl_walker() noexcept {
        this->add(&dom_swap_decl_walker::visit_xi_decl_type);
    }

protected:

    void visit_xi_decl_type(xi_decl_type*,                              xi_builder&);
};

}

#endif /* XCC_XI_XI_DOM_HPP_ */
