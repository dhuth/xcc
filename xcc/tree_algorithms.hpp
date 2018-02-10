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
inline T first(__tree_list_value<T>& l) {
    return *begin(l);
}

template<typename T>
inline T* first(__tree_list_tree<T>& l) {
    return *begin(l);
}

template<typename T>
inline typename __tree_property_list<T>::element_t first(__tree_property_list<T>& l) {
    return *begin(l);
}

template<typename T>
inline T first(__tree_list_value<T>* l) {
    return *begin(l);
}

template<typename T>
inline T* first(__tree_list_tree<T>* l) {
    return *begin(l);
}

template<typename T>
inline typename __tree_property_list<T>::element_t first(__tree_property_list<T>* l) {
    return *begin(l);
}

template<typename T>
inline T first(ptr<__tree_list_value<T>> l) {
    return *begin(l);
}

template<typename T>
inline T* first(ptr<__tree_list_tree<T>> l) {
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
inline enable_if_base_of_t<TDestEl, TSrcEl, __tree_list_tree<TDestEl>*>
__copy_list_as(__tree_list_tree<TSrcEl>* slist) {
    auto newlist = new __tree_list_tree<TDestEl>();
    for(auto st: *slist) {
        newlist->append(st->template as<TDestEl>());
    }
    return newlist;
}

template<typename TSrcEl,
         typename TDestEl>
inline enable_if_base_of_t<TDestEl, TSrcEl, __tree_list_tree<TDestEl>*>
copy_list_as(__tree_list_tree<TSrcEl>* slist) {
    return __copy_list_as<TDestEl>(slist);
}

// Map Algorithm
// -------------

template<typename TSrcEl,
         typename TDestEl>
inline __tree_list_tree<TDestEl>* __map(__tree_list_tree<TSrcEl>* slist, std::function<TDestEl*(TSrcEl*)> f) {
    __tree_list_tree<TDestEl>* dlist = new __tree_list_tree<TDestEl>();
    for(auto el: *slist) {
        dlist->push_back(f(el));
    }
    return dlist;
}

template<typename TSrcEl,
         typename TDestEl>
inline __tree_list_value<TDestEl>* __map(__tree_list_value<TSrcEl>* slist, std::function<TDestEl(TSrcEl)> f) {
    __tree_list_value<TDestEl>* dlist = new __tree_list_value<TDestEl>();
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
inline ptr<__tree_list_tree<_TDestEl>> map(ptr<__tree_list_tree<TSrcEl>>& slist, TFunc f) {
    return box(__map(unbox(slist), std::function<_TDestEl*(TSrcEl*)>(f)));
}

template<typename TSrcEl,
         typename TFunc,
         typename _TDestEl = std::remove_pointer_t<std::result_of_t<TFunc(TSrcEl*)>>>
inline ptr<__tree_list_tree<_TDestEl>> map(ptr<__tree_list_tree<TSrcEl>>&& slist, TFunc f) {
    return box(__map(unbox(slist), std::function<_TDestEl*(TSrcEl*)>(f)));
}

template<typename TSrcEl,
         typename TFunc,
         typename _TDestEl = std::remove_pointer_t<std::result_of_t<TFunc(TSrcEl*)>>>
inline ptr<__tree_list_tree<_TDestEl>> map(__tree_list_tree<TSrcEl>* slist, TFunc f) {
    return box(__map(slist, std::function<_TDestEl*(TSrcEl*)>(f)));
}

template<typename TSrcEl,
         typename TFunc,
         typename _TDestEl = std::remove_pointer_t<std::result_of_t<TFunc(TSrcEl*)>>>
inline ptr<__tree_list_tree<_TDestEl>> map(__tree_property_list<TSrcEl>& slist, TFunc f) {
    return box(__map((__tree_list_tree<TSrcEl>*) slist, std::function<_TDestEl*(TSrcEl*)>(f)));
}

// Filter algorithm
// ----------------

template<typename TTreeType,
         typename TPred>
inline __tree_list_tree<TTreeType>* __filter(__tree_list_tree<TTreeType>* slist, TPred pred) {
    __tree_list_tree<TTreeType>* dlist = new __tree_list_tree<TTreeType>();
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
inline ptr<__tree_list_tree<TTreeType>> filter(ptr<__tree_list_tree<TTreeType>>& slist, TPred pred) {
    return box(__filter(unbox(slist), pred));
}

template<typename TTreeType,
         typename TPred>
inline ptr<__tree_list_tree<TTreeType>> filter(ptr<__tree_list_tree<TTreeType>>&& slist, TPred pred) {
    return box(__filter(unbox(slist), pred));
}

template<typename TTreeType,
         typename TPred>
inline ptr<__tree_list_tree<TTreeType>> filter(__tree_property_list<TTreeType>& slist, TPred pred) {
    return box(__filter((__tree_list_tree<TTreeType>*) slist, pred));
}

}



#endif /* XCC_TREE_ALGORITHMS_HPP_ */
