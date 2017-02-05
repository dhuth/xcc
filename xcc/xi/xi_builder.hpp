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

private:

    ptr<xi_scope>                                                       _scope;

};

struct xi_scope {
public:

    xi_scope() = default;
    virtual ~xi_scope() = default;

    virtual ast_type*                               find_type(const char* name) = 0;
    virtual void                                    add_type(const char* name, ast_type* tp) = 0;

};

struct xi_global_scope : public xi_scope {
public:

    xi_global_scope() = default;
    virtual ~xi_global_scope() = default;

    virtual ast_type*                               find_type(const char* name) final override;
    virtual void                                    add_type(const char* name, ast_type* tp);

private:

    std::unordered_map<std::string, ptr<ast_type>>                      _named_types;


};

}



#endif /* XI_XI_BUILDER_HPP_ */
