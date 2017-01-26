/*
 * tree.cpp
 *
 *  Created on: Jan 19, 2017
 *      Author: derick
 */


#include "tree.hpp"

namespace xcc {

const tree_type __all_tree_types[] = {
        {tree_type_id::tree,            tree_type_id::tree,     "tree"},
#ifdef   TREE_TYPE
#error   "TREE_TYPE Already defined"
#endif
#define  TREE_TYPE(name, base)                                  {tree_type_id::name, tree_type_id::base, #name},
#include "all_tree_types.def.hpp"
#undef   TREE_TYPE
        {tree_type_id::tree,             tree_type_id::tree}
};

}

