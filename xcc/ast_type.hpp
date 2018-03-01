/*
 * ast_type.hpp
 *
 *  Created on: Feb 28, 2018
 *      Author: derick
 */

#ifndef XCC_AST_TYPE_HPP_
#define XCC_AST_TYPE_HPP_

#include "ast_builder.hpp"
#include "ast_type_set.hpp"
#include "ast_type_func.hpp"

namespace xcc {

struct ast_type_provider {

    explicit ast_type_provider() noexcept = default;
    virtual ~ast_type_provider() = default;

    void initialize(const ast_builder_impl_t&) noexcept;

    ast_sametype_func*      get_sametype_func()     const noexcept;
    ast_widens_func*        get_widens_func()       const noexcept;
    ast_maxtype_func*       get_maxtype_func()      const noexcept;
    ast_cast_func*          get_cast_func()         const noexcept;
    ast_typeset*            get_typeset()           const noexcept;

protected:

    virtual ast_sametype_func*      create_sametype_func(const ast_builder_impl_t&) noexcept;
    virtual ast_widens_func*        create_widens_func(const ast_builder_impl_t&)   noexcept;
    virtual ast_maxtype_func*       create_maxtype_func(const ast_builder_impl_t&)  noexcept;
    virtual ast_cast_func*          create_cast_func(const ast_builder_impl_t&)     noexcept;
    virtual ast_typeset*            create_typeset(const ast_builder_impl_t&)       noexcept;

private:

    ptr<ast_sametype_func>                                  _sametype_ptr;
    ptr<ast_widens_func>                                    _widens_ptr;
    ptr<ast_maxtype_func>                                   _maxtype_ptr;
    ptr<ast_cast_func>                                      _cast_ptr;
    ptr<ast_typeset>                                        _typeset_ptr;
};



}


#endif /* XCC_AST_TYPE_HPP_ */
