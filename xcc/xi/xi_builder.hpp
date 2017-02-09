/*
 * xi_builder.hpp
 *
 *  Created on: Feb 1, 2017
 *      Author: derick
 */

#ifndef XI_XI_BUILDER_HPP_
#define XI_XI_BUILDER_HPP_

#include <unordered_map>
#include "frontend.hpp"
#include "xi_tree.hpp"
#include "ast_builder.hpp"

namespace xcc {

struct xi_scope {
public:

    inline xi_scope() = default;
    inline xi_scope(ptr<xi_scope> p): parent(p) { }
    virtual ~xi_scope() = default;

    virtual ast_type*                               find_type(const char* name) = 0;
    virtual ast_decl*                               find_decl(const char* name) = 0;
    virtual void                                    add_type(const char* name, ast_type* tp) = 0;
    virtual void                                    add_decl(const char* name, ast_variable_decl* decl) = 0;

    ptr<xi_scope>                                                       parent;

protected:

    std::unordered_map<std::string, ptr<ast_type>>                      named_types;
    std::unordered_map<std::string, ptr<ast_decl>>                      named_decls;

private:

    std::unordered_map<std::string, ptr<xi_scope>>                      _using_scopes;

};

struct xi_global_scope : public xi_scope {
public:

    xi_global_scope() = default;
    virtual ~xi_global_scope() = default;

    virtual ast_type*                               find_type(const char* name) final override;
    virtual ast_decl*                               find_decl(const char* name) final override;
    virtual void                                    add_type(const char* name, ast_type* tp) final override;
    virtual void                                    add_decl(const char* name, ast_variable_decl* tp) final override;


};

struct xi_builder : public ast_builder<> {
public:

    xi_builder(translation_unit& tu);
    virtual ~xi_builder() = default;

            xi_const_type*                          get_const_type(ast_type*) const noexcept;
    virtual ast_type*                               identifier_as_type(const char* name);
    virtual void                                    define_typedef(const char*, ast_type*);
    virtual ast_variable_decl*                      define_global_variable(ast_type*, const char*, bool is_extern = false);
    virtual ast_variable_decl*                      define_global_variable(ast_type*, const char*, ast_expr*);

    virtual ast_expr*                               identifier_as_expr(const char*);

            ast_expr*                               make_op(xi_operator op, ast_expr*);
            ast_expr*                               make_op(xi_operator op, ast_expr*, ast_expr*);

private:

    ptr<xi_scope>                                                       scope;
    translation_unit&                                                   tu;

    template<typename TScope, typename... TArgs>
    inline void push_scope(TArgs... args) {
        this->scope = box(new TScope(this->scope, args...));
    }

    inline void pop_scope() {
        this->scope = this->scope->parent;
    }

};

}



#endif /* XI_XI_BUILDER_HPP_ */
