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
inline T first(value_list<T>* l) {
    return *begin(l);
}

template<typename T>
inline T* first(tree_list<T>* l) {
    return *begin(l);
}

template<typename T>
inline T first(ptr<value_list<T>> l) {
    return *l->begin();
}

template<typename T>
inline T* first(ptr<tree_list<T>> l) {
    return *l->begin();
}

template<typename T>
inline T first(reference<value_list<T>>& l) {
    return *l->begin();
}

template<typename T>
inline T* first(reference<tree_list<T>>& l) {
    return *l->begin();
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

template<typename _SrcEl,
         typename _DestEl>
inline ptr<tree_list<_DestEl>> __map(
                tree_list<_SrcEl>* slist,
                std::function<_DestEl*(_SrcEl*)> f) {
    ptr<tree_list<_DestEl>> dlist = box(new tree_list<_DestEl>());
    for(auto el: *slist) {
        dlist->push_back(f(el));
    }
    return dlist;
}

template<typename _SrcEl,
         typename _DestEl>
inline ptr<value_list<_DestEl>> __map(
                value_list<_SrcEl> slist,
                std::function<_DestEl*(_SrcEl*)> f) {
    ptr<value_list<_DestEl>> dlist = box(new value_list<_DestEl>());
    for(auto el: *slist) {
        dlist->push_back(f(el));
    }
    return dlist;
}

template<typename TSrcEl,
         typename TFunc,
         typename _TDestEl = std::remove_pointer_t<std::result_of_t<TFunc(TSrcEl*)>>>
inline ptr<tree_list<_TDestEl>> map(ptr<tree_list<TSrcEl>> slist, TFunc f) {
    return __map(unbox(slist), std::function<_TDestEl*(TSrcEl*)>(f));
}

template<typename TSrcEl,
         typename TFunc,
         typename _TDestEl = std::remove_pointer_t<std::result_of_t<TFunc(TSrcEl*)>>>
inline ptr<tree_list<_TDestEl>> map(tree_list<TSrcEl>* slist, TFunc f) {
    return __map(slist, std::function<_TDestEl*(TSrcEl*)>(f));
}

template<typename _SrcEl,
         typename _Func,
         typename _DestEl = std::remove_pointer_t<std::result_of_t<_Func(_SrcEl*)>>>
 inline ptr<tree_list<_DestEl>> map(reference<tree_list<_SrcEl>>& slist, _Func f) {
    return __map((tree_list<_SrcEl>*)slist, std::function<_DestEl*(_SrcEl*)>(f));
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

}



#endif /* XCC_TREE_ALGORITHMS_HPP_ */
