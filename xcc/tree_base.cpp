/*
 * tree.cpp
 *
 *  Created on: Jan 19, 2017
 *      Author: derick
 */


#include "tree_base.hpp"

#include <cstring>
#include <new>
#include "all_tree_headers.def.hpp"


namespace xcc {

template<typename T>
static __tree_base* __clone_tree(const __tree_base* b) {
    if(b != nullptr) {
        return new T(*static_cast<const T*>(b));
    }
    return nullptr;
}

#ifdef   TREE_TYPE
#error   "TREE_TYPE Already defined"
#endif

const tree_type __all_tree_types[] = {
        {tree_type_id::tree,            tree_type_id::tree,     "tree",     NULL},
        {tree_type_id::dummy,           tree_type_id::tree,     "dummy",    NULL},
        {tree_type_id::tree_list,       tree_type_id::tree,     "list",     NULL},
#ifdef   TREE_TYPE
#error   "TREE_TYPE Already defined"
#endif
#define  TREE_TYPE(name, base)\
        {tree_type_id::name,            tree_type_id::base,     #name,      &__clone_tree<name>},
#include "all_tree_types.def.hpp"
#undef   TREE_TYPE
        {tree_type_id::__type_count,    tree_type_id::tree,     NULL,       NULL}
};

}

