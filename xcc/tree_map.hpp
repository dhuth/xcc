/*
 * tree_map.hpp
 *
 *  Created on: Mar 23, 2018
 *      Author: derick
 */

#ifndef XCC_TREE_MAP_HPP_
#define XCC_TREE_MAP_HPP_

#include "tree_base.hpp"
#include <functional>

namespace xcc {

template<typename           _Key,
         typename           _Value,
         enable_if_tree_t<_Key, int> = 0>
struct treemap {
public:

    typedef std::pair<_Key*, _Value>                        kv_t;
    typedef std::vector<kv_t>                               bin_t;

    explicit treemap() = default;
    explicit inline treemap(std::function<bool(_Key*,_Key*)> cfunc) noexcept
            : _compare_func(cfunc) {
        // do nothing
    }

    _Value& operator[](_Key* tp) noexcept {
        return this->getref(tp, tp->get_tree_type());
    }

private:

    inline bin_t& getbin(tree_type_id tt) noexcept {
        return _bins.at(tt);
    }

    inline _Value& getref(_Key* tp, tree_type_id tt) noexcept {
        auto&       bin = this->getbin(tt);
        for(auto& p: bin) {
            if(_compare_func(tp, p.first)) {
                return p.second;
            }
        }

        bin.emplace_back();
        bin.back().first = tp;
        return bin.back().second;
    }

    std::function<bool(_Key*, _Key*)>                       _compare_func;
    std::map<tree_type_id, bin_t>                           _bins;

};

}



#endif /* XCC_TREE_MAP_HPP_ */
