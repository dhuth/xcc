/*
 * ast.cpp
 *
 *  Created on: Aug 27, 2017
 *      Author: derick
 */


#include "ast.hpp"

namespace xcc {

bool ast_type_comparer::same_typelist(list<ast_type>* lhs, list<ast_type>* rhs) const noexcept {
    if(lhs->size() != rhs->size()) {
        return false;
    }

    for(size_t i = 0; i < lhs->size(); i++) {
        if(!this->operator() ((*lhs)[i], (*rhs)[i])) {
            return false;
        }
    }
    return true;
}

bool ast_type_comparer::operator()(ast_type* const& lhs, ast_type* const& rhs) const {
    if(!(lhs->get_tree_type() != rhs->get_tree_type())) {
        return false;
    }

    switch(lhs->get_tree_type()) {
    case tree_type_id::ast_void_type:
        return true;
    case tree_type_id::ast_integer_type:
        {
            auto lhs_width      = (uint32_t)    lhs->as<ast_integer_type>()->bitwidth;
            auto lhs_unsigned   = (bool)        lhs->as<ast_integer_type>()->is_unsigned;
            auto rhs_width      = (uint32_t)    rhs->as<ast_integer_type>()->bitwidth;
            auto rhs_unsigned   = (bool)        rhs->as<ast_integer_type>()->is_unsigned;

            return lhs_width == rhs_width &&
                    lhs_unsigned == rhs_unsigned;
        }
    case tree_type_id::ast_real_type:
        {
            auto lhs_width      = (uint32_t)    lhs->as<ast_real_type>()->bitwidth;
            auto rhs_width      = (uint32_t)    rhs->as<ast_real_type>()->bitwidth;

            return lhs_width == rhs_width;
        }
    case tree_type_id::ast_pointer_type:
        {
            auto lhs_type       = (ast_type*)   lhs->as<ast_pointer_type>()->element_type;
            auto rhs_type       = (ast_type*)   rhs->as<ast_pointer_type>()->element_type;

            return this->operator ()(lhs_type, rhs_type);
        }
    case tree_type_id::ast_array_type:
        {
            auto lhs_type       = (ast_type*)   lhs->as<ast_array_type>()->element_type;
            auto lhs_size       = (uint32_t)    lhs->as<ast_array_type>()->size;
            auto rhs_type       = (ast_type*)   rhs->as<ast_array_type>()->element_type;
            auto rhs_size       = (uint32_t)    rhs->as<ast_array_type>()->size;

            return this->operator ()(lhs_type, rhs_type) &&
                    lhs_size == rhs_size;
        }
    case tree_type_id::ast_record_type:
        {
            auto lhs_type_list  = (list<ast_type>*)    lhs->as<ast_record_type>()->field_types;
            auto rhs_type_list  = (list<ast_type>*)    rhs->as<ast_record_type>()->field_types;

            return this->same_typelist(lhs_type_list, rhs_type_list);
        }
    case tree_type_id::ast_function_type:
        {
            auto lhs_rtype      = (ast_type*)           lhs->as<ast_function_type>()->return_type;
            auto lhs_ptypes     = (list<ast_type>*)     lhs->as<ast_function_type>()->parameter_types;
            auto rhs_rtype      = (ast_type*)           rhs->as<ast_function_type>()->return_type;
            auto rhs_ptypes     = (list<ast_type>*)     rhs->as<ast_function_type>()->parameter_types;

            return
                    this->operator()(lhs_rtype, rhs_rtype) &&
                    this->same_typelist(lhs_ptypes, rhs_ptypes);
        }
    default:
        throw std::runtime_error(std::string("ast.cpp: ast_type_comparer::operator(): ") +
                "unsuported type " + lhs->get_tree_type_name());
    }
}

size_t ast_type_hasher::hash_typelist(list<ast_type>* list) const noexcept {
    size_t h = 0;
    for(auto t: *list) {
        h += this->operator()(t);
    }
    return h;
}

size_t ast_type_hasher::operator()(ast_type* const& tp) const {
    switch(tp->get_tree_type()) {
    case tree_type_id::ast_void_type:
        return (size_t) tree_type_id::ast_void_type;
    case tree_type_id::ast_integer_type:
        return
                ((size_t) tree_type_id::ast_integer_type) << 8 |
                ((size_t) tp->as<ast_integer_type>()->bitwidth) << 1 |
                ((bool)   tp->as<ast_integer_type>()->is_unsigned) ? 1 : 0;
    case tree_type_id::ast_real_type:
        return
                ((size_t) tree_type_id::ast_real_type) << 8 |
                ((size_t) tp->as<ast_real_type>()->bitwidth) ;
    case tree_type_id::ast_pointer_type:
        return
                ((size_t) tree_type_id::ast_pointer_type) << 8 |
                this->operator()(tp->as<ast_pointer_type>()->element_type);
    case tree_type_id::ast_array_type:
        return
                ((size_t) tree_type_id::ast_array_type) << 8 |
                this->operator()(tp->as<ast_array_type>()->element_type);
    case tree_type_id::ast_record_type:
        return
                ((size_t) tree_type_id::ast_record_type) << 8 |
                this->hash_typelist(tp->as<ast_record_type>()->field_types);
    case tree_type_id::ast_function_type:
        return
                ((size_t) tree_type_id::ast_function_type) << 8 |
                this->operator()(tp->as<ast_function_type>()->return_type) << 16 |
                this->hash_typelist(tp->as<ast_function_type>()->parameter_types);
    default:
        throw std::runtime_error(std::string("ast.cpp: ast_type_hasher::operator(): ") +
                "unsuported type " + tp->get_tree_type_name());
    }
}

}
