/*
 * ast_type_set.cpp
 *
 *  Created on: Feb 28, 2018
 *      Author: derick
 */

#include "ast_type_set.hpp"
#include "ast_type_func.hpp"

namespace xcc {


ast_typeset::ast_typeset(ast_sametype_func* st) noexcept
        : _sametype(st) {
    // do nothing
}

ast_typeset::bin_t& ast_typeset::getbin(ast_type* t) noexcept {
    auto id = t->get_tree_type();
    if(_bins.find(id) == _bins.end()) {
        _bins[id] = bin_t();
    }
    return _bins[id];
}

void ast_typeset::addtype(ast_type* t) noexcept {
    _all.push_back(box(t));
    this->getbin(t).insert(t);
}

ast_type* ast_typeset::get(ast_type* tp) noexcept {
    auto bin = this->getbin(tp);
    if(bin.find(tp) == bin.end()) {
        for(auto stored_type : bin) {
            if(_sametype->visit(tp, stored_type)) {
                return stored_type;
            }
        }
        this->addtype(tp);
    }
    return tp;
}

}

