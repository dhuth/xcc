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

#ifdef   TREE_TYPE
#error   "TREE_TYPE Already defined"
#endif

const tree_type __all_tree_types[] = {
        {tree_type_id::tree,            tree_type_id::tree,     "tree"},
        {tree_type_id::dummy,           tree_type_id::tree,     "dummy"},
        {tree_type_id::tree_list,       tree_type_id::tree,     "list"},
#ifdef   TREE_TYPE
#error   "TREE_TYPE Already defined"
#endif
#define  TREE_TYPE(name, base)                                  {tree_type_id::name, tree_type_id::base, #name},
#include "all_tree_types.def.hpp"
#undef   TREE_TYPE
        {tree_type_id::__type_count,    tree_type_id::tree,     NULL}
};

}

