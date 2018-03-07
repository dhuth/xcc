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

static bool __check_candidate(ast_decl* decl, ptr<list<ast_expr>> args, ast_expr*& callexpr, int& cost, xi_builder& b) {
    auto            ftype       = b.get_declaration_type(decl)->as<ast_function_type>();
    list<ast_type>* ptypes      = ftype->parameter_types;

    if(ftype->is_varargs) {
        if(args->size() < ptypes->size()) {
            return false;
        }
    }
    else {
        if(args->size() != ptypes->size()) {
            return false;
        }
    }

    //TODO: maybe start binding generics
    auto        args_iter       = begin(args);
    auto        args_iter_end   = end(args);
    auto        parm_iter       = begin(*ptypes);
    auto        parm_iter_end   = end(*ptypes);
    auto        oargs           = box(new list<ast_expr>());

    // Do non vararg parameters
    while(parm_iter != parm_iter_end) {
        if(b.widens(*parm_iter, *args_iter, cost)) {
            oargs->push_back(b.cast(*parm_iter, *args_iter));
        }
        else {
            return false;
        }

        parm_iter++;
        args_iter++;
    }

    // Do vararg parameters
    while(args_iter != args_iter_end) {
        oargs->push_back(*args_iter);
        args_iter++;
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



