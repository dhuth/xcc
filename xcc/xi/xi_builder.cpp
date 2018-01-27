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

ast_type* xi_builder::get_auto_type() const noexcept {
    return this->_the_auto_type;
}

ast_type* xi_builder::get_const_type(ast_type* type) noexcept {
    return this->_const_types.get_new<xi_const_type>(type);
}

ast_type* xi_builder::get_reference_type(ast_type* type) noexcept {
    return this->_reference_types.get_new<xi_reference_type>(type);
}

ast_type* xi_builder::get_object_type(ast_decl* decl) noexcept {
    return this->_object_types.get_new<xi_decl_type>(decl);
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
    return this->_tuple_types.get_new<xi_tuple_type>(types);
}

ast_type* xi_builder::get_id_type(xi_qname* name) const noexcept {
    return new xi_id_type(name);
}

xi_function_decl* xi_builder::make_xi_function_decl(const char* name, ast_type* return_type, list<xi_parameter_decl>* parameters, ast_stmt* body) const noexcept {
    return new xi_function_decl(name, return_type, parameters, body);
}

xi_parameter_decl* xi_builder::make_xi_parameter_decl(const char* name, ast_type* type) const noexcept {
    return new xi_parameter_decl(name, type);
}

xi_struct_decl* xi_builder::make_xi_struct_decl(const char* name, list<xi_member_decl>* members) const noexcept {
    return new xi_struct_decl(name, nullptr, members);
}

xi_field_decl* xi_builder::make_xi_field_decl(const char* name, ast_type* type) const noexcept {
    return new xi_field_decl(name, nullptr, type);
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

void xi_builder::push_xi_using(xi_using_decl* u) noexcept {
    this->context = this->context->push_context<xi_using_context>(u);
}

void xi_builder::push_xi_function(xi_function_decl* decl) noexcept {
    this->context = this->context->push_context<xi_function_context>(decl);
}

void xi_builder::push_xi_struct(xi_struct_decl* s) noexcept {
    this->context = this->context->push_context<xi_struct_context>(s);
}

void xi_builder::push_xi_method(xi_method_decl* m) noexcept {
    this->context = this->context->push_context<xi_method_context>(m);
}

static inline ptr<ast_context> __push_context(ptr<ast_context> ctx, ast_decl* decl) {
    switch(decl->get_tree_type()) {
    case tree_type_id::ast_namespace_decl:      return ctx->push_context<ast_namespace_context>(decl->as<ast_namespace_decl>());
    case tree_type_id::xi_struct_decl:          return ctx->push_context<xi_struct_context>(decl->as<xi_struct_decl>());
    default:
        __throw_unhandled_tree_type(__FILE__, __LINE__, decl, "push_context");
    }
}

static ptr<ast_decl> __find_declaration(ptr<ast_context> ctx, list<std::string>::iterator iter_start, list<std::string>::iterator iter_end, bool search_parent) {
    auto decl = ctx->find((*iter_start).c_str(), search_parent);
    iter_start++;
    if(iter_start == iter_end) {
        return decl;
    }
    else {
        return __find_declaration(__push_context(ctx, decl), iter_start, iter_end, false);
    }
}

static ptr<list<ast_decl>> __find_all_declarations(ptr<ast_context> ctx, list<std::string>::iterator iter_start, list<std::string>::iterator iter_end, bool search_parent) {
    ptr<list<ast_decl>> ilist = ctx->findall((*iter_start).c_str(), search_parent);
    iter_start++;
    if(iter_start == iter_end) {
        return ilist;
    }
    else {
        ptr<list<ast_decl>> olist = new list<ast_decl>();
        for(auto d: ilist) {
            auto res = __find_all_declarations(__push_context(ctx, d), iter_start, iter_end, false);
            for(auto r: res) {
                olist->append(r);
            }
        }
        return olist;
    }
}


ast_decl* xi_builder::find_declaration(xi_qname* qname) const noexcept {
    return __find_declaration(this->context, begin(qname->names), end(qname->names), true);
}

ptr<list<ast_decl>> xi_builder::find_all_declarations(xi_qname* qname) const noexcept {
    return __find_all_declarations(this->context, begin(qname->names), end(qname->names), true);
}

void maybe_push_context(tree_t* t, xi_builder& b) {
    switch(t->get_tree_type()) {
    case tree_type_id::ast_namespace_decl:      b.push_namespace(t->as<ast_namespace_decl>()); break;
    case tree_type_id::ast_block_stmt:          b.push_block(t->as<ast_block_stmt>()); break;
    case tree_type_id::ast_function_decl:       b.push_function(t->as<ast_function_decl>()); break;
    case tree_type_id::xi_using_decl:           b.push_xi_using(t->as<xi_using_decl>()); break;
    case tree_type_id::xi_function_decl:        b.push_xi_function(t->as<xi_function_decl>()); break;
    case tree_type_id::xi_struct_decl:          b.push_xi_struct(t->as<xi_struct_decl>()); break;
    case tree_type_id::xi_method_decl:          b.push_xi_method(t->as<xi_method_decl>()); break;
    }
}

void maybe_pop_context(tree_t* t, xi_builder& b) {
    switch(t->get_tree_type()) {
    case tree_type_id::ast_namespace_decl:      b.pop_context(); break;
    case tree_type_id::ast_block_stmt:          b.pop_context(); break;
    case tree_type_id::ast_function_decl:       b.pop_context(); break;
    case tree_type_id::xi_using_decl:           b.pop_context(); break;
    case tree_type_id::xi_function_decl:        b.pop_context(); break;
    case tree_type_id::xi_struct_decl:          b.pop_context(); break;
    case tree_type_id::xi_method_decl:          b.pop_context(); break;
    }
}

}
