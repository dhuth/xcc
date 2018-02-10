/*
 * xi_tc_invoke.cpp
 *
 *  Created on: Jan 31, 2018
 *      Author: derick
 */

#include "xi_semantic.hpp"
#include "error.hpp"

#include <algorithm>
#include <tuple>
#include <vector>

namespace xcc {

ast_expr* xi_builder::make_xi_call_expr(ast_decl* decl, list<ast_expr>* arg_list) noexcept {
    auto        ftype           = this->get_declaration_type(decl)->as<ast_function_type>();
    ast_type*   rtype           = ftype->return_type;

    return new ast_call(rtype, this->make_declref_expr(decl), arg_list);
}

static ptr<list<ast_type>> __get_parameter_types(ast_decl* d) {
    ptr<list<xi_parameter_decl>>                parameters;

    switch(d->get_tree_type()) {
    case tree_type_id::xi_function_decl:
    case tree_type_id::xi_operator_function_decl:
        parameters = static_cast<xi_function_base*>(d->as<xi_function_decl>())->parameters;
        break;
    case tree_type_id::xi_method_decl:
    case tree_type_id::xi_operator_method_decl:
        parameters = static_cast<xi_function_base*>(d->as<xi_method_decl>())->parameters;
        break;
    default:
        __throw_unhandled_tree_type(__FILE__, __LINE__, d, "__get_parameter_types()");
    }
    return map(parameters, [&](xi_parameter_decl* p) -> ast_type* {return p->type;});
}

static bool __check_candidate(ast_decl* decl, ptr<list<ast_expr>> args, ast_expr*& callexpr, int& cost, xi_builder& b) {
    //TODO: handle generics
    auto arg_types                  = map(args, [&](ast_expr* e)->ast_type* { return e->type; });
    auto param_types                = __get_parameter_types(decl);

    //TODO: handle vararg functions...
    if(args->size() != param_types->size()) {
        return false;
    }

    auto            arg_expr_iter   = begin(args);
    auto            arg_type_iter   = begin(arg_types);
    auto            param_type_iter = begin(param_types);
    list<ast_expr>* oargs           = new list<ast_expr>();

    //TODO: maybe start binding generics
    while(arg_expr_iter != end(args)) {
        int arg_cost = 0;

        auto param_type_to  = *param_type_iter;
        auto arg_from       = *arg_expr_iter;
        auto arg_type_from  = *arg_type_iter;

        if(b.widens(arg_type_from, param_type_to, arg_cost)) {
            oargs->push_back(b.widen(param_type_to, arg_from));
        }
        else if(b.coercable(param_type_to, arg_from, arg_cost)) {
            oargs->push_back(b.coerce(param_type_to, arg_from));
        }
        else {
            delete oargs;
            return false;
        }

        cost += arg_cost;
        arg_expr_iter++;
        arg_type_iter++;
        param_type_iter++;
    }

    callexpr = b.make_xi_call_expr(decl, oargs);
    return true;
}

static ast_expr* __handle_expr_call(ast_expr* fexpr, ptr<list<ast_expr>> args, xi_builder& b) {
    return b.make_call_expr(fexpr, args);
}

static ast_expr* __handle_static_call(ptr<list<ast_decl>> candidates, ptr<list<ast_expr>> args, xi_builder& b) {
    //TODO: check size of candidate list
    typedef std::tuple<int, ast_expr*>                      good_candidate_t;
    std::vector<good_candidate_t>                           good_candidates;

    for(auto candidate: candidates) {
        ast_expr*       callexpr    = nullptr;
        int             cost        = 0;
        if(__check_candidate(candidate->as<ast_decl>(), args, callexpr, cost, b)) {
            good_candidates.emplace_back(cost, callexpr);
        }
    }

    //TODO: handle no good candidates
    if(good_candidates.size() == 0) {
        return nullptr;
    }

    auto best_candidate = std::min_element(
            good_candidates.begin(),
            good_candidates.end(),
            [&](good_candidate_t l, good_candidate_t r)->bool{
        return std::get<0>(l) < std::get<0>(r);
    });

    return std::get<1>(*best_candidate);
}

static ast_expr* __handle_instance_call(ast_expr* objexpr, ptr<list<xi_member_decl>> candidates, ptr<list<ast_expr>> args, xi_builder& b) {
    //TODO: check size of candidate list

    if(first(candidates)->is<xi_field_decl>()) {
        return __handle_expr_call(b.make_xi_member_expr(objexpr, first(candidates)), args, b);
    }
    else {
        auto mtd_args       = box(new list<ast_expr>(objexpr, args));
        return __handle_static_call(
                map(candidates, [&](xi_member_decl* d) -> ast_decl* {return d->as<ast_decl>();}),
                mtd_args,
                b);
    }
}

static ast_expr* __handle_id_call(xi_id_expr* fexpr, ptr<list<ast_expr>> args, xi_builder& b) {
    auto decls = b.find_all_declarations(fexpr->name);

    // Function Call
    // Static Method Call
    // Static Field Invoke

    return __handle_static_call(decls, args, b);
}

static ast_expr* __handle_id_member_call(xi_member_id_expr* fexpr, ptr<list<ast_expr>> args, xi_builder& b) {
    auto objexpr    = tc_single_expr(fexpr->expr, b);
    auto objtype    = (ast_type*) objexpr->type;

    // Instance Method Call
    // Instance Field Invoke

    return __handle_instance_call(objexpr, b.find_all_members(objtype, fexpr->name), args, b);
}

ast_expr* xi_tc_walker::tc_invoke_expr(xi_invoke_expr* iexpr, xi_builder& b) {
    ast_expr*               fexpr       = iexpr->funcexpr;
    //TODO: handle unpack tuple expressions
    ptr<list<ast_expr>>     args        = map(iexpr->args, [&](ast_expr* e)->ast_expr*{ return tc_single_expr(e, b); });

    switch(fexpr->get_tree_type()) {
    case tree_type_id::xi_id_expr:
        return copyloc(__handle_id_call(fexpr->as<xi_id_expr>(), args, b), iexpr);
    case tree_type_id::xi_member_id_expr:
        return copyloc(__handle_id_member_call(fexpr->as<xi_member_id_expr>(), args, b), iexpr);
    default:
        return copyloc(__handle_expr_call(fexpr, args, b), iexpr);
    }

    return iexpr;
}

}



