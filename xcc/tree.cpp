/*
 * tree.cpp
 *
 *  Created on: Jan 19, 2017
 *      Author: derick
 */


#include <cstring>
#include <new>
#include "tree.hpp"
#include "all_tree_headers.def.hpp"


namespace xcc {

template<tree_type_id Id>
__tree_base* __shallow_clone_tree(__tree_base* src) {
    typedef typename __tree_id_to_type<Id>::type T;
    if(src != nullptr) {
        T* new_tree = (T*) operator new(sizeof(T));
        memcpy(new_tree, src, sizeof(T));
        return new_tree;
    }
    return nullptr;
}

template<>
__tree_base* __shallow_clone_tree<tree_type_id::tree_list>(__tree_base* src) {
    if(src != nullptr) {
        return src->__shallow_clone();
    }
    return nullptr;
}

#define SHALLOW_CLONE_FUNC_NAME(name)   __shallow_clone_tree<tree_type_id::name>
#define SHALLOW_CLONE_FUNC(name)


#ifdef   TREE_TYPE
#error   "TREE_TYPE Already defined"
#endif
#define  TREE_TYPE(name, ...)           SHALLOW_CLONE_FUNC(name)
#include "all_tree_types.def.hpp"
#undef   TREE_TYPE

const tree_type __all_tree_types[] = {
        {tree_type_id::tree,            tree_type_id::tree,     "tree",         NULL},
        {tree_type_id::dummy,           tree_type_id::tree,     "dummy",        NULL},
        {tree_type_id::tree_list,       tree_type_id::tree,     "list",         &__shallow_clone_tree<tree_type_id::tree_list>},
#ifdef   TREE_TYPE
#error   "TREE_TYPE Already defined"
#endif
#define  TREE_TYPE(name, base)                                  {tree_type_id::name, tree_type_id::base, #name, &SHALLOW_CLONE_FUNC_NAME(name)},
#include "all_tree_types.def.hpp"
#undef   TREE_TYPE
        {tree_type_id::__type_count,    tree_type_id::tree,     NULL,           NULL}
};

}

