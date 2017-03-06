/*
 * ast_context.hpp
 *
 *  Created on: Mar 6, 2017
 *      Author: derick
 */

#ifndef AST_CONTEXT_HPP_
#define AST_CONTEXT_HPP_


#include "ast.hpp"

namespace xcc {

struct ast_context {
public:

    inline ast_context()                  : parent(nullptr) { };
    inline ast_context(ast_context* prev) : parent(prev)    { };
    virtual ~ast_context() = default;

    virtual void      insert(const char*, ast_decl*) = 0;
    virtual ast_type* get_return_type() = 0;

    inline ptr<ast_decl> find(const char* name, bool search_parent = true) {
        auto ff = this->find_first_impl(name);
        if(unbox(ff) != nullptr) {
            return ff;
        }
        else if(search_parent && (this->parent != nullptr)) {
            return this->parent->find(name, true);
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
        if(search_parent && (this->parent != nullptr)) {
            this->parent->findall(olist, name, search_parent);
        }
    }

    virtual ptr<ast_decl> find_first_impl(const char* name) = 0;
    virtual void find_all_impl(ptr<list<ast_decl>>, const char*) = 0;

    friend struct __ast_builder_impl;

    ast_context*                                                         parent;

};

struct ast_namespace_context : public ast_context {
public:

    ast_namespace_context();
    ast_namespace_context(ast_context* p, ast_namespace_decl* ns);
    virtual ~ast_namespace_context() = default;

    void insert(const char*, ast_decl*) final override;
    ast_type* get_return_type() final override;

protected:

    ptr<ast_decl> find_first_impl(const char*) final override;
    void find_all_impl(ptr<list<ast_decl>>, const char*) final override;

private:

    ptr<ast_namespace_decl>                                             _ns;
    bool                                                                _is_global;

};

struct ast_block_context : public ast_context {
public:

    ast_block_context(ast_context* p, ast_block_stmt* block);
    virtual ~ast_block_context() = default;

    void insert(const char*, ast_decl*) final override;
    ast_type* get_return_type() final override;
    void emit(ast_stmt*);

protected:

    ptr<ast_decl> find_first_impl(const char*) final override;
    void find_all_impl(ptr<list<ast_decl>>, const char*) final override;

private:

    ptr<ast_block_stmt>                                                 _block;

};

}


#endif /* AST_CONTEXT_HPP_ */
