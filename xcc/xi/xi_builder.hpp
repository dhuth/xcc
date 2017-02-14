/*
 * xi_builder.hpp
 *
 *  Created on: Feb 1, 2017
 *      Author: derick
 */

#ifndef XI_XI_BUILDER_HPP_
#define XI_XI_BUILDER_HPP_

#include <functional>
#include <unordered_map>
#include "frontend.hpp"
#include "xi_tree.hpp"
#include "ast_builder.hpp"

namespace xcc {

struct xi_context {
public:

    inline xi_context()                 : _parent(nullptr) { };
    inline xi_context(xi_context* prev) : _parent(prev)    { };
    virtual ~xi_context() = default;

    virtual void insert(const char*, ast_decl*) = 0;

    inline ptr<ast_decl> find(const char* name, bool search_parent = true) {
        auto ff = this->find_first_impl(name);
        if(unbox(ff) != nullptr) {
            return ff;
        }
        else if(search_parent && (this->_parent != nullptr)) {
            return this->_parent->find(name, true);
        }
        return box<ast_decl>(nullptr);
    }

    inline ptr<list<ast_decl>> findall(const char* name, bool search_parent = true) {
        ptr<list<ast_decl>> olist = box(new list<ast_decl>());
        this->findall(olist, name, search_parent);
        return olist;
    }

protected:

    inline void findall(ptr<list<ast_decl>> olist, const char* name, bool search_parent) {
        this->find_all_impl(olist, name);
        if(search_parent && (this->_parent != nullptr)) {
            this->_parent->findall(olist, name, search_parent);
        }
    }

    virtual ptr<ast_decl> find_first_impl(const char* name) = 0;
    virtual void find_all_impl(ptr<list<ast_decl>>, const char*) = 0;

private:

    friend struct xi_builder;

    xi_context*                                                         _parent;

};

struct xi_namespace_context : public xi_context {
public:

    xi_namespace_context();
    xi_namespace_context(xi_context* p, ast_namespace_decl* ns);
    virtual ~xi_namespace_context() = default;

    void insert(const char*, ast_decl*) final override;

protected:

    ptr<ast_decl> find_first_impl(const char*) final override;
    void find_all_impl(ptr<list<ast_decl>>, const char*) final override;

private:

    ptr<ast_namespace_decl>                                             _ns;
    bool                                                                _is_global;

};

struct xi_function_context : public xi_context {
public:

    xi_function_context(xi_context* parent, xi_function_decl* func);
    virtual ~xi_function_context() = default;

    void insert(const char*, ast_decl*) final override; //TODO: just throw error

protected:

    ptr<ast_decl> find_first_impl(const char*) final override;
    void find_all_impl(ptr<list<ast_decl>>, const char*) final override;

private:

    ptr<xi_function_decl>                                               _func;

};


struct xi_builder final : public ast_builder<> {
public:

    xi_builder(translation_unit&);
    virtual ~xi_builder() = default;

    xi_const_type*                                  get_const_type(ast_type*) const noexcept;
    void                                            define_typedef(const char*, ast_type*);
    ast_namespace_decl*                             define_namespace(const char*);
    ast_variable_decl*                              define_global_variable(ast_type*, const char*);
    ast_variable_decl*                              define_global_variable(ast_type*, const char*, ast_expr*);
    xi_function_decl*                               define_global_function(ast_type*, const char*, list<xi_parameter_decl>*);
    xi_parameter_decl*                              define_parameter(ast_type*, const char*);
    xi_parameter_decl*                              define_parameter(ast_type*);

    ast_local_decl*                                 define_local_variable(ast_type*, const char*);
    ast_local_decl*                                 define_local_variable(ast_type*, const char*, ast_expr*);

    ast_expr*                                       make_op(xi_operator op, ast_expr*);
    ast_expr*                                       make_op(xi_operator op, ast_expr*, ast_expr*);

    void                                            push_function(xi_function_decl*);
    void                                            push_namespace(ast_namespace_decl* name);
    void                                            push_block(ast_block_stmt*);
    void                                            pop();

    void                                            generate();

protected:

    ast_expr*                                       flatten(ast_expr*);
    ast_type*                                       flatten(ast_type*);
    ast_decl*                                       flatten(ast_decl*);
    ast_function_decl*                              flatten_function(xi_function_decl*);
    void                                            flatten_body(xi_function_decl*);

private:

    translation_unit&                                                   tu;
    ptr<list<xi_function_decl>>                                         _all_functions;
    ptr<xi_context>                                                     _context;

};

}



#endif /* XI_XI_BUILDER_HPP_ */
