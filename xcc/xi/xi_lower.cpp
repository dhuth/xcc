/*
 * xi_lower.cpp
 *
 *  Created on: Nov 28, 2017
 *      Author: derick
 */

#include "xi_lower.hpp"
#include "xi_walker.hpp"
#include "frontend.hpp"

namespace xcc {

/* ============= *
 * Lower Methods *
 * ============= */

static inline void copy_declaration_info(ast_decl* dest, ast_decl* src) {
    dest->source_location       = src->source_location;
    dest->generated_name        = src->generated_name;
}

static inline void copy_externable_info(ast_externable* dest, ast_externable* src) {
    dest->is_extern             = src->is_extern;
    dest->is_extern_visible     = src->is_extern_visible;
}

static xi_function_decl* __lower_method_cont(xi_method_decl* mtd, xi_function_decl* func) noexcept {
    copy_declaration_info(func, mtd);
    copy_externable_info(func, mtd);

    //TODO: parameters and return type

    func->is_forward_decl       = mtd->is_forward_decl;
    func->is_c_extern           = false;

    mtd->lowered_func = func;
    return func;
}

static inline xi_function_decl* lower_operator_method(xi_operator_method_decl* mtd) noexcept {
    auto func = new xi_operator_function_decl((std::string) mtd->name, (xi_operator) mtd->op, mtd->return_type, mtd->parameters, mtd->is_vararg, mtd->body);
    return __lower_method_cont(mtd, func);
}

static inline xi_function_decl* lower_method(xi_method_decl* mtd) noexcept {
    auto func = new xi_function_decl((std::string) mtd->name, mtd->return_type, mtd->parameters, mtd->is_vararg, mtd->body);
    return __lower_method_cont(mtd, func);
}

static void lower_methods(ptr<list<xi_function_decl>> flist, xi_builder& b) {

    // Lower methods
    // -------------

    auto mtd_functions = map(
            gather_nodes<xi_method_decl>(b.get_global_namespace()),
            [&](xi_method_decl* d)->xi_function_decl* {
        if(d->is<xi_operator_method_decl>()) {
            return lower_operator_method(d->as<xi_operator_method_decl>());
        }
        return lower_method(d);
    });

    // Update references
    // -----------------

    auto mtdrefs = filter(
            gather_nodes<ast_declref>(b.get_global_namespace()),
            [&](ast_declref* r)->bool {
        return r->declaration->is<xi_method_decl>();
    });
    for(auto mtdref : mtdrefs) {
        mtdref->declaration = mtdref->declaration->as<xi_method_decl>()->lowered_func;
    }

    // Add Lowered Functions to List
    // -----------------------------

    for(auto f: mtd_functions) {
        flist->push_back(f);
    }
}

/* =============== *
 * Lower Functions *
 * =============== */

static ast_parameter_decl* lower_parameter(xi_parameter_decl* d, xi_builder& b) {
    return d;
}

static ast_function_decl* lower_function(xi_function_decl* fdecl, xi_builder& b) {
    std::string name            = fdecl->name;
    ast_type*   return_type     = fdecl->return_type;
    auto        parameters      =
            map(fdecl->parameters, [&](xi_parameter_decl* d)->ast_parameter_decl* {
                return lower_parameter(d, b);
            });
    ast_stmt*   body            = fdecl->body;

    ast_function_decl*  nfunc   = new ast_function_decl(name, return_type, parameters, body);
    copy_declaration_info(nfunc, fdecl);
    copy_externable_info(nfunc, fdecl);

    nfunc->is_c_extern          = fdecl->is_c_extern;
    nfunc->is_varargs           = fdecl->is_vararg;
    fdecl->lowered_func         = nfunc;
    return nfunc;
}

static void lower_function_list(ptr<list<xi_function_decl>> flist, translation_unit& tu, xi_builder& b) {
    // Lower Functions
    // ---------------
    for(auto f: flist) {
        tu.append(lower_function(f, b));
    }

    // Update Function Refs
    // --------------------
    auto frefs = filter(
            gather_nodes<ast_declref>(b.get_global_namespace()),
            [&](ast_declref* dref)->bool {
        return dref->declaration->is<xi_function_decl>();
    });
    for(auto dref: frefs) {
        dref->declaration = dref->declaration->as<xi_function_decl>()->lowered_func;
    }

    // Update Parameter Refs
    // ---------------------
    auto prefs = filter(
            gather_nodes<ast_declref>(b.get_global_namespace()),
            [&](ast_declref* dref)->bool {
        return dref->declaration->is<xi_parameter_decl>();
    });
    for(auto pref: prefs) {
        pref->declaration = pref->declaration->as<ast_parameter_decl>();
    }
}


bool xi_builder::lower_pass(ircode_context&) noexcept {
    // Get All Non Generic Function Declarations
    // -----------------------------------------

    ptr<list<xi_function_decl>> flist = filter(
            gather_nodes<xi_function_decl>(this->global_namespace),
            [&](xi_function_decl* fdecl) -> bool {
        //TODO: pick non-generic functions only
        return true;
    });


    // Lower generics
    // --------------
    // TODO: ...


    // Promote Closures
    // ----------------
    // TODO: ...


    // Lower methods
    // -------------
    lower_methods(flist, *this);

    // Lower functions
    // ---------------
    lower_function_list(flist, this->tu, *this);

    // Lower Types & Expressions
    // -------------------------
    // TODO: ...

    return true;
}

}
