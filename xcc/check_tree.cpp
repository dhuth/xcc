/*
 * check_tree.cpp
 *
 *  Created on: Jan 20, 2017
 *      Author: derick
 */

#include "tree.hpp"
#include "ast.hpp"
#include "gtest/gtest.h"

namespace xcc {

class TreeTest : public ::testing::Test { };

TEST_F(TreeTest, TreeTypeAssertions) {
    for(size_t i = 0; i < tree_type::count(); i++) {
        ASSERT_EQ(__all_tree_types[i].id, (tree_type_id)i);
    }

    ASSERT_TRUE(tree_type::is_base_of(tree_type_id::ast_tree, tree_type_id::ast_expr));
    ASSERT_TRUE(tree_type::is_base_of(tree_type_id::ast_tree, tree_type_id::ast_integer));
}

template<typename T>
static T pass(T value) { return value; }

TEST_F(TreeTest, BasicPropertyTest) {
    ast_integer_type tp(32, false);
    uint32_t bw = tp.bitwidth;
    bool     us = tp.is_unsigned;

    ASSERT_EQ((uint32_t)tp.bitwidth, 32);
    ASSERT_EQ((bool)tp.is_unsigned, false);

    ASSERT_EQ(pass<uint64_t>(tp.bitwidth), 32);
    ASSERT_EQ(pass<int32_t>(tp.bitwidth),  32);
}

TEST_F(TreeTest, BasicList) {
    list<uint32_t> ilist1({2,3});

    ASSERT_EQ(ilist1[0], 2);
    ASSERT_EQ(ilist1[1], 3);

    list<uint32_t> ilist2(1, &ilist1);
    list<uint32_t> ilist3(&ilist2, 4);
    list<uint32_t> ilist4(0, &ilist3);

    ASSERT_EQ(ilist4[0], 0);
    ASSERT_EQ(ilist4[1], 1);
    ASSERT_EQ(ilist4[2], 2);
    ASSERT_EQ(ilist4[3], 3);
    ASSERT_EQ(ilist4[4], 4);

}

TEST_F(TreeTest, TreePropertyTest) {
    ast_array_type array_tp(new ast_integer_type(64, false), 4);

    uint64_t gtsize = array_tp.size;
    ASSERT_EQ(gtsize, 4);

    ast_integer_type* i64_t = array_tp.element_type;
    ASSERT_EQ(i64_t->bitwidth, 64);
    ASSERT_EQ(i64_t->is_unsigned, false);
}

}



int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
