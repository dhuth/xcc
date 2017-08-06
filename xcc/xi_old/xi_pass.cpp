/*
 * xi_pass.cpp
 *
 *  Created on: Mar 7, 2017
 *      Author: derick
 */


#include <xi_old/xi_builder.hpp>
#include <xi_old/xi_pass.hpp>

namespace xcc {

template<typename TBaseType>
static inline void begin_pass(__xi_pass<TBaseType>* pass, xi_builder& builder, tree_type_id id, ast_tree* t) {
    switch(id) {
    case tree_type_id::ast_namespace_decl: {
        builder.push_namespace(t->as<ast_namespace_decl>());
        pass->push_namespace(t->as<ast_namespace_decl>());
        break;
    }
    case tree_type_id::ast_block_stmt:      builder.push_block(t->as<ast_block_stmt>());            break;
    case tree_type_id::xi_struct_decl:      builder.push_member(t->as<xi_struct_decl>());           break;
    case tree_type_id::xi_class_decl:       builder.push_member(t->as<xi_class_decl>());            break;
    case tree_type_id::xi_mixin_decl:       builder.push_member(t->as<xi_mixin_decl>());            break;
    case tree_type_id::xi_method_decl:      builder.push_member(t->as<xi_method_decl>());           break;
    case tree_type_id::xi_constructor_decl: builder.push_member(t->as<xi_constructor_decl>());      break;
    case tree_type_id::xi_destructor_decl:  builder.push_member(t->as<xi_destructor_decl>());       break;
    case tree_type_id::xi_function_decl:    builder.push_function(t->as<xi_function_decl>());       break;
    }
}

void xi_preorder_pass::begin(tree_type_id id, ast_tree* t) {
    begin_pass(this, this->builder, id, t);
}

void xi_postorder_pass::begin(tree_type_id id, ast_tree* t) {
    begin_pass(this, this->builder, id, t);
}

template<typename TBaseType>
static inline void end_pass(__xi_pass<TBaseType>* pass, xi_builder& builder, tree_type_id id, ast_tree* t) {
    switch(id) {
    case tree_type_id::ast_namespace_decl: {
        pass->pop_namespace();
        break;
    }
    case tree_type_id::ast_block_stmt:
    case tree_type_id::xi_struct_decl:
    case tree_type_id::xi_class_decl:
    case tree_type_id::xi_mixin_decl:
    case tree_type_id::xi_method_decl:
    case tree_type_id::xi_constructor_decl:
    case tree_type_id::xi_destructor_decl:
    case tree_type_id::xi_function_decl:
        builder.pop();
        break;
    }
}

void xi_preorder_pass::end(tree_type_id id, ast_tree* t) {
    end_pass(this, this->builder, id, t);
}

void xi_postorder_pass::end(tree_type_id id, ast_tree* t) {
    end_pass(this, this->builder, id, t);
}

}

