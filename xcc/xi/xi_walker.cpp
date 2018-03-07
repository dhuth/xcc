

#include "xi_builder.hpp"
#include "xi_context.hpp"
#include "xi_walker.hpp"
#include "error.hpp"

namespace xcc {

void maybe_push_context(ast_tree* t, xi_builder& b) {
    switch(t->get_tree_type()) {
    case tree_type_id::ast_namespace_decl:              b.push_namespace(t->as<ast_namespace_decl>());      break;
    case tree_type_id::xi_namespace_decl:               b.push_xi_namespace(t->as<xi_namespace_decl>());    break;
    case tree_type_id::ast_block_stmt:                  b.push_block(t->as<ast_block_stmt>());              break;
    case tree_type_id::ast_function_decl:               b.push_function(t->as<ast_function_decl>());        break;
    case tree_type_id::xi_function_decl:                b.push_xi_function(t->as<xi_function_decl>());      break;
    case tree_type_id::xi_operator_function_decl:       b.push_xi_function(t->as<xi_function_decl>());      break;
    case tree_type_id::xi_struct_decl:                  b.push_xi_struct(t->as<xi_struct_decl>());          break;
    case tree_type_id::xi_method_decl:                  b.push_xi_method(t->as<xi_method_decl>());          break;
    case tree_type_id::xi_operator_method_decl:         b.push_xi_method(t->as<xi_method_decl>());          break;
    }
}

void maybe_pop_context(ast_tree* t, xi_builder& b) {
    switch(t->get_tree_type()) {
    case tree_type_id::ast_namespace_decl:              b.pop_context();                                    break;
    case tree_type_id::xi_namespace_decl:               b.leave_xi_namespace();                             break;
    case tree_type_id::ast_block_stmt:                  b.pop_context();                                    break;
    case tree_type_id::ast_function_decl:               b.pop_context();                                    break;
    case tree_type_id::xi_function_decl:                b.leave_xi_function();                              break;
    case tree_type_id::xi_operator_function_decl:       b.leave_xi_function();                              break;
    case tree_type_id::xi_struct_decl:                  b.leave_xi_struct();                                break;
    case tree_type_id::xi_method_decl:                  b.leave_xi_method();                                break;
    case tree_type_id::xi_operator_method_decl:         b.leave_xi_method();                                break;
    }
}

ast_type* xi_builder::get_return_type() const noexcept {
    auto decl = unbox(_nesting_closure_decls.top());
    switch(decl->get_tree_type()) {
    case tree_type_id::xi_function_decl:
    case tree_type_id::xi_operator_function_decl:
        return static_cast<xi_function_decl*>(decl)->return_type;
    case tree_type_id::xi_method_decl:
    case tree_type_id::xi_operator_method_decl:
        return static_cast<xi_method_decl*>(decl)->return_type;
    default:
        __throw_unhandled_tree_type(__FILE__, __LINE__, decl, "xi_builder::get_return_type()");
    }
}

ast_namespace_decl* xi_builder::get_nesting_namespace() const noexcept {
    if(_nesting_namespaces.size() > 0)      return unbox(_nesting_namespaces.top());
    else                                    return unbox(this->global_namespace);
}

ast_type* xi_builder::get_nesting_type() const noexcept {
    if(_nesting_types.size() > 0)           return unbox(_nesting_types.top());
    else                                    return nullptr;
}

void xi_builder::push_xi_namespace(xi_namespace_decl* decl) noexcept {
    this->context = this->context->push_context<ast_namespace_context>(decl);
    _nesting_namespaces.push(decl);
}

void xi_builder::leave_xi_namespace() noexcept {
    this->pop_context();
    _nesting_namespaces.pop();
}

void xi_builder::push_xi_function(xi_function_decl* decl) noexcept {
    this->context = this->context->push_context<xi_function_context>(decl);
    _nesting_closure_decls.push(decl);
}

void xi_builder::leave_xi_function() noexcept {
    this->pop_context();
    _nesting_closure_decls.pop();
}

void xi_builder::push_xi_struct(xi_struct_decl* s) noexcept {
    this->context = this->context->push_context<xi_struct_context>(s);
    _nesting_types.push(this->get_object_type(s));
}

void xi_builder::leave_xi_struct() noexcept {
    this->pop_context();
    _nesting_types.pop();
}

void xi_builder::push_xi_method(xi_method_decl* m) noexcept {
    this->context = this->context->push_context<xi_method_context>(m);
    _nesting_closure_decls.push(m);
}

void xi_builder::leave_xi_method() noexcept {
    this->pop_context();
    _nesting_closure_decls.pop();
}

}




