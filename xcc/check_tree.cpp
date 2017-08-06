/*
 * check_tree.cpp
 *
 *  Created on: Jan 20, 2017
 *      Author: derick
 */

#include "tree.hpp"
#include "ast.hpp"
#include "ast_builder.hpp"
#include "gtest/gtest.h"

namespace xcc {

class TreeTest : public ::testing::Test {
public:
    TreeTest(): void_tp(),
                i32(32, false),
                u32(32, true),
                u64(64, true),
                f32(32) {
        void_tp.pin();
        i32.pin();
        u32.pin();
        u64.pin();
        f32.pin();
    }

    ast_void_type                                               void_tp;
    ast_integer_type                                            i32;
    ast_integer_type                                            u32;
    ast_integer_type                                            u64;
    ast_real_type                                               f32;
    ast_default_name_mangler                                    mangler;
};

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
    ASSERT_EQ((uint32_t)this->u32.bitwidth,    32);
    ASSERT_EQ((bool)    this->u32.is_unsigned, true);

    ASSERT_EQ(pass<uint64_t>(this->u32.bitwidth), 32);
    ASSERT_EQ(pass<int32_t>(this->u32.bitwidth),  32);
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

TEST_F(TreeTest, TreeList) {
    list<ast_type> type_list({
        new ast_void_type(),
        new ast_integer_type(32, false),
        new ast_real_type(32)
    });

    for(auto tp : type_list) {
        ASSERT_TRUE(tp->is<ast_type>());
    }

    ASSERT_TRUE(type_list[0]->is<ast_void_type>());
    ASSERT_TRUE(type_list[1]->is<ast_integer_type>());
    ASSERT_TRUE(type_list[2]->is<ast_real_type>());
}

TEST_F(TreeTest, TestMangle_Void) {
    ASSERT_EQ(mangler(&void_tp), "v");
}

TEST_F(TreeTest, TestMangle_Int) {
    ASSERT_EQ(mangler(&u32), "j");
    ASSERT_EQ(mangler(&i32), "i");
}

TEST_F(TreeTest, TestMangle_Float) {
    ASSERT_EQ(mangler(&f32), "f");
}

TEST_F(TreeTest, TestMamgler_Array) {
    ast_array_type arr_tp(&f32, 6);
    ASSERT_EQ(mangler(&arr_tp), "A6_f");
}

TEST_F(TreeTest, TestMangler_Pointer) {
    ast_pointer_type ptr_tp(&u64);
    ASSERT_EQ(mangler(&ptr_tp), "Pm");
}

TEST_F(TreeTest, TestMangler_Function) {
    ast_function_type func_tp(&i32, new list<ast_type>({&f32, new ast_pointer_type(&u64)}));
    ASSERT_EQ(mangler(&func_tp), "FifPmE");
}

TEST_F(TreeTest, TestMangler_FunctionDecl) {
    ast_function_decl foo("foo", &void_tp, new list<ast_parameter_decl>({new ast_parameter_decl("x", &u32)}), nullptr);
    ASSERT_EQ(mangler(&foo), "_ZfooFvjE");
}

}



int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
