/*
 * xi_name_mangler.cpp
 *
 *  Created on: Feb 6, 2018
 *      Author: derick
 */


#include "xi_name_mangler.hpp"
#include "xi_builder.hpp"
#include "xi_walker.hpp"

namespace xcc {

std::string xi_name_mangler::mangle_xi_namespace_decl(xi_namespace_decl* ns) {
    return ((std::string) ns->name) + "$_";
}

std::string xi_name_mangler::mangle_xi_function_decl(xi_function_decl* fdecl) {
    std::stringstream s;
    s << (std::string) fdecl->name << "F";
    s << this->visit(fdecl->return_type);
    for(auto param : fdecl->parameters) {
        s << (std::string) this->visit(param->type);
    }
    s << "E";
    return s.str();
}

std::string xi_name_mangler::mangle_xi_operator_function_decl(xi_operator_function_decl* fdecl) {
    return this->mangle_xi_function_decl(fdecl);
}

std::string xi_name_mangler::mangle_xi_method_decl(xi_method_decl* mdecl) {
    std::stringstream s;
    s << (std::string) mdecl->name << "F";
    s << this->visit(mdecl->return_type);
    for(auto param : mdecl->parameters) {
        s << (std::string) this->visit(param->type);
    }
    s << "E";
    return s.str();
}

std::string xi_name_mangler::mangle_xi_operator_method_decl(xi_operator_method_decl* mdecl) {
    return this->mangle_xi_method_decl(mdecl);
}

struct mangle_names_walker : public xi_preorder_walker<> {
public:

    explicit inline mangle_names_walker() noexcept
        : xi_preorder_walker<>() {

        this->add(&mangle_names_walker::visit_xi_namespace);
        this->add(&mangle_names_walker::visit_xi_function);
        this->add(&mangle_names_walker::visit_xi_method);
    }

    void begin(tree_type_id, ast_tree* t, xi_builder&) override;
    void end(tree_type_id, ast_tree*, xi_builder&) override;

private:

    inline void visit_decl(ast_decl* d, xi_builder& b) {
        d->generated_name = b.get_mangled_name(this->get_prefix(), d);
    }

    void visit_xi_namespace(xi_namespace_decl* n,   xi_builder& b)  { this->visit_decl(n, b);}
    void visit_xi_method(xi_method_decl* m,         xi_builder& b)  { this->visit_decl(m, b);}

    void visit_xi_function(xi_function_decl* f,     xi_builder& b) {
        if(!f->is_c_extern) {
            this->visit_decl(f, b);
        }
        else {
            f->generated_name = f->name;
        }
    }

    inline std::string get_prefix() noexcept {
        if(_prefix.size() > 0) {
            return _prefix.top();
        }
        return "";
    }

    inline void push_prefix(std::string n) {
        _prefix.push(n);
    }

    inline void pop_prefix() {
        _prefix.pop();
    }

    std::stack<std::string>                                 _prefix;
};

void mangle_names_walker::begin(tree_type_id id, ast_tree* t, xi_builder& b) {
    switch(id) {
    case tree_type_id::xi_namespace_decl:
    case tree_type_id::ast_namespace_decl:
    case tree_type_id::xi_struct_decl:
        this->push_prefix((std::string) t->as<ast_decl>()->generated_name);
        break;
    }
    xi_preorder_walker<>::begin(id, t, b);
}

void mangle_names_walker::end(tree_type_id id, ast_tree* t, xi_builder& b) {
    switch(id) {
    case tree_type_id::xi_namespace_decl:
    case tree_type_id::ast_namespace_decl:
    case tree_type_id::xi_struct_decl:
        this->pop_prefix();
        break;
    }
    xi_preorder_walker<>::end(id, t, b);
}

bool xi_builder::mangle_names_pass() noexcept {
    mangle_names_walker mw;
    for(auto d: this->global_namespace->declarations) {
        mw.visit(d, *this);
    }
    return true;
}
}
