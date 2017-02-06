/*
 * xi_builder.hpp
 *
 *  Created on: Feb 1, 2017
 *      Author: derick
 */

#ifndef XI_XI_BUILDER_HPP_
#define XI_XI_BUILDER_HPP_

#include <unordered_map>
#include "ast_builder.hpp"

namespace xcc {

struct xi_scope;

struct xi_builder : public ast_builder<> {
public:

    xi_builder();
    virtual ~xi_builder() = default;

    virtual ast_type*                               get_named_type(const char* name);
    virtual ast_variable_decl*                      define_variable(ast_type*, const char*);

private:

    ptr<xi_scope>                                                       _scope;

};

struct xi_scope {
public:

    xi_scope() = default;
    virtual ~xi_scope() = default;

    virtual ast_type*                               find_type(const char* name) = 0;
    virtual void                                    add_type(const char* name, ast_type* tp) = 0;
    virtual void                                    add_decl(const char* name, ast_variable_decl* decl) = 0;

};

struct xi_global_scope : public xi_scope {
public:

    xi_global_scope() = default;
    virtual ~xi_global_scope() = default;

    virtual ast_type*                               find_type(const char* name) final override;
    virtual void                                    add_type(const char* name, ast_type* tp) final override;
    virtual void                                    add_decl(const char* name, ast_variable_decl* tp) final override;

private:

    std::unordered_map<std::string, ptr<ast_type>>                      _named_types;
    std::unordered_map<std::string, ptr<ast_decl>>                      _named_decls;


};

}



#endif /* XI_XI_BUILDER_HPP_ */
