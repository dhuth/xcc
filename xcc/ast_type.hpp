/*
 * ast_type.hpp
 *
 *  Created on: Feb 28, 2018
 *      Author: derick
 */

#ifndef XCC_AST_TYPE_HPP_
#define XCC_AST_TYPE_HPP_

#include "ast.hpp"
#include "ast_type_fwd.hpp"
#include "ast_type_set.hpp"
#include "ast_type_func.hpp"

namespace xcc {

struct __ast_builder_impl;

struct ast_type_provider {
public:

    explicit ast_type_provider() noexcept = default;
    virtual ~ast_type_provider() = default;

    void initialize(const __ast_builder_impl&) noexcept;

    ast_sametype_func*      get_sametype_func()     const noexcept;
    ast_widens_func*        get_widens_func()       const noexcept;
    ast_maxtype_func*       get_maxtype_func()      const noexcept;
    ast_cast_func*          get_cast_func()         const noexcept;
    ast_typeset*            get_typeset()           const noexcept;

    template<typename T>
    inline treemap<ast_type, T> create_typemap() const noexcept {
        return treemap<ast_type, T>(
                [=](ast_type* l, ast_type* r)->bool {
                    return _sametype_ptr->visit(l, r);
                });
    }

protected:

    virtual ast_sametype_func*      create_sametype_func(const __ast_builder_impl&) noexcept;
    virtual ast_widens_func*        create_widens_func(const __ast_builder_impl&)   noexcept;
    virtual ast_maxtype_func*       create_maxtype_func(const __ast_builder_impl&)  noexcept;
    virtual ast_cast_func*          create_cast_func(const __ast_builder_impl&)     noexcept;
    virtual ast_typeset*            create_typeset(const __ast_builder_impl&)       noexcept;

private:

    ptr<ast_sametype_func>                                  _sametype_ptr;
    ptr<ast_widens_func>                                    _widens_ptr;
    ptr<ast_maxtype_func>                                   _maxtype_ptr;
    ptr<ast_cast_func>                                      _cast_ptr;
    ptr<ast_typeset>                                        _typeset_ptr;
};



}


#endif /* XCC_AST_TYPE_HPP_ */
