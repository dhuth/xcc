/*
 * xi_builder.cpp
 *
 *  Created on: Aug 20, 2017
 *      Author: derick
 */

#include "xi_builder.hpp"
#include "xi_context.hpp"
#include "error.hpp"

namespace xcc {

xi_auto_type* xi_builder::get_auto_type() const noexcept {
    return this->_the_auto_type;
}

xi_const_type* xi_builder::get_const_type(ast_type* type) noexcept {
    return this->_const_types.get_new_as<xi_const_type>(type);
}

xi_reference_type* xi_builder::get_reference_type(ast_type* type) noexcept {
    return this->_reference_types.get_new_as<xi_reference_type>(type);
}

xi_decl_type* xi_builder::get_object_type(ast_decl* decl) noexcept {
    return this->_object_types.get_new_as<xi_decl_type>(decl);
}

xi_tuple_type* xi_builder::get_tuple_type(list<ast_type>* types) noexcept {
    return this->_tuple_types.get_new_as<xi_tuple_type>(types);
}

ast_type* xi_builder::get_declaration_type(ast_decl* decl) noexcept {
    switch(decl->get_tree_type()) {
    case tree_type_id::xi_parameter_decl:                   return decl->as<xi_parameter_decl>()->type;
    case tree_type_id::xi_field_decl:                       return decl->as<xi_field_decl>()->type;
    default:
        return __ast_builder_impl::get_declaration_type(decl);
    }
}

ast_type* xi_builder::get_id_type(xi_qname* name) const noexcept {
    return new xi_id_type(name);
}

ast_namespace_decl* xi_builder::make_namespace_decl(const char* name, list<ast_decl>* declarations) const noexcept {
    return new xi_namespace_decl(std::string(name), declarations);
}

xi_function_decl* xi_builder::make_xi_function_decl(const char* name, ast_type* return_type, list<xi_parameter_decl>* parameters, ast_stmt* body) const noexcept {
    return new xi_function_decl(name, return_type, parameters, body);
}

xi_operator_function_decl* xi_builder::make_xi_operator_function_decl(xi_operator op, ast_type* return_type, list<xi_parameter_decl>* parameters, ast_stmt* body) const noexcept {
    return new xi_operator_function_decl(std::to_string(op).c_str(), op, return_type, parameters, body);
}

xi_method_decl* xi_builder::make_xi_method_decl(const char* name, ast_type* return_type, list<xi_parameter_decl>* parameters, ast_stmt* body) const noexcept {
    return new xi_method_decl(name, return_type, parameters, body);
}

xi_operator_method_decl* xi_builder::make_xi_operator_method_decl(xi_operator op, ast_type* return_type, list<xi_parameter_decl>* parameters, ast_stmt* body) const noexcept {
    return new xi_operator_method_decl(std::to_string(op).c_str(), op, return_type, parameters, body);
}

xi_parameter_decl* xi_builder::make_xi_parameter_decl(const char* name, ast_type* type) const noexcept {
    return new xi_parameter_decl(name, type);
}

xi_struct_decl* xi_builder::make_xi_struct_decl(const char* name, list<xi_member_decl>* members) const noexcept {
    return new xi_struct_decl(name, members);
}

xi_field_decl* xi_builder::make_xi_field_decl(const char* name, ast_type* type) const noexcept {
    return new xi_field_decl(name, type);
}

ast_expr* xi_builder::make_xi_id_expr(xi_qname* name) const noexcept {
    return new xi_id_expr(name);
}

ast_expr* xi_builder::make_xi_member_id_expr(ast_expr* e, xi_qname* name) const noexcept {
    return new xi_member_id_expr(e, name);
}

ast_expr* xi_builder::make_xi_deref_member_id_expr(ast_expr* e, xi_qname* name) const noexcept {
    //TODO: member pointer operator ???
    return new xi_member_id_expr(copyloc(this->make_xi_op(xi_op_expr::xi_operator::__deref__, e), e), name);
}

ast_expr* xi_builder::make_xi_tuple_expr(list<ast_expr>* t) const noexcept {
    return new xi_tuple_expr(t);
}

ast_expr* xi_builder::make_xi_op(xi_op_expr::xi_operator op, ast_expr* p) const noexcept {
    return copyloc(new xi_op_expr(op, new list<ast_expr>{p}), p);
}

ast_expr* xi_builder::make_xi_op(xi_op_expr::xi_operator op, ast_expr* l, ast_expr* r) const noexcept {
    return copyloc(new xi_op_expr(op, new list<ast_expr>{l, r}), l, r);
}

ast_expr* xi_builder::make_xi_op(xi_op_expr::xi_operator op, list<ast_expr>* elist) const noexcept {
    return new xi_op_expr(op, elist);
}

ast_expr* xi_builder::make_xi_invoke(ast_expr* e, list<ast_expr>* args) const noexcept {
    return new xi_invoke_expr(e, args);
}

ast_expr* xi_builder::make_xi_index(ast_expr* e, list<ast_expr>* args) const noexcept {
    return new xi_index_expr(e, args);
}

ast_expr* xi_builder::make_xi_member_expr(ast_expr* expr, xi_member_decl* member) noexcept {
    if(expr->type->is<xi_reference_type>()) {
        return this->make_xi_member_expr(
                this->widen(expr->type->as<xi_reference_type>()->type, expr),
                member);
    }
    return copyloc(new xi_member_expr(this->get_declaration_type(member), expr, member), expr);
}

ast_expr* xi_builder::make_xi_deref_expr(ast_expr* e) const noexcept {
    assert(e->type->is<xi_reference_type>());
    return copyloc(new xi_deref_expr(e->type->as<xi_reference_type>()->type, e), e);
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
    this->context = this->context->push_context<xi_function_context>(decl);
    _nesting_function_decls.push(decl);
}

void xi_builder::leave_xi_function() noexcept {
    this->pop_context();
    _nesting_function_decls.pop();
}

void xi_builder::push_xi_struct(xi_struct_decl* s) noexcept {
    this->context = this->context->push_context<xi_struct_context>(s);
    _nesting_types.push(s);
}

void xi_builder::leave_xi_struct() noexcept {
    this->pop_context();
    _nesting_types.pop();
}

void xi_builder::push_xi_method(xi_method_decl* m) noexcept {
    this->context = this->context->push_context<xi_method_context>(m);
    _nesting_function_decls.push(m);
}

void xi_builder::leave_xi_method() noexcept {
    this->pop_context();
    _nesting_function_decls.pop();
}

void maybe_push_context(ast_tree* t, xi_builder& b) {
    switch(t->get_tree_type()) {
    case tree_type_id::ast_namespace_decl:              b.push_namespace(t->as<ast_namespace_decl>());  break;
    case tree_type_id::xi_namespace_decl:               b.push_namespace(t->as<xi_namespace_decl>());   break;
    case tree_type_id::ast_block_stmt:                  b.push_block(t->as<ast_block_stmt>());          break;
    case tree_type_id::ast_function_decl:               b.push_function(t->as<ast_function_decl>());    break;
    case tree_type_id::xi_function_decl:                b.push_xi_function(t->as<xi_function_decl>());  break;
    case tree_type_id::xi_operator_function_decl:       b.push_xi_function(t->as<xi_function_decl>());  break;
    case tree_type_id::xi_struct_decl:                  b.push_xi_struct(t->as<xi_struct_decl>());      break;
    case tree_type_id::xi_method_decl:                  b.push_xi_method(t->as<xi_method_decl>());      break;
    case tree_type_id::xi_operator_method_decl:         b.push_xi_method(t->as<xi_method_decl>());      break;
    }
}

void maybe_pop_context(ast_tree* t, xi_builder& b) {
    switch(t->get_tree_type()) {
    case tree_type_id::ast_namespace_decl:              b.pop_context();                                break;
    case tree_type_id::xi_namespace_decl:               b.pop_context();                                break;
    case tree_type_id::ast_block_stmt:                  b.pop_context();                                break;
    case tree_type_id::ast_function_decl:               b.pop_context();                                break;
    case tree_type_id::xi_function_decl:                b.leave_xi_function();                          break;
    case tree_type_id::xi_operator_function_decl:       b.leave_xi_function();                          break;
    case tree_type_id::xi_struct_decl:                  b.leave_xi_struct();                            break;
    case tree_type_id::xi_method_decl:                  b.leave_xi_method();                            break;
    case tree_type_id::xi_operator_method_decl:         b.leave_xi_method();                            break;
    }
}

}
