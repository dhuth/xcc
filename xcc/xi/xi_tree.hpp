#ifndef XI_XI_TREE_HPP_
#define XI_XI_TREE_HPP_

#include <string>
#include "ast.hpp"

namespace xcc {

/**
 * Base type tree for all xi specific types
 */
struct xi_type : public implement_tree<tree_type_id::xi_type> {
public:

    explicit inline xi_type(tree_type_id id) noexcept : base_type(id) {
        /* do nothing */
    }

    explicit inline xi_type(const xi_type& t) noexcept
            : base_type((base_type&) t) {
        /* do nothing */
    }

};


/**
 * Base decl tree for all xi specific declarations
 */
struct xi_decl : public implement_tree<tree_type_id::xi_decl> {
public:

    explicit inline xi_decl(tree_type_id id, std::string name) noexcept : base_type(id, name) {
        /* do nothing */
    }

    explicit inline xi_decl(const xi_decl& d) noexcept
            : base_type((base_type&) d) {
        /* do nothing */
    }

};


/**
 * Base expr tree for all xi specific expressions
 */
struct xi_expr : public implement_tree<tree_type_id::xi_expr> {
public:

    explicit inline xi_expr(tree_type_id id) noexcept : base_type(id) {
        /* do nothing */
    }

    explicit inline xi_expr(tree_type_id id, ast_type* type) noexcept : base_type(id, type) {
        /* do nothing */
    }

    explicit inline xi_expr(const xi_expr& e) noexcept
            : base_type((base_type&) e) {
        /* do nothing */
    }

};


/**
 * Base stmt tree for all xi specific statements
 */
struct xi_stmt : public implement_tree<tree_type_id::xi_stmt> {
public:

    explicit inline xi_stmt(tree_type_id id) noexcept : base_type(id) {
        /* do nothing */
    }

    explicit inline xi_stmt(const xi_stmt& s) noexcept
            : base_type((base_type&) s) {
        /* do nothing */
    }

};


/**
 * QName
 */
struct xi_qname : public implement_tree<tree_type_id::xi_qname> {
public:

    explicit inline xi_qname(std::string name) noexcept
            : base_type(),
              names(this, new list<std::string>(name)) {
        /* do nothing */
    }

    explicit inline xi_qname(xi_qname* p, std::string name) noexcept
            : base_type(),
              names(this, new list<std::string>(p->names, name)) {
        /* do nothing */
    }

    explicit inline xi_qname(const xi_qname& other) noexcept
            : base_type((base_type&) other),
              names(this, other.names) {
        /* do nothing */
    }

    property<list<std::string>>                             names;
};


/**
 * The name of a type
 */
struct xi_id_type : public implement_tree<tree_type_id::xi_id_type> {
public:

    explicit inline xi_id_type(xi_qname* name) noexcept
            : base_type(),
              name(this, name) {
        /* do nothing */
    }

    explicit inline xi_id_type(const xi_id_type& i) noexcept
            : base_type((base_type&) i),
              name(this, i.name) {
        /* do nothing */
    }

    property<xi_qname>                                              name;   //! Type name
};


/**
 * A group candidate of types
 */
struct xi_group_type : public implement_tree<tree_type_id::xi_group_type> {
public:

    explicit inline xi_group_type(list<ast_type>* types) noexcept
            : base_type(),
              types(this, types) {
        /* do nothing */
    }

    explicit inline xi_group_type(const xi_group_type& t) noexcept
            : base_type((base_type&) t),
              types(this, t.types) {
        /* do nothing */
    }

    property<list<ast_type>>                                        types; //! Potential types to resolve to

};


/**
 * A const type
 */
struct xi_const_type : public implement_tree<tree_type_id::xi_const_type> {
public:

    explicit inline xi_const_type(ast_type* type) noexcept
            : base_type(),
              type(this, type) {
        /* do nothing */
    }

    explicit inline xi_const_type(const xi_const_type& c) noexcept
            : base_type((base_type&) c),
              type(this, c.type) {
        /* do nothing */
    }

    property<ast_type>                                              type;

};


/**
 * An inferred type
 */
struct xi_auto_type : public implement_tree<tree_type_id::xi_auto_type> {
public:

    explicit inline xi_auto_type() noexcept
            : base_type() {
        /* do nothing */
    }

    explicit inline xi_auto_type(const xi_auto_type& t) noexcept
            : base_type((base_type&) t) {
        /* do nothing */
    }

};


/**
 * A reference pointer type
 */
struct xi_reference_type : public implement_tree<tree_type_id::xi_reference_type> {
public:

    explicit inline xi_reference_type(ast_type* type) noexcept
            : base_type(),
              type(this, type) {
        /* do nothing */
    }

    explicit inline xi_reference_type(const xi_reference_type& r) noexcept
            : base_type((base_type&) r),
              type(this, r.type) {
        /* do nothing */
    }

    property<ast_type>                                              type;   //! The type being pointed to

};


/**
 * An object type
 */
struct xi_object_type : public implement_tree<tree_type_id::xi_object_type> {
public:

    explicit inline xi_object_type(xi_decl* decl) noexcept
            : base_type(),
              declaration(this, decl) {
        /* do nothing */
    }

    explicit inline xi_object_type(const xi_object_type& t) noexcept
            : base_type((base_type&) t),
              declaration(this, t.declaration) {
        /* do nothing */
    }

    property<xi_decl>                                               declaration;    //! The declaration that defines the type

};


/**
 * A tuple type
 */
struct xi_tuple_type : public implement_tree<tree_type_id::xi_tuple_type> {
public:

    explicit inline xi_tuple_type(list<ast_type>* types) noexcept
            : base_type(),
              types(this, types) {
        /* do nothing */
    }

    explicit inline xi_tuple_type(const xi_tuple_type& t) noexcept
            : base_type((base_type&) t),
              types(this, t.types) {
        /* do nothing */
    }

    property<list<ast_type>>                                        types;

};


/**
 * A using declaration (???)
 */
struct xi_using_decl : public implement_tree<tree_type_id::xi_using_decl> {
public:

    explicit inline xi_using_decl(list<ast_decl>* using_declarations) noexcept
            : base_type("<using>"),
              using_declarations(this, using_declarations) {
        /* do nothing */
    }

    explicit inline xi_using_decl(const xi_using_decl& d) noexcept
            : base_type((base_type&) d),
              using_declarations(this, using_declarations) {
        /* do nothing */
    }

    property<list<ast_decl>>                                        using_declarations; //! Exposed declarations
};


/**
 * A parameter to a function
 */
struct xi_parameter_decl : public implement_tree<tree_type_id::xi_parameter_decl> {
public:

    explicit inline xi_parameter_decl(std::string name, ast_type* type) noexcept
            : base_type(name),
              type(this, type) {
        /* do nothing */
    }

    explicit inline xi_parameter_decl(const xi_parameter_decl& p) noexcept
            : base_type((base_type&) p),
              type(this, p.type) {
        /* do nothing */
    }

    property<ast_type>                                              type;

};


/**
 * A function
 */
struct xi_function_decl : public implement_tree<tree_type_id::xi_function_decl> {
public:

    explicit inline xi_function_decl(
            std::string name,
            ast_type* return_type,
            list<xi_parameter_decl>* parameters,
            ast_stmt* body) noexcept
                    : base_type(name),
                      return_type(this, return_type),
                      parameters(this, parameters),
                      body(this, body),
                      is_extern(this, false),
                      is_inline(this, false) {
        /* do nothing */
    }

    explicit inline xi_function_decl(const xi_function_decl& f) noexcept
            : base_type((base_type&) f),
              return_type(this, f.return_type),
              parameters(this, f.parameters),
              body(this, f.body),
              is_extern(this, false),
              is_inline(this, false) {
        /* do nothing */
    }

    property<bool>                                                  is_extern;      //! is defined somewhere else
    property<bool>                                                  is_inline;      //! the code generator should try to inline this function
    property<ast_type>                                              return_type;    //! the return type of this function
    property<list<xi_parameter_decl>>                               parameters;     //! function parameters
    property<ast_stmt>                                              body;           //! function body

};


/**
 * Base class for type members
 */
struct xi_member_decl : public implement_tree<tree_type_id::xi_member_decl> {
public:

    explicit inline xi_member_decl(tree_type_id id, std::string name, ast_type* parent) noexcept
            : base_type(id, name),
              is_static(this, false),
              parent(this, parent) {
        /* do nothing */
    }

    explicit inline xi_member_decl(const xi_member_decl& m) noexcept
            : base_type((base_type&) m),
              is_static(this, m.is_static),
              parent(this, m.parent) {
        /* do nothing */
    }

    property<bool>                                                  is_static;  //! is this member static
    property<ast_type>                                              parent;     //! the type that this member belongs to

};


/**
 * A simple data member
 */
struct xi_field_decl : public implement_tree<tree_type_id::xi_field_decl> {
public:

    explicit inline xi_field_decl(std::string name, ast_type* parent, ast_type* type) noexcept
            : base_type(name, parent),
              type(this, type) {
        /* do nothing */
    }

    explicit inline xi_field_decl(const xi_field_decl& f) noexcept
            : base_type((base_type&) f),
              type(this, f.type) {
        /* do nothing */
    }

    property<ast_type>                                              type;   //! this datamembers type

};


/**
 * A method
 */
struct xi_method_decl : public implement_tree<tree_type_id::xi_method_decl> {
public:

    //TODO: incomplete
    explicit inline xi_method_decl(std::string name, ast_type* parent) noexcept
            : base_type(name, parent) {
        /* do nothing */
    }

    //TODO: incomplete
    explicit inline xi_method_decl(const xi_method_decl& m) noexcept
            : base_type((base_type&) m) {
        /* do nothing */
    }

    //TODO: fields
};


/**
 * Base class for all type declarations
 */
struct xi_type_decl : public implement_tree<tree_type_id::xi_type_decl> {
public:

    explicit inline xi_type_decl(tree_type_id id, std::string name, ast_type* parent, list<xi_member_decl>* members) noexcept
            : base_type(id, name, parent),
              members(this, members) {
        /* do nothing */
    }

    explicit inline xi_type_decl(const xi_type_decl& t) noexcept
            : base_type((base_type&) t),
              members(this, t.members) {
        /* do nothing */
    }

    property<list<xi_member_decl>>                                  members;    //! this types members

};


/**
 * A struct declarations
 */
struct xi_struct_decl : public implement_tree<tree_type_id::xi_struct_decl> {
public:

    explicit inline xi_struct_decl(std::string name, ast_type* parent, list<xi_member_decl>* members) noexcept
            : base_type(name, parent, members) {
        /* do nothing */
    }

    explicit inline xi_struct_decl(const xi_struct_decl& s) noexcept
            : base_type((base_type&) s) {
        /* do nothing */
    }

};


/**
 * (???)
 */
struct xi_const_expr : public implement_tree<tree_type_id::xi_const_expr> {

    inline xi_const_expr(ast_expr* expr) noexcept
            : base_type(nullptr),
              expr(this, expr) {
        /* do nothing */
    }

    inline xi_const_expr(const xi_const_expr& e) noexcept
            : base_type((base_type&) e),
              expr(this, e.expr){
        /* do nothing */
    }

    property<ast_expr>                                              expr;

};


/**
 * A name that should be resolved during semantic checks
 */
struct xi_id_expr : public implement_tree<tree_type_id::xi_id_expr> {
public:

    explicit inline xi_id_expr(xi_qname* name) noexcept
            : base_type(nullptr),
              name(this, name) {
        /* do nothing */
    }

    explicit inline xi_id_expr(const xi_id_expr& e) noexcept
            : base_type((base_type&) e),
              name(this, e.name) {
        /* do nothing */
    }

    property<xi_qname>                                              name;

};


/**
 * An expression that resolves to a type (???)
 */
struct xi_type_expr : public implement_tree<tree_type_id::xi_type_expr> {
public:

    explicit inline xi_type_expr(ast_type* type) noexcept
            : base_type(type) {
        /* do nothing */
    }

    explicit inline xi_type_expr(const xi_type_expr& e) noexcept
            : base_type((base_type&) e) {
        /* do nothing */
    }

};


/**
 * A member name expression that is resolved during semantic checks
 */
struct xi_member_id_expr : public implement_tree<tree_type_id::xi_member_id_expr> {
public:

    explicit inline xi_member_id_expr(ast_expr* expr, xi_qname* name) noexcept
            : base_type(nullptr),
              expr(this, expr),
              name(this, name) {
        /* do nothing */
    }

    explicit inline xi_member_id_expr(const xi_member_id_expr& m) noexcept
            : base_type((base_type&) m),
              expr(this, m.expr),
              name(this, m.name) {
        /* do nothing */
    }

    property<ast_expr>                                              expr;   //! object expression
    property<xi_qname>                                              name;   //! member name

};


/**
 * A group of candidate expressions (ususally a group of function overload candidates)
 */
struct xi_group_expr : public implement_tree<tree_type_id::xi_group_expr> {
public:

    explicit inline xi_group_expr(list<ast_expr>* expressions) noexcept
            : base_type(nullptr),
              expressions(this, expressions) {
        /* do nothing */
    }

    explicit inline xi_group_expr(ptr<list<ast_expr>> expressions) noexcept
            : base_type(nullptr),
              expressions(this, unbox(expressions)) {
        /* do nothing */
    }

    explicit inline xi_group_expr(const xi_group_expr& d) noexcept
            : base_type((base_type&) d),
              expressions(this, d.expressions) {
        /* do nothing */
    }

    property<list<ast_expr>>                                        expressions;    //! Candidate expressions
};


/**
 * Operators
 */
struct xi_op_expr : public implement_tree<tree_type_id::xi_op_expr> {

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

        __assign__              =       30  | OP_SPECIAL    | OP_ASSIGN,

        __move__                =       31  | OP_SPECIAL    | OP_ASSIGN,
        __move_const__          =       32  | OP_SPECIAL    | OP_ASSIGN,
        __copy__                =       33  | OP_SPECIAL    | OP_ASSIGN,
        __copy_const__          =       34  | OP_SPECIAL    | OP_ASSIGN,

        __deref__               =       35  | OP_SPECIAL    | OP_UNARY,
        __address_of__          =       36  | OP_SPECIAL    | OP_UNARY,

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

    explicit inline xi_op_expr(xi_operator op, list<ast_expr>* operands) noexcept
            : base_type(nullptr),
              op(this, op),
              operands(this, operands) {
        /* do nothing */
    }

    explicit inline xi_op_expr(const xi_op_expr& e) noexcept
            : base_type((base_type&) e),
              op(this, e.op),
              operands(this, e.operands) {
        /* do nothing */
    }

    property<xi_operator>                                           op;
    property<list<ast_expr>>                                        operands;

};


/**
 * A tuple expression
 */
struct xi_tuple_expr : public implement_tree<tree_type_id::xi_tuple_expr> {
public:

    explicit inline xi_tuple_expr(list<ast_expr>* expressions) noexcept
            : base_type(nullptr),
              expressions(this, expressions) {
        /* do nothing */
    }

    explicit inline xi_tuple_expr(const xi_tuple_expr& e) noexcept
            : base_type((base_type&) e),
              expressions(this, e.expressions) {
        /* do nothing */
    }

    property<list<ast_expr>>                                        expressions;

};


/**
 * A member expression
 */
struct xi_member_expr : public implement_tree<tree_type_id::xi_member_expr> {
public:

    explicit inline xi_member_expr(ast_type* owner_type, ast_expr* expression, xi_member_decl* member) noexcept
            : base_type(nullptr),
              owner_type(this, owner_type),
              expression(this, expression),
              member(this, member) {
        /* do nothing */
    }

    explicit inline xi_member_expr(const xi_member_expr& e) noexcept
            : base_type((base_type&) e),
              owner_type(this, e.owner_type),
              expression(this, e.expression),
              member(this, e.member) {
        /* do nothing */
    }

    property<ast_type>                                              owner_type; //! ???
    property<ast_expr>                                              expression; //! the obj expression
    property<xi_member_decl>                                        member;     //! the member

};


/**
 * Type comparer for the xi language
 */
struct xi_type_comparer final : public ast_type_comparer {
public:

    inline xi_type_comparer() = default;
    virtual ~xi_type_comparer() = default;

    virtual bool operator()(ast_type* const&, ast_type* const&) const;

};

/**
 * Type hasher for the xi language
 */
struct xi_type_hasher final : public ast_type_hasher {
public:

    inline xi_type_hasher() = default;
    virtual ~xi_type_hasher() = default;

    virtual size_t operator()(ast_type* const&) const;

};
} // namespace xcc

namespace std {
    std::string to_string(xcc::xi_op_expr::xi_operator op) noexcept;
} // namespace std
#endif
