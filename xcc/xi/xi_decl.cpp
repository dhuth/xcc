/*
 * xi_decl.cpp
 *
 *  Created on: Mar 7, 2018
 *      Author: derick
 */

#include "xi_builder.hpp"
#include "xi_decl.hpp"
#include "error.hpp"

namespace xcc {


static inline bool in_same_namespace(xi_namespace_member_base* lhs, xi_namespace_member_base* rhs) {
    return lhs->ns == rhs->ns;
}

static inline bool in_same_type(xi_member_decl* lhs, xi_member_decl* rhs, const xi_builder& b) {
    if(lhs->parent == lhs->parent)              return true;
    if(b.sametype(lhs->parent, rhs->parent))    return true;
    return false;
}

static bool same_struct(xi_struct_decl* lhs, xi_struct_decl* rhs, const xi_builder& b) {
    if(!in_same_namespace(lhs, rhs))                    return false;
    if(!in_same_type(lhs, rhs, b))                      return false;
    //TODO: generic parameters
    return true;
}

static bool same_field(xi_field_decl* lhs, xi_field_decl* rhs, const xi_builder& b) {
    if(!in_same_type(lhs, rhs, b))                      return false;
    return true;
}

static bool same_rt_parameters_varargs(xi_function_base* lhs, xi_function_base* rhs, const xi_builder& b) {
    if(!b.sametype(lhs->return_type, rhs->return_type))         return false;
    if(!(lhs->is_vararg == rhs->is_vararg))                     return false;
    if(!(lhs->parameters->size() == rhs->parameters->size()))   return false;

    auto lhs_iter           = begin(lhs->parameters);
    auto lhs_iter_end       = end(lhs->parameters);
    auto rhs_iter           = begin(rhs->parameters);

    while(lhs_iter != lhs_iter_end) {
        auto    lp = *lhs_iter;
        auto    rp = *rhs_iter;

        if(!b.sametype(lp->type, rp->type)) {
            return false;
        }

        lhs_iter++;
        rhs_iter++;
    }
    return true;
}

static bool same_function(xi_function_decl* lhs, xi_function_decl* rhs, const xi_builder& b) {
    if(!in_same_namespace(lhs, rhs))                    return false;
    return same_rt_parameters_varargs(lhs, rhs, b);
}

static bool same_method(xi_method_decl* lhs, xi_method_decl* rhs, const xi_builder& b) {
    if(!in_same_namespace(lhs, rhs))                    return false;
    if(!in_same_type(lhs, rhs, b))                      return false;
    return same_rt_parameters_varargs(lhs, rhs, b);
}


bool xi_builder::samedecl(ast_decl* lhs, ast_decl* rhs) const noexcept {
    //TODO: remove alias
    if(lhs == rhs)                                      return true;
    if(lhs->get_tree_type() != rhs->get_tree_type())    return false;   //TODO: might not be right
    if(lhs->name            != rhs->name)               return false;

    switch(lhs->get_tree_type()) {
    case tree_type_id::xi_struct_decl:
        return same_struct(
                lhs->as<xi_struct_decl>(),
                rhs->as<xi_struct_decl>(), *this);
    case tree_type_id::xi_field_decl:
        return same_field(
                lhs->as<xi_field_decl>(),
                rhs->as<xi_field_decl>(), *this);
    case tree_type_id::xi_function_decl:
    case tree_type_id::xi_operator_function_decl:
        return same_function(
                    lhs->as<xi_function_decl>(),
                    rhs->as<xi_function_decl>(), *this);
    case tree_type_id::xi_method_decl:
    case tree_type_id::xi_operator_method_decl:
        return same_method(
                lhs->as<xi_method_decl>(),
                rhs->as<xi_method_decl>(), *this);
    default:
        __throw_unhandled_tree_type(__FILE__, __LINE__, lhs, "same_decl");
    }
}

ast_type* xi_builder::get_declaration_type(ast_decl* decl) noexcept {
    switch(decl->get_tree_type()) {
    case tree_type_id::xi_parameter_decl:                   return decl->as<xi_parameter_decl>()->type;
    case tree_type_id::xi_field_decl:                       return decl->as<xi_field_decl>()->type;
    case tree_type_id::xi_function_decl:
    case tree_type_id::xi_operator_function_decl:
        return this->get_function_type(
                decl->as<xi_function_decl>()->return_type,
                map(decl->as<xi_function_decl>()->parameters,
                        [&](xi_parameter_decl* d)->ast_type* {
                            return this->get_declaration_type(d);
        }), decl->as<xi_function_decl>()->is_vararg);
    case tree_type_id::xi_method_decl:
    case tree_type_id::xi_operator_method_decl:
        return this->get_function_type(
                decl->as<xi_method_decl>()->return_type,
                map(decl->as<xi_method_decl>()->parameters,
                        [&](xi_parameter_decl* d)->ast_type* {
                            return this->get_declaration_type(d);
        }), decl->as<xi_method_decl>()->is_vararg);
    default:
        return __ast_builder_impl::get_declaration_type(decl);
    }
}


}

