/*
 * ast_type_set.hpp
 *
 *  Created on: Feb 28, 2018
 *      Author: derick
 */

#ifndef XCC_AST_TYPE_SET_HPP_
#define XCC_AST_TYPE_SET_HPP_

#include "ast_builder.hpp"
#include <map>
#include <set>

namespace xcc {

struct ast_typeset {
public:

    explicit inline ast_typeset(ast_sametype_func* st) noexcept
            : _sametype(st) {
        // do nothing
    }

    ast_type* get(ast_type*) noexcept;

    template<typename    _T,
             typename... _Args>
    _T* get_new(_Args&&... args) {
        auto new_t = new _T(std::forward<_Args>(args)...);
        auto old_t = this->get(new_t);
        if(new_t != old_t) {
            delete new_t;
        }
        return old_t->template as<_T>();
    }

protected:

    typedef std::set<ast_type*>                             bin_t;

    bin_t& getbin(ast_type*) noexcept;
    void   addtype(ast_type*) noexcept;

private:

    ast_sametype_func*                                      _sametype;
    std::map<tree_type_id, bin_t>                           _bins;
    std::vector<ptr<ast_type>>                              _all;
};

}


#endif /* XCC_AST_TYPE_SET_HPP_ */
