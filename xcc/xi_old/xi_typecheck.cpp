/*
 * xi_typecheck.cpp
 *
 *  Created on: Mar 11, 2017
 *      Author: derick
 */

#include <xi_old/xi_builder.hpp>
#include <xi_old/xi_pass_typecheck.hpp>
#include <vector>
#include <limits>

namespace xcc {

void xi_bottom_up_typecheck_pass::postvisit(tree_type_id id, ast_tree* t) {
    if(t->is<ast_expr>()) {
        assert(t->as<ast_expr>()->type != nullptr);
    }
}

static ast_type* remove_const(xi_builder& builder, ast_type* t) {
    if(t->is<xi_const_type>()) {
        ast_type* nt = t->as<xi_const_type>()->type;
        return remove_const(builder, nt);
    }
    if(t->is<xi_ref_type>()) {
        ast_type* nt = t->as<xi_ref_type>()->element_type;
        ast_type* newt = remove_const(builder, nt);
        return builder.get_ref_type(newt);
    }
    return t;
}

static ast_expr* remove_const(xi_builder& builder, ast_type* t, ast_expr* e) {
    if(t->is<xi_const_type>()) {
        ast_type* nt = t->as<xi_const_type>()->type;
        return builder.make_cast_expr(nt, remove_const(builder, nt, e));
    }
    return e;
}

static ast_expr* remove_ref(xi_builder& builder, ast_type* t, ast_expr* e) {
    if(t->is<xi_ref_type>()) {
        ast_type* nt = t->as<xi_ref_type>()->element_type;
        return builder.make_deref_expr(e);
    }
    if(t->is<xi_const_type>()) {
        ast_type* nt = t->as<xi_const_type>()->type;
        auto newexpr = remove_ref(builder, nt, e);
        if(!newexpr->type->is<xi_const_type>()) {
            newexpr->type = builder.get_const_type(newexpr->type);
        }
        return newexpr;
    }
    return e;
}

static ast_expr* remove_ref(xi_builder& builder, ast_expr* e) {
    return remove_ref(builder, e->type, e);
}

static ast_expr* remove_const_and_ref(xi_builder& builder, ast_type* t, ast_expr* e) {
    if(t->is<xi_const_type>()) {
        ast_type* nt = t->as<xi_const_type>()->type;
        return builder.make_cast_expr(nt, remove_const_and_ref(builder, nt, e));
    }
    if(t->is<xi_ref_type>()) {
        ast_type* nt = t->as<xi_ref_type>()->element_type;
        return builder.make_deref_expr(remove_const_and_ref(builder, nt, e));
    }
    return e;
}

static ast_expr* to_lower_rhs(xi_builder& builder, ast_expr* e) {
    return remove_const_and_ref(builder, e->type, e);
}

ast_expr* xi_bottom_up_typecheck_pass::check_named_memberref_expr(xi_named_memberref_expr* expr) {
    ast_expr* objexpr = remove_ref(this->builder, expr->objexpr);
    xi_type_decl* decl = remove_const(this->builder, objexpr->type)->as<xi_object_type>()->declaration;

    auto memref = this->builder.make_instance_memberref_expr(decl, objexpr, expr->member_name->c_str(), (source_span&) expr->source_location);

    //TODO: ...

    if(objexpr->type->is<xi_const_type>()) {
        return this->builder.make_cast_expr(this->builder.get_const_type(memref->type), memref);
    }
    return memref;
}

ast_expr* xi_bottom_up_typecheck_pass::check_static_named_memberref_expr(xi_static_named_memberref_expr* expr) {
    assert(expr->objtype->is<xi_object_type>());

    xi_type_decl* decl = expr->objtype->as<xi_object_type>()->declaration;

    return this->builder.make_static_memberref_expr(decl, expr->member_name->c_str(), (source_span&) expr->source_location);
}

static ast_expr* get_binary_op(xi_builder& builder, xi_operator op, ast_expr* lhs, ast_expr* rhs) {
    switch(op) {
    case xi_operator::add:      return builder.make_lower_op_expr(ast_op::add, lhs, rhs);
    case xi_operator::sub:      return builder.make_lower_op_expr(ast_op::sub, lhs, rhs);
    case xi_operator::mul:      return builder.make_lower_op_expr(ast_op::mul, lhs, rhs);
    case xi_operator::div:      return builder.make_lower_op_expr(ast_op::div, lhs, rhs);
    case xi_operator::mod:      return builder.make_lower_op_expr(ast_op::mod, lhs, rhs);
    case xi_operator::land:     return builder.make_lower_op_expr(ast_op::logical_and, lhs, rhs);
    case xi_operator::lor:      return builder.make_lower_op_expr(ast_op::logical_or, lhs, rhs);
    case xi_operator::band:     return builder.make_lower_op_expr(ast_op::binary_and, lhs, rhs);
    case xi_operator::bor:      return builder.make_lower_op_expr(ast_op::binary_or, lhs, rhs);
    case xi_operator::bxor:     return builder.make_lower_op_expr(ast_op::binary_xor, lhs, rhs);
    case xi_operator::eq:       return builder.make_lower_op_expr(ast_op::eq, lhs, rhs);
    case xi_operator::ne:       return builder.make_lower_op_expr(ast_op::ne, lhs, rhs);
    case xi_operator::lt:       return builder.make_lower_op_expr(ast_op::lt, lhs, rhs);
    case xi_operator::le:       return builder.make_lower_op_expr(ast_op::le, lhs, rhs);
    case xi_operator::gt:       return builder.make_lower_op_expr(ast_op::gt, lhs, rhs);
    case xi_operator::ge:       return builder.make_lower_op_expr(ast_op::ge, lhs, rhs);
    }
    throw std::runtime_error("what?\n");
}

static ast_expr* get_unary_op(xi_builder& builder, xi_operator op, ast_expr* expr) {
    switch(op) {
    case xi_operator::sub:      return builder.make_op_expr(ast_op::neg,         expr);
    case xi_operator::lnot:     return builder.make_op_expr(ast_op::logical_not, expr);
    case xi_operator::bnot:     return builder.make_op_expr(ast_op::binary_not,  expr);
    }
    throw std::runtime_error("what?\n");
}

ast_expr* xi_bottom_up_typecheck_pass::check_op_expr(xi_op_expr* expr) {
    //TODO: actually look for an overload
    //...

    if(expr->operands->size() == 1) {
        //auto opr = this->builder.lower((ast_expr*) expr->operands[0]);
        auto opr = to_lower_rhs(this->builder, (ast_expr*) expr->operands[0]);
        return get_unary_op(this->builder, expr->op, opr);
    }
    else if(expr->operands->size() == 2) {
        //auto lhs = this->builder.lower((ast_expr*) expr->operands[0]);
        //auto rhs = this->builder.lower((ast_expr*) expr->operands[1]);
        auto lhs = to_lower_rhs(this->builder, (ast_expr*) expr->operands[0]);
        auto rhs = to_lower_rhs(this->builder, (ast_expr*) expr->operands[1]);
        return get_binary_op(this->builder, expr->op, lhs, rhs);
    }
    throw std::runtime_error("what?\n");
}

static xi_array_type* get_array_type(ast_type* tp, bool& is_const) {
    if(tp->is<xi_array_type>()) {
        return tp->as<xi_array_type>();
    }
    else if(tp->is<xi_const_type>()) {
        is_const = true;
        return get_array_type(tp->as<xi_const_type>()->type, is_const);
    }
    else {
        //TODO: not an indexable type
        throw std::runtime_error("Not implemented yet\n");
    }
}

ast_expr* xi_bottom_up_typecheck_pass::check_index_expr(xi_index_expr* expr) {
    auto aexpr = expr->array_expr->as<ast_expr>();
    //TODO: check for overloads

    bool is_const = false;
    auto atype = get_array_type(aexpr->type, is_const);


    if(expr->index_expr_list->size() != atype->dimensions->size()) {
        //TODO: user error
        assert(false);
    }

    if(expr->index_expr_list->size() == atype->dimensions->size()) {
        if(is_const) {
            expr->type = this->builder.get_const_type(atype->element_type);
        }
        else {
            expr->type = atype->element_type;
        }
    }
    else {
        throw std::runtime_error("Not implemented yet\n");
    }

    return expr;
}

static ast_expr* check_invoke_overload(xi_builder& builder, ast_expr* funcexpr, list<ast_expr>* arguments, uint32_t& effort, bool& is_valid) {
    ast_type*               funcexpr_type     = funcexpr->type;
    ast_function_type*      functype          = nullptr;

    if(funcexpr_type->is<ast_function_type>()) {
        functype      = funcexpr->type->as<ast_function_type>();
        funcexpr      = builder.make_addressof_expr(funcexpr);
        funcexpr_type = builder.get_pointer_type(funcexpr_type);
    }
    else {
        if(!(funcexpr_type->is<ast_pointer_type>() &&
             funcexpr_type->as<ast_pointer_type>()->element_type->is<ast_function_type>())) {
            throw std::runtime_error("This should be a user error");
        }
        functype      = funcexpr->type->as<ast_pointer_type>()->element_type->as<ast_function_type>();
        //funcexpr      = builder.make_deref_expr(funcexpr);
    }

    if(arguments->size() != functype->parameter_types->size()) {
        is_valid = false;
        return nullptr;
    }

    ptr<list<ast_expr>> new_args = new list<ast_expr>();
    for(uint32_t i = 0; i < arguments->size(); i++) {
        ast_expr* aexpr = (*arguments)[i];
        ast_type* atype = (*arguments)[i]->type;
        ast_type* ptype = functype->parameter_types[i];

        if(builder.sametype(atype, ptype)) {
            new_args->append(aexpr);
        }
        else if(builder.widens(atype, ptype)) {
            uint32_t weffort = 0;
            new_args->append(builder.widen(ptype, aexpr, weffort));
            effort += weffort;
        }
        else {
            is_valid = false;
            return nullptr;
        }
    }

    return new ast_invoke(functype->return_type, funcexpr, new_args);
}

ast_expr* xi_bottom_up_typecheck_pass::check_invoke_expr(ast_invoke* expr) {

    //TODO: ...
    ast_expr* new_expr = nullptr;

    uint32_t    effort      = 0;
    bool        is_valid    = true;

    if(expr->funcexpr->is<xi_group_expr>()) {
        uint32_t min_effort = std::numeric_limits<uint32_t>::max();
        for(auto fexpr: expr->funcexpr->as<xi_group_expr>()->expressions) {
            is_valid = true;
            effort   = 0;
            if(fexpr->type->is<ast_function_type>() ||
                    (fexpr->type->is<ast_pointer_type>() &&
                     fexpr->type->as<ast_pointer_type>()->element_type->is<ast_function_type>())) {
                auto next_expr = check_invoke_overload(this->builder, fexpr, expr->arguments, effort, is_valid);
                if(is_valid && effort < min_effort) {
                    min_effort = effort;
                    new_expr   = next_expr;
                }
            }
        }
    }
    else {
        new_expr = check_invoke_overload(this->builder, expr->funcexpr, expr->arguments, effort, is_valid);
    }
    this->builder.copyloc(new_expr, expr);
    return new_expr;
}

ast_expr* xi_bottom_up_typecheck_pass::check_call_expr(ast_call* expr) {
    assert(false);
    //TODO: ...
    return expr;
}

}
