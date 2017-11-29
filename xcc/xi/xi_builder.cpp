/*
 * xi_builder.cpp
 *
 *  Created on: Aug 20, 2017
 *      Author: derick
 */

#include "xi_builder.hpp"

namespace xcc {

ast_type* xi_builder::get_auto_type() const noexcept {
    return this->_the_auto_type;
}

ast_type* xi_builder::get_const_type(ast_type* type) noexcept {
    auto newtype = box<ast_type>(new xi_const_type(type));
    if(this->_const_types.find(newtype) == this->_const_types.end()) {
        this->_const_types[newtype] = newtype;
        return newtype;
    }
    else {
        return this->_const_types[newtype];
    }
}

ast_type* xi_builder::get_reference_type(ast_type* type) noexcept {
    auto newtype = box<ast_type>(new xi_reference_type(type));
    if(this->_reference_types.find(newtype) == this->_reference_types.end()) {
        this->_reference_types[newtype] = newtype;
        return newtype;
    }
    else {
        return this->_reference_types[newtype];
    }
}

ast_type* xi_builder::get_object_type(xi_decl* decl) noexcept {
    auto newtype = box<ast_type>(new xi_object_type(decl));
    if(this->_object_types.find(newtype) == this->_object_types.end()) {
        this->_object_types[newtype] = newtype;
        return newtype;
    }
    else {
        return this->_object_types[newtype];
    }
}

ast_type* xi_builder::get_declaration_type(ast_decl* decl) noexcept {
    switch(decl->get_tree_type()) {
    case tree_type_id::xi_parameter_decl:
        return decl->as<xi_parameter_decl>()->type;
    default:
        return __ast_builder_impl::get_declaration_type(decl);
    }
}

ast_type* xi_builder::get_tuple_type(list<ast_type>* types) noexcept {
    auto newtype = box<ast_type>(new xi_tuple_type(types));
    if(this->_tuple_types.find(newtype) == this->_tuple_types.end()) {
        this->_tuple_types[newtype] = newtype;
        return newtype;
    }
    else {
        return this->_tuple_types[newtype];
    }
}

ast_type* xi_builder::get_id_type(const char* name) const noexcept {
    return new xi_id_type(name);
}


xi_function_decl* xi_builder::make_xi_function_decl(const char* name, ast_type* return_type, list<xi_parameter_decl>* parameters, ast_stmt* body) const noexcept {
    return new xi_function_decl(name, return_type, parameters, body);
}

xi_parameter_decl* xi_builder::make_xi_parameter_decl(const char* name, ast_type* type) const noexcept {
    return new xi_parameter_decl(name, type);
}


ast_expr* xi_builder::make_xi_id_expr(const char* name) const noexcept {
    return new xi_id_expr(name);
}

ast_expr* xi_builder::make_xi_member_id_expr(ast_expr* e, const char* name) const noexcept {
    return new xi_member_id_expr(e, name);
}

ast_expr* xi_builder::make_xi_deref_member_id_expr(ast_expr* e, const char* name) const noexcept {
    return new xi_member_id_expr(copyloc(this->make_xi_op(xi_op_expr::xi_operator::__deref__, e), e), name);
}

ast_expr* xi_builder::make_xi_tuple_expr(list<ast_expr>* t) const noexcept {
    return new xi_tuple_expr(t);
}

ast_expr* xi_builder::make_xi_op(xi_op_expr::xi_operator op, ast_expr* p) const noexcept {
    return new xi_op_expr(op, new list<ast_expr>{p});
}

ast_expr* xi_builder::make_xi_op(xi_op_expr::xi_operator op, ast_expr* l, ast_expr* r) const noexcept {
    return new xi_op_expr(op, new list<ast_expr>{l, r});
}

ast_expr* xi_builder::make_xi_op(xi_op_expr::xi_operator op, list<ast_expr>* elist) const noexcept {
    return new xi_op_expr(op, elist);
}

ast_stmt* xi_builder::make_xi_if_stmt(ast_expr* expr, ast_stmt* tstmt, ast_stmt* fstmt) const noexcept {
    return new ast_if_stmt(expr, tstmt, fstmt);
}

ast_stmt* xi_builder::make_xi_while_stmt(ast_expr* expr, ast_stmt* stmt) const noexcept {
    return new ast_while_stmt(expr, stmt);
}

ast_stmt* xi_builder::make_xi_return_stmt(ast_expr* expr) const noexcept {
    return new ast_return_stmt(expr);
}

void xi_builder::push_xi_function(xi_function_decl* decl) noexcept {
    this->push_context<xi_function_context>(decl);
}



static inline void walker_begin(tree_type_id i, ast_tree* t, xi_builder& b) {
    switch(i) {
    case tree_type_id::ast_namespace_decl:
        b.push_namespace(t->as<ast_namespace_decl>());
        break;
    case tree_type_id::ast_block_stmt:
        b.push_block(t->as<ast_block_stmt>());
        break;
    case tree_type_id::xi_function_decl:
        b.push_xi_function(t->as<xi_function_decl>());
        break;
    }
}

static inline void walker_end(tree_type_id i, ast_tree* t, xi_builder& b) {
    switch(i) {
    case tree_type_id::ast_namespace_decl:
    case tree_type_id::ast_block_stmt:
    case tree_type_id::xi_function_decl:
        b.pop();
        break;
    }
}

void xi_postorder_walker::begin(tree_type_id i, ast_tree* t, xi_builder& b) { walker_begin(i, t, b); }
void xi_postorder_walker::end(tree_type_id i, ast_tree* t, xi_builder& b)   { walker_end(i, t, b);   }

}
