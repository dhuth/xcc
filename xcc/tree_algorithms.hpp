/*
 * tree_algorithms.hpp
 *
 *  Created on: Feb 9, 2018
 *      Author: derick
 */

#ifndef XCC_TREE_ALGORITHMS_HPP_
#define XCC_TREE_ALGORITHMS_HPP_

#include "tree_base.hpp"
#include "tree_reference.hpp"
#include "tree_list.hpp"

namespace xcc {

template<typename T>
inline T first(value_list<T>& l) {
    return *begin(l);
}

template<typename T>
inline T* first(tree_list<T>& l) {
    return *begin(l);
}

template<typename T>
inline typename __tree_property_list<T>::element_t first(__tree_property_list<T>& l) {
    return *begin(l);
}

template<typename T>
inline T first(value_list<T>* l) {
    return *begin(l);
}

template<typename T>
inline T* first(tree_list<T>* l) {
    return *begin(l);
}

template<typename T>
inline typename __tree_property_list<T>::element_t first(__tree_property_list<T>* l) {
    return *begin(l);
}

template<typename T>
inline T first(ptr<value_list<T>> l) {
    return *begin(l);
}

template<typename T>
inline T* first(ptr<tree_list<T>> l) {
    return *begin(l);
}

template<typename T>
inline typename __tree_property_list<T>::element_t first(ptr<__tree_property_list<T>> l) {
    return *begin(l);
}

// Copy As Algorithm
// -----------------

template<typename TSrcEl,
         typename TDestEl>
inline enable_if_base_of_t<TDestEl, TSrcEl, tree_list<TDestEl>*>
__copy_list_as(tree_list<TSrcEl>* slist) {
    auto newlist = new tree_list<TDestEl>();
    for(auto st: *slist) {
        newlist->append(st->template as<TDestEl>());
    }
    return newlist;
}

template<typename TSrcEl,
         typename TDestEl>
inline enable_if_base_of_t<TDestEl, TSrcEl, tree_list<TDestEl>*>
copy_list_as(tree_list<TSrcEl>* slist) {
    return __copy_list_as<TDestEl>(slist);
}

// Map Algorithm
// -------------

template<typename TSrcEl,
         typename TDestEl>
inline tree_list<TDestEl>* __map(tree_list<TSrcEl>* slist, std::function<TDestEl*(TSrcEl*)> f) {
    tree_list<TDestEl>* dlist = new tree_list<TDestEl>();
    for(auto el: *slist) {
        dlist->push_back(f(el));
    }
    return dlist;
}

template<typename TSrcEl,
         typename TDestEl>
inline value_list<TDestEl>* __map(value_list<TSrcEl>* slist, std::function<TDestEl(TSrcEl)> f) {
    value_list<TDestEl>* dlist = new value_list<TDestEl>();
    for(auto el: *slist) {
        dlist->push_back(f(el));
    }
    return dlist;
}

//template<typename TSrcEl,
//         typename TFunc,
//         typename _TDestEl = std::remove_pointer_t<std::result_of_t<TFunc(TSrcEl*)>>>
//inline ptr<__tree_list_tree<_TDestEl>> map(ptr<__tree_list_tree<TSrcEl>> slist, TFunc f) {
//    return box(__map(unbox(slist), std::function<_TDestEl*(TSrcEl*)>(f)));
//}

template<typename TSrcEl,
         typename TFunc,
         typename _TDestEl = std::remove_pointer_t<std::result_of_t<TFunc(TSrcEl*)>>>
inline ptr<tree_list<_TDestEl>> map(ptr<tree_list<TSrcEl>>& slist, TFunc f) {
    return box(__map(unbox(slist), std::function<_TDestEl*(TSrcEl*)>(f)));
}

template<typename TSrcEl,
         typename TFunc,
         typename _TDestEl = std::remove_pointer_t<std::result_of_t<TFunc(TSrcEl*)>>>
inline ptr<tree_list<_TDestEl>> map(ptr<tree_list<TSrcEl>>&& slist, TFunc f) {
    return box(__map(unbox(slist), std::function<_TDestEl*(TSrcEl*)>(f)));
}

template<typename TSrcEl,
         typename TFunc,
         typename _TDestEl = std::remove_pointer_t<std::result_of_t<TFunc(TSrcEl*)>>>
inline ptr<tree_list<_TDestEl>> map(tree_list<TSrcEl>* slist, TFunc f) {
    return box(__map(slist, std::function<_TDestEl*(TSrcEl*)>(f)));
}

template<typename TSrcEl,
         typename TFunc,
         typename _TDestEl = std::remove_pointer_t<std::result_of_t<TFunc(TSrcEl*)>>>
inline ptr<tree_list<_TDestEl>> map(__tree_property_list<TSrcEl>& slist, TFunc f) {
    return box(__map((tree_list<TSrcEl>*) slist, std::function<_TDestEl*(TSrcEl*)>(f)));
}

// Filter algorithm
// ----------------

template<typename TTreeType,
         typename TPred>
inline tree_list<TTreeType>* __filter(tree_list<TTreeType>* slist, TPred pred) {
    tree_list<TTreeType>* dlist = new tree_list<TTreeType>();
    auto f = std::function<bool(TTreeType*)>(pred);
    for(auto el: *slist) {
        if(f(el)) {
            dlist->push_back(el);
        }
    }
    return dlist;
}

template<typename TTreeType,
         typename TPred>
inline ptr<tree_list<TTreeType>> filter(ptr<tree_list<TTreeType>>& slist, TPred pred) {
    return box(__filter(unbox(slist), pred));
}

template<typename TTreeType,
         typename TPred>
inline ptr<tree_list<TTreeType>> filter(ptr<tree_list<TTreeType>>&& slist, TPred pred) {
    return box(__filter(unbox(slist), pred));
}

template<typename TTreeType,
         typename TPred>
inline ptr<tree_list<TTreeType>> filter(__tree_property_list<TTreeType>& slist, TPred pred) {
    return box(__filter((tree_list<TTreeType>*) slist, pred));
}

}



#endif /* XCC_TREE_ALGORITHMS_HPP_ */
