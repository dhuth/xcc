/*
 * xi_context.hpp
 *
 *  Created on: Jan 23, 2018
 *      Author: derick
 */

#ifndef XCC_XI_XI_CONTEXT_HPP_
#define XCC_XI_XI_CONTEXT_HPP_

#include "xi_tree.hpp"
#include "ast_context.hpp"

namespace xcc {


struct xi_context : public ast_context {
public:

    explicit inline xi_context(ast_context* prev) noexcept
            : ast_context(prev) {
        // do nothing
    }
    virtual ~xi_context() = default;

};


struct xi_function_context final : public xi_context {
public:

    explicit inline xi_function_context(ast_context* prev, xi_function_decl* decl) noexcept
            : xi_context(prev),
              func(decl) {
        // do nothing
    }
    virtual ~xi_function_context() = default;

protected:

    virtual ptr<ast_decl> find_first_impl(const char*) override final;
    virtual void          find_all_impl(ptr<list<ast_decl>>, const char*) override final;

private:

    ptr<xi_function_decl>                                   func;

};


struct xi_method_context final : public xi_context {
public:

    explicit inline xi_method_context(ast_context* prev, xi_method_decl* methoddecl) noexcept
            : xi_context(prev),
              methoddecl(methoddecl) {
        // do nothing
    }
    virtual ~xi_method_context() = default;

protected:

    virtual ptr<ast_decl> find_first_impl(const char*) override final;
    virtual void          find_all_impl(ptr<list<ast_decl>>, const char*) override final;

private:

    ptr<xi_method_decl>                                     methoddecl;

};


struct xi_type_context : public xi_context {
public:

    inline xi_type_context(ast_context* prev, xi_type_decl* typedecl) noexcept
            : xi_context(prev),
              typedecl(typedecl) {
        // do nothing
    }
    virtual ~xi_type_context() = default;

protected:

    virtual ptr<ast_decl> find_first_impl(const char*) override;
    virtual void          find_all_impl(ptr<list<ast_decl>>, const char*) override;

private:

    ptr<xi_type_decl>                                       typedecl;

};


struct xi_struct_context : public xi_type_context {
public:

    inline xi_struct_context(ast_context* prev, xi_struct_decl* structdecl) noexcept
            : xi_type_context(prev, structdecl),
              structdecl(structdecl) {
        // do nothing
    }
    virtual ~xi_struct_context() = default;

protected:

    virtual ptr<ast_decl> find_first_impl(const char*) override;
    virtual void          find_all_impl(ptr<list<ast_decl>>, const char*) override;

private:

    ptr<xi_struct_decl>                                     structdecl;

};



}


#endif /* XCC_XI_XI_CONTEXT_HPP_ */
