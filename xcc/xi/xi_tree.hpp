#ifndef XI_XI_TREE_HPP_
#define XI_XI_TREE_HPP_

#include "ast.hpp"

namespace xcc {


struct xi_type : public extend_tree<tree_type_id::xi_type, ast_type> {

    inline xi_type(tree_type_id id) noexcept : base_type(id) {
        /* do nothing */
    }

};


struct xi_decl : public extend_tree<tree_type_id::xi_decl, ast_decl> {

    inline xi_decl(tree_type_id id, std::string name) noexcept : base_type(id, name) {
        /* do nothing */
    }
};


struct xi_expr : public extend_tree<tree_type_id::xi_expr, ast_expr> {

    inline xi_expr(tree_type_id id) noexcept : base_type(id) {
        /* do nothing */
    }

    inline xi_expr(tree_type_id id, ast_type* type) noexcept : base_type(id, type) {
        /* do nothing */
    }

};


struct xi_stmt : public extend_tree<tree_type_id::xi_stmt, ast_stmt> {

    inline xi_stmt(tree_type_id id) noexcept : base_type(id) {
        /* do nothing */
    }

};


struct xi_const_type : public extend_tree<tree_type_id::xi_const_type, xi_type> {

    inline xi_const_type(ast_type* type) noexcept
            : base_type(),
              type(this, type) {
        /* do nothing */
    }

    property<ast_type>                                              type;

};


struct xi_auto_type : public extend_tree<tree_type_id::xi_auto_type, xi_type> {

    inline xi_auto_type() noexcept
            : base_type() {
        /* do nothing */
    }
};


struct xi_array_type : public extend_tree<tree_type_id::xi_array_type, xi_type> {

    inline xi_array_type(ast_type* type, ast_expr* size)
            : base_type(),
              type(this, type),
              size(this, size) {
        /* do nothing */
    }

    property<ast_type>                                              type;
    property<ast_expr>                                              size;

};


struct xi_reference_type : public extend_tree<tree_type_id::xi_reference_type, xi_type> {

    inline xi_reference_type(ast_type* type) noexcept
            : base_type(),
              type(this, type) {
        /* do nothing */
    }

    property<ast_type>                                              type;

};


struct xi_object_type : public extend_tree<tree_type_id::xi_object_type, xi_type> {

    inline xi_object_type(xi_decl* decl) noexcept
            : base_type(),
              declaration(this, decl) {
        /* do nothing */
    }

    property<xi_decl>                                               declaration;

};


struct xi_tuple_type : public extend_tree<tree_type_id::xi_tuple_type, xi_type> {

    inline xi_tuple_type(list<ast_type>* types) noexcept
            : base_type(),
              types(this, types) {
        /* do nothing */
    }

    property<list<ast_type>>                                        types;

};


struct xi_parameter_decl : public extend_tree<tree_type_id::xi_parameter_decl, xi_decl> {

    inline xi_parameter_decl(std::string name, ast_type* type) noexcept
            : base_type(name),
              type(this, type) {
        /* do nothing */
    }

    property<ast_type>                                              type;

};


struct xi_function_decl : public extend_tree<tree_type_id::xi_function_decl, xi_decl> {

    inline xi_function_decl(
            std::string name,
            ast_type* return_type,
            list<xi_parameter_decl>* parameters,
            ast_stmt* body) noexcept
                    : base_type(name),
                      return_type(this, return_type),
                      parameters(this, parameters),
                      body(this, body) {
        /* do nothing */
    }

    property<ast_type>                                              return_type;
    property<list<xi_parameter_decl>>                               parameters;
    property<ast_stmt>                                              body;

};


struct xi_member_decl : public extend_tree<tree_type_id::xi_member_decl, xi_decl> {

    inline xi_member_decl(tree_type_id id, std::string name, ast_type* parent) noexcept
            : base_type(id, name),
              parent(this, parent) {
        /* do nothing */
    }

    property<ast_type>                                              parent;

};


struct xi_field_decl : public extend_tree<tree_type_id::xi_field_decl, xi_member_decl> {

    inline xi_field_decl(std::string name, ast_type* parent, ast_type* type) noexcept
            : base_type(name, parent),
              type(this, type) {
        /* do nothing */
    }

    property<ast_type>                                              type;

};


struct xi_method_decl : public extend_tree<tree_type_id::xi_method_decl, xi_member_decl> {

    inline xi_method_decl(
            std::string name,
            ast_type* parent,
            ast_type* return_type,
            list<xi_parameter_decl>* parameters,
            ast_stmt* body) noexcept
                    : base_type(name, parent),
                      return_type(this, return_type),
                      parameters(this, parameters),
                      body(this, body) {
        /* do nothing */
    }

    property<ast_type>                                              return_type;
    property<list<xi_parameter_decl>>                               parameters;
    property<ast_stmt>                                              body;

};


struct xi_type_decl : public extend_tree<tree_type_id::xi_type_decl, xi_member_decl> {

    inline xi_type_decl(tree_type_id id, std::string name, ast_type* parent) noexcept
            : base_type(id, name, parent) {
        /* do nothing */
    }

};


struct xi_struct_decl : public extend_tree<tree_type_id::xi_struct_decl, xi_type_decl> {

    inline xi_struct_decl(
            tree_type_id id,
            std::string name,
            ast_type* parent,
            list<xi_member_decl>* members) noexcept
                    : base_type(id, name, parent),
                      members(this, members) {
        /* do nothing */
    }

    inline xi_struct_decl(
            std::string name,
            ast_type* parent,
            list<xi_member_decl>* members) noexcept
                    : base_type(name, parent),
                      members(this, members) {
        /* do nothing */
    }

    property<list<xi_member_decl>>                                  members;

};


struct xi_class_decl : public extend_tree<tree_type_id::xi_class_decl, xi_struct_decl> {

    inline xi_class_decl(
            std::string name,
            ast_type* parent,
            list<xi_member_decl>* members) noexcept
                    : base_type(name, parent, members) {
        /* do nothing */
    }

};


struct xi_const_expr : public extend_tree<tree_type_id::xi_const_expr, xi_expr> {

    inline xi_const_expr(ast_type* type) noexcept
            : base_type(type) {
        /* do nothing */
    }

};


struct xi_op_expr : public extend_tree<tree_type_id::xi_op_expr, xi_expr> {

    enum class xi_operator_flag : uint32_t {
        unary                   = 1 <<   8,
        binary                  = 1 <<   9,
        logical                 = 1 <<  10,
        numeric                 = 1 <<  11,
        bitwise                 = 1 <<  12,
        comparative             = 1 <<  13,
        short_circiut           = 1 <<  14,
        reverse                 = 1 <<  15,
        assign                  = 1 <<  16,
        special                 = 1 <<  17,
    };

#define OP_UNARY                (uint32_t) xi_operator_flag::unary
#define OP_BINARY               (uint32_t) xi_operator_flag::binary
#define OP_LOGICAL              (uint32_t) xi_operator_flag::logical
#define OP_NUMERIC              (uint32_t) xi_operator_flag::numeric
#define OP_BITWISE              (uint32_t) xi_operator_flag::bitwise
#define OP_COMP                 (uint32_t) xi_operator_flag::comparative
#define OP_SHORT_CIRC           (uint32_t) xi_operator_flag::short_circiut
#define OP_REVERSE              (uint32_t) xi_operator_flag::reverse
#define OP_ASSIGN               (uint32_t) xi_operator_flag::assign
#define OP_SPECIAL              (uint32_t) xi_operator_flag::special

    enum class xi_operator : uint32_t {

        __none__                =       0,

        __and__                 =       1   | OP_BINARY | OP_LOGICAL    | OP_SHORT_CIRC,
        __or__                  =       2   | OP_BINARY | OP_LOGICAL    | OP_SHORT_CIRC,
        __xor__                 =       3   | OP_BINARY | OP_LOGICAL    | OP_SHORT_CIRC,
        __not__                 =       4   | OP_UNARY  | OP_LOGICAL,
        __rand__                =       __and__         | OP_REVERSE,
        __ror__                 =       __or__          | OP_REVERSE,
        __rxor__                =       __xor__         | OP_REVERSE,

        __iand__                =       __and__         | OP_ASSIGN,
        __ior__                 =       __or__          | OP_ASSIGN,
        __ixor__                =       __xor__         | OP_ASSIGN,

        __add__                 =       5   | OP_BINARY | OP_NUMERIC,
        __sub__                 =       6   | OP_BINARY | OP_NUMERIC,
        __mul__                 =       7   | OP_BINARY | OP_NUMERIC,
        __div__                 =       8   | OP_BINARY | OP_NUMERIC,
        __mod__                 =       9   | OP_BINARY | OP_NUMERIC,
        __radd__                =       __add__         | OP_REVERSE,
        __rsub__                =       __sub__         | OP_REVERSE,
        __rmul__                =       __mul__         | OP_REVERSE,
        __rdiv__                =       __div__         | OP_REVERSE,
        __rmod__                =       __mod__         | OP_REVERSE,

        __iadd__                =       __add__         | OP_ASSIGN,
        __isub__                =       __sub__         | OP_ASSIGN,
        __imul__                =       __mul__         | OP_ASSIGN,
        __idiv__                =       __div__         | OP_ASSIGN,
        __imod__                =       __mod__         | OP_ASSIGN,

        __sl__                  =       10  | OP_BINARY | OP_BITWISE,
        __sr__                  =       11  | OP_BINARY | OP_BITWISE,
        __band__                =       12  | OP_BINARY | OP_BITWISE,
        __bor__                 =       13  | OP_BINARY | OP_BITWISE,
        __bxor__                =       14  | OP_BINARY | OP_BITWISE,
        __bnot__                =       15  | OP_UNARY  | OP_BITWISE,
        __rsl__                 =       __sl__          | OP_REVERSE,
        __rsr__                 =       __sr__          | OP_REVERSE,
        __rband__               =       __band__        | OP_REVERSE,
        __rbor__                =       __bor__         | OP_REVERSE,
        __rbxor__               =       __bxor__        | OP_REVERSE,
        __isl__                 =       __sl__          | OP_ASSIGN,
        __isr__                 =       __sr__          | OP_ASSIGN,
        __iband__               =       __band__        | OP_ASSIGN,
        __ibor__                =       __bor__         | OP_ASSIGN,
        __ibxor__               =       __bxor__        | OP_ASSIGN,

        __lt__                  =       16  | OP_BINARY | OP_NUMERIC,
        __le__                  =       17  | OP_BINARY | OP_NUMERIC,
        __gt__                  =       18  | OP_BINARY | OP_NUMERIC,
        __ge__                  =       19  | OP_BINARY | OP_NUMERIC,
        __eq__                  =       20  | OP_BINARY | OP_NUMERIC,
        __ne__                  =       21  | OP_BINARY | OP_NUMERIC,
        __rlt__                 =       __lt__          | OP_REVERSE,
        __rle__                 =       __le__          | OP_REVERSE,
        __rgt__                 =       __gt__          | OP_REVERSE,
        __rge__                 =       __ge__          | OP_REVERSE,
        __req__                 =       __eq__          | OP_REVERSE,
        __rne__                 =       __ne__          | OP_REVERSE,

        __invoke__              =       22  | OP_SPECIAL,
        __index__               =       23  | OP_SPECIAL,
        __ctor__                =       24  | OP_SPECIAL,
        __dtor__                =       25  | OP_SPECIAL,
        __get__                 =       26  | OP_SPECIAL,
        __set__                 =       27  | OP_SPECIAL,
        __alloc__               =       28  | OP_SPECIAL,
        __dealloc__             =       29  | OP_SPECIAL,
        __move__                =       30  | OP_SPECIAL    | OP_ASSIGN,
        __move_const__          =       31  | OP_SPECIAL    | OP_ASSIGN,
        __copy__                =       32  | OP_SPECIAL    | OP_ASSIGN,
        __copy_const__          =       33  | OP_SPECIAL    | OP_ASSIGN,

    };

#undef OP_SPECIAL
#undef OP_ASSIGN
#undef OP_REVERSE
#undef OP_SHORT_CURC
#undef OP_COMP
#undef OP_BITWISE
#undef OP_NUMERIC
#undef OP_LOGICAL
#undef OP_BINARY
#undef OP_UNARY

    inline xi_op_expr(xi_operator op, list<ast_expr>* operands) noexcept
            : base_type(nullptr),
              op(this, op),
              operands(this, operands) {
        /* do nothing */
    }

    property<xi_operator>                                           op;
    property<list<ast_expr>>                                        operands;

};


struct xi_tuple_expr : public extend_tree<tree_type_id::xi_tuple_expr, xi_expr> {

    inline xi_tuple_expr(list<ast_expr>* expressions) noexcept
            : base_type(nullptr),
              expressions(this, expressions) {
        /* do nothing */
    }

    property<list<ast_expr>>                                        expressions;

};


struct xi_member_expr : public extend_tree<tree_type_id::xi_member_expr, xi_expr> {

    inline xi_member_expr(ast_type* owner_type, ast_expr* expression, xi_member_decl* member) noexcept
            : base_type(nullptr),
              owner_type(this, owner_type),
              expression(this, expression),
              member(this, member) {
        /* do nothing */
    }

    property<ast_type>                                              owner_type;
    property<ast_expr>                                              expression;
    property<xi_member_decl>                                        member;

};

struct xi_type_comparer final : public ast_type_comparer {
public:

    inline xi_type_comparer() = default;
    virtual ~xi_type_comparer() = default;

    virtual bool operator()(ast_type* const&, ast_type* const&) const;

};

struct xi_type_hasher final : public ast_type_hasher {
public:

    inline xi_type_hasher() = default;
    virtual ~xi_type_hasher() = default;

    virtual size_t operator()(ast_type* const&) const;

};

}
#endif
