/*
 * check_ast.cpp
 *
 *  Created on: Jan 25, 2017
 *      Author: derick
 */

#include "tree.hpp"
#include "ast.hpp"
#include "ast_builder.hpp"
#include "frontend.hpp"
#include "gtest/gtest.h"

namespace xcc {

class AstTest : public ::testing::Test {
public:

    AstTest(): builder(unit) {
        this->void_tp                       = builder.get_void_type();
        this->i32                           = builder.get_integer_type(32, false);
        this->u32                           = builder.get_integer_type(32, true);
        this->i64                           = builder.get_integer_type(64, false);
        this->u64                           = builder.get_integer_type(64, true);
        this->f32                           = builder.get_real_type(32);
        this->f64                           = builder.get_real_type(64);

        this->u32x6                         = builder.get_array_type(u32, 6);
        this->u32x8                         = builder.get_array_type(u32, 8);
        this->i32x6                         = builder.get_array_type(i32, 6);
        this->i32x8                         = builder.get_array_type(i32, 8);

        this->f64p                          = nullptr;
        this->u32p                          = nullptr;
        this->f64pp                         = nullptr;
        this->u32pp                         = nullptr;

        this->func_v_v                      = nullptr;
        this->func_v_i32                    = nullptr;
        this->func_v_i32f64                 = nullptr;
        this->func_i32_v                    = nullptr;
        this->func_i32_i32                  = nullptr;
        this->func_i32_i32f64               = nullptr;
    }

    void make_basic_pointer_types() {
        this->f64p                          = builder.get_pointer_type(f64);
        this->u32p                          = builder.get_pointer_type(u32);
        this->f64pp                         = builder.get_pointer_type(f64p);
        this->u32pp                         = builder.get_pointer_type(u32p);
    }

    void make_basic_function_types() {
        this->func_v_v                      = builder.get_function_type(void_tp, new list<ast_type>({}));
        this->func_v_i32                    = builder.get_function_type(void_tp, new list<ast_type>({i32}));
        this->func_v_i32f64                 = builder.get_function_type(void_tp, new list<ast_type>({i32, f64}));
        this->func_i32_v                    = builder.get_function_type(i32, new list<ast_type>({}));
        this->func_i32_i32                  = builder.get_function_type(i32, new list<ast_type>({i32}));
        this->func_i32_i32f64               = builder.get_function_type(i32, new list<ast_type>({i32, f64}));
    }

    translation_unit                                            unit;

    ast_builder<>                                               builder;
    ast_void_type*                                              void_tp;
    ast_integer_type*                                           i32;
    ast_integer_type*                                           u32;
    ast_integer_type*                                           i64;
    ast_integer_type*                                           u64;
    ast_real_type*                                              f32;
    ast_real_type*                                              f64;

    ast_array_type*                                             u32x6;
    ast_array_type*                                             u32x8;
    ast_array_type*                                             i32x6;
    ast_array_type*                                             i32x8;

    ast_pointer_type*                                           f64p;
    ast_pointer_type*                                           u32p;
    ast_pointer_type*                                           f64pp;
    ast_pointer_type*                                           u32pp;

    ast_function_type*                                          func_v_v;
    ast_function_type*                                          func_v_i32;
    ast_function_type*                                          func_v_i32f64;
    ast_function_type*                                          func_i32_v;
    ast_function_type*                                          func_i32_i32;
    ast_function_type*                                          func_i32_i32f64;

};

TEST_F(AstTest, TestSameType_Basic) {
    ASSERT_TRUE(builder.sametype(void_tp, void_tp));
    ASSERT_TRUE(builder.sametype(u32, u32));
    ASSERT_FALSE(builder.sametype(i32, u32));
    ASSERT_TRUE(builder.sametype(i32, i32));
    ASSERT_TRUE(builder.sametype(f32, f32));
    ASSERT_FALSE(builder.sametype(f32, f64));
}

TEST_F(AstTest, TestSameType_Array) {
    ASSERT_TRUE(builder.sametype(builder.get_array_type(u32, 6), u32x6));
    ASSERT_TRUE(builder.sametype(builder.get_array_type(u32, 8), u32x8));
    ASSERT_TRUE(builder.sametype(builder.get_array_type(i32, 6), i32x6));
    ASSERT_TRUE(builder.sametype(builder.get_array_type(i32, 8), i32x8));

    ASSERT_FALSE(builder.sametype(u32x6, i32x6));
    ASSERT_FALSE(builder.sametype(u32x6, u32x8));
}

TEST_F(AstTest, TestSameType_Pointers) {
    this->make_basic_pointer_types();

    ASSERT_TRUE(builder.sametype(f64p, f64p));
    ASSERT_TRUE(builder.sametype(u32p, u32p));
    ASSERT_FALSE(builder.sametype(f64p, u32p));
    ASSERT_TRUE(builder.sametype(f64pp, f64pp));

    ASSERT_EQ(builder.get_pointer_type(f64), f64p);
}

TEST_F(AstTest, TestSameType_Functions) {
    this->make_basic_function_types();

    ASSERT_FALSE(builder.sametype(func_v_v,      func_v_i32));
    ASSERT_FALSE(builder.sametype(func_v_i32,    func_v_i32f64));
    ASSERT_FALSE(builder.sametype(func_v_i32f64, func_i32_i32f64));
    ASSERT_FALSE(builder.sametype(func_v_i32f64, func_i32_i32));
    ASSERT_FALSE(builder.sametype(func_v_i32f64, func_i32_v));
}

TEST_F(AstTest, TestMaxType) {
    this->make_basic_pointer_types();

    ASSERT_TRUE(builder.sametype(builder.maxtype(i32, i32), i32));
    ASSERT_TRUE(builder.sametype(builder.maxtype(i32, u32), i32));
    ASSERT_TRUE(builder.sametype(builder.maxtype(i32, f32), f32));
    ASSERT_TRUE(builder.sametype(builder.maxtype(f32, f64), f64));
    ASSERT_TRUE(builder.sametype(builder.maxtype(f32, i64), f64));
}

}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



