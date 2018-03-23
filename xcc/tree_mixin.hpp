/*
 * tree_mixin.hpp
 *
 *  Created on: Mar 5, 2018
 *      Author: derick
 */

#ifndef XCC_TREE_MIXIN_HPP_
#define XCC_TREE_MIXIN_HPP_

#include "tree_base.hpp"
#include "tree_reference.hpp"
#include "tree_list.hpp"

namespace xcc {

template<typename       _MixinType>
struct mixin : public _MixinType {
public:

    template<typename...    _Args>
    explicit inline mixin(tree_t* t, _Args&&... args) noexcept
            : _MixinType(t, std::forward<_Args>(args)...) {
        // do nothing
    }

    explicit inline mixin(tree_t* t, const _MixinType& m) noexcept
            : _MixinType(t, m) {
        // do nothing
    }

};


}


#endif /* XCC_TREE_MIXIN_HPP_ */
