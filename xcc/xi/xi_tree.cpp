/*
 * xi_tree.cpp
 *
 *  Created on: Nov 9, 2017
 *      Author: derick
 */

#include "xi_tree.hpp"


namespace xcc {

bool xi_type_comparer::operator()(ast_type* const& lhs, ast_type* const& rhs) const {
    if(lhs->type_id != rhs->type_id) {
        return false;
    }

    switch(lhs->type_id) {
    case tree_type_id::xi_id_type:
        {
            auto xilhs = lhs->as<xi_id_type>();
            auto xirhs = rhs->as<xi_id_type>();

            return
                    ((std::string) xilhs->name) == ((std::string) xirhs->name);
        }
    case tree_type_id::xi_const_type:
        {
            auto xilhs = lhs->as<xi_const_type>();
            auto xirhs = rhs->as<xi_const_type>();

            return
                    this->operator()(xilhs->type, xirhs->type);
        }
        break;
    case tree_type_id::xi_array_type:
        {
            auto xilhs = lhs->as<xi_array_type>();
            auto xirhs = rhs->as<xi_array_type>();

            return
                    this->operator()(xilhs->type, xirhs->type); //TODO: ???
        }
        break;
    case tree_type_id::xi_auto_type:
        {
            return true;
        }
        break;
    case tree_type_id::xi_reference_type:
        {
            auto xilhs = lhs->as<xi_reference_type>();
            auto xirhs = rhs->as<xi_reference_type>();

            return
                    this->operator()(xilhs->type, xirhs->type);
        }
        break;
    case tree_type_id::xi_tuple_type:
        {
            auto xilhs = lhs->as<xi_tuple_type>();
            auto xirhs = rhs->as<xi_tuple_type>();

            return
                    this->same_typelist(xilhs->types, xirhs->types);
        }
        break;
    case tree_type_id::xi_object_type:
        {
            auto xilhs = lhs->as<xi_object_type>();
            auto xirhs = rhs->as<xi_object_type>();

            return
                    xilhs->declaration == xirhs->declaration; //TODO: should be smarter
        }
        break;
    default:
        return ast_type_comparer::operator ()(lhs, rhs);
    }
}

size_t xi_type_hasher::operator()(ast_type* const& tp) const {
    size_t prefix = (size_t) tp->get_tree_type() << 8;

    switch(tp->get_tree_type()) {
    case tree_type_id::xi_id_type:
        return prefix;
    case tree_type_id::xi_const_type:
        return prefix | this->operator()(tp->as<xi_const_type>()->type);
    case tree_type_id::xi_array_type:
        return prefix | this->operator()(tp->as<xi_array_type>()->type);
    case tree_type_id::xi_auto_type:
        return prefix;
    case tree_type_id::xi_reference_type:
        return prefix | this->operator()(tp->as<xi_reference_type>()->type);
    case tree_type_id::xi_tuple_type:
        return prefix | this->hash_typelist(tp->as<xi_tuple_type>()->types);
    case tree_type_id::xi_object_type:
        return prefix; // TODO: ???
    default:
        return ast_type_hasher::operator()(tp);
    }
}

}

