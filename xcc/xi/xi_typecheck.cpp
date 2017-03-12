/*
 * xi_typecheck.cpp
 *
 *  Created on: Mar 11, 2017
 *      Author: derick
 */

#include <vector>
#include "xi_pass_typecheck.hpp"
#include "xi_builder.hpp"

namespace xcc {

#define FIND_MEMBER_ARGS_DECL(T)        T*                              tp,                 \
                                        std::string                     name,               \
                                        std::vector<xi_type_decl*>&     searched,           \
                                        ptr<list<xi_member_decl>>       found,              \
                                        bool                            search_instance,    \
                                        bool                            search_static       \

#define FIND_MEMBER_ARGS_PASS(tp, T)    tp->as<T>(),                                        \
                                        name,                                               \
                                        searched,                                           \
                                        found,                                              \
                                        search_instance,                                    \
                                        search_static

static void find_members(FIND_MEMBER_ARGS_DECL(xi_struct_decl));
static void find_members(FIND_MEMBER_ARGS_DECL(xi_class_decl));
static void find_members(FIND_MEMBER_ARGS_DECL(xi_mixin_decl));

static void find_members(FIND_MEMBER_ARGS_DECL(xi_type_decl)) {
    if(std::find(searched.begin(), searched.end(), tp) != searched.end()) {
        return;
    }
    searched.push_back(tp);

    if(tp->is<xi_struct_decl>()) return find_members(FIND_MEMBER_ARGS_PASS(tp, xi_struct_decl));
    if(tp->is<xi_mixin_decl>())  return find_members(FIND_MEMBER_ARGS_PASS(tp, xi_mixin_decl));
    if(tp->is<xi_class_decl>())  return find_members(FIND_MEMBER_ARGS_PASS(tp, xi_class_decl));

    throw std::runtime_error("Unhandeld " + std::string(tp->get_tree_type_name()) + " in find_members\n");
}

static void find_members(FIND_MEMBER_ARGS_DECL(xi_struct_decl)) {
    //TODO: mixins
    for(auto m: tp->members) {
        if(((std::string) m->name) == name) {
            switch(m->get_tree_type()) {
            case tree_type_id::xi_field_decl:
                {
                    auto f = m->as<xi_field_decl>();
                    if((f->is_static  && search_static) ||
                       (!f->is_static && search_instance)) {
                        found->append(f);
                    }
                }
            }
        }
    }
    if(tp->supertype) {
        find_members(FIND_MEMBER_ARGS_PASS(tp->supertype, xi_type_decl));
    }
}

static void find_members(FIND_MEMBER_ARGS_DECL(xi_mixin_decl)) {
}

static void find_members(FIND_MEMBER_ARGS_DECL(xi_class_decl)) {
    find_members(FIND_MEMBER_ARGS_PASS(tp, xi_struct_decl));
}

static ptr<list<xi_member_decl>> find_members(xi_type_decl* tp, std::string name, bool search_instance, bool search_static) {
    std::vector<xi_type_decl*>      searched;
    ptr<list<xi_member_decl>>       found       = new list<xi_member_decl>();

    find_members(tp, name, searched, found, search_instance, search_static);
    return found;
}

static ptr<list<xi_member_decl>> find_instance_members(xi_type_decl* tp, std::string name) {
    return find_members(tp, name, true, false);
}

void xi_bottom_up_typecheck_pass::postvisit(tree_type_id id, ast_tree* t) {
    if(t->is<ast_expr>()) {
        ast_type* tp = t->as<ast_expr>()->type;
        if(tp == nullptr) {
            throw std::runtime_error("Type not set for " + std::string(t->get_tree_type_name()) + "\n");
        }
    }
}


ast_expr* xi_bottom_up_typecheck_pass::check_named_memberref_expr(xi_named_memberref_expr* expr) {
    ast_type* otype = expr->objexpr->type;

    switch(otype->get_tree_type()) {
    case tree_type_id::xi_object_type:
        {
            xi_type_decl* decl = otype->as<xi_object_type>()->declaration;
            //TODO: consider visibility
            //TODO: consider method overloading
            ptr<list<xi_member_decl>> member_list = find_instance_members(decl, expr->member_name);
            //...
            if(member_list->size() == 0) {
                //TODO: a common error
                assert(false);
            }
            else if(member_list->size() == 1) {
                xi_member_decl* m = (*member_list)[0];
                switch(m->get_tree_type()) {
                case tree_type_id::xi_field_decl:
                    auto f = m->as<xi_field_decl>();
                    return builder.make_fieldref_expr(expr->objexpr, f);
                    break;
                }
                throw std::runtime_error("Unhandled " + std::string(m->get_tree_type_name()) + " in " + std::string(__FILE__) + ":" + std::to_string(__LINE__));
            }
            else {
                //TODO: return a member group type
                assert(false);
            }
        }
        break;
    case tree_type_id::xi_ref_type:
        break;
    }

    throw std::runtime_error("Uhandled object type " + std::string(otype->get_tree_type_name()) + " in xi_bottom_up_typecheck_pass::check_xi_named_memberref_expr\n");
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
    case xi_operator::sub:      return builder.make_op_expr(ast_op::neg, expr);
    case xi_operator::lnot:     return builder.make_op_expr(ast_op::logical_not, expr);
    case xi_operator::bnot:     return builder.make_op_expr(ast_op::binary_not, expr);
    }
    throw std::runtime_error("what?\n");
}

ast_expr* xi_bottom_up_typecheck_pass::check_op_expr(xi_op_expr* expr) {
    //TODO: actually look for an overload
    //...

    if(expr->operands->size() == 1) {
        return get_unary_op(this->builder, expr->op, (ast_expr*) expr->operands[0]);
    }
    else if(expr->operands->size() == 2) {
        return get_binary_op(this->builder, expr->op, (ast_expr*) expr->operands[0], (ast_expr*) expr->operands[1]);
    }
    throw std::runtime_error("what?\n");
}

ast_expr* xi_bottom_up_typecheck_pass::check_index_expr(xi_index_expr* expr) {
    //TODO: check for overloads

    if(!expr->type->is<xi_array_type>()) {
        //TODO: user error
        assert(false);
    }

    auto atype = expr->type->as<xi_array_type>();
    if(expr->index_expr_list->size() != atype->dimensions->size()) {
        //TODO: user error
        assert(false);
    }

}

}
