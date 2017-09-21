/*
 * ast.hpp
 *
 *  Created on: Jan 20, 2017
 *      Author: derick
 */

#ifndef AST_HPP_
#define AST_HPP_

#include "tree.hpp"
#include "source.hpp"

#include <iostream>
#include <sstream>

#include <llvm/ADT/APSInt.h>
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/StringRef.h>

namespace xcc {


/**
 * Base class for all ast nodes
 */
struct ast_tree : public extend_tree<tree_type_id::ast_tree> {
public:

    /**
     * Passing constructor for ast_tree
     * \param id overriding tree type id
     */
    inline ast_tree(tree_type_id id) noexcept
            : base_type(id),
              source_location(this) {
        //...
    }

    property<source_span>                           source_location;

};


/**
 * Base class for all type ast nodes
 */
struct ast_type : public extend_tree<tree_type_id::ast_type, ast_tree> {
public:

    /**
     * Passing constructor
     * \param id overriding tree type id
     */
    inline ast_type(tree_type_id id) noexcept : base_type(id) {
        //...
    }

};


/**
 * Base class for all declaration ast nodes
 */
struct ast_decl : public extend_tree<tree_type_id::ast_decl, ast_tree> {
public:

    /**
     * Passing constructor
     * \param id overriding tree type id
     * \param name declaration name
     */
    inline ast_decl(tree_type_id id, std::string name) noexcept
            : base_type(id),
              name(this, name),
              generated_name(this, "") {
        //...
    }

    property<std::string>                                       name;               //!< declaration name
    property<std::string>                                       generated_name;     //!< mangled declaration name

};


/**
 * Base class for all expression ast nodes
 */
struct ast_expr : public extend_tree<tree_type_id::ast_expr, ast_tree> {
public:

    /**
     * Passing constructor
     * \param id overriding tree type id
     */
    inline ast_expr(tree_type_id id) noexcept
            : base_type(id),
              type(this, nullptr) {
        //...
    }

    /**
     * Passing constructor
     * \param id overriding tree type id
     * \param type expression type
     */
    inline ast_expr(tree_type_id id, ast_type* type)
            : base_type(id),
              type(this, type) {
        //...
    }

    property<ast_type>                                          type;   //!< Expression type

};


/**
 * Base class for all statement ast nodes
 */
struct ast_stmt : public extend_tree<tree_type_id::ast_stmt, ast_tree> {
public:

    /**
     * Passing constructor
     * \param id overriding type id
     */
    inline ast_stmt(tree_type_id id) noexcept : base_type(id) {
        //...
    }

};


/**
 * A Namespace
 */
struct ast_namespace_decl final : public extend_tree<tree_type_id::ast_namespace_decl, ast_decl> {
public:

    inline ast_namespace_decl(std::string name)
            : base_type(name),
              declarations(this, new list<ast_decl>()) {
        //...
    }

    inline ast_namespace_decl(std::string name, list<ast_decl>* declarations)
            : base_type(name),
              declarations(this, declarations) {
        //...
    }

    property<list<ast_decl>>                                    declarations;   //!<

};


/**
 * Global variable declaration
 */
struct ast_variable_decl final : public extend_tree<tree_type_id::ast_variable_decl, ast_decl> {
public:

    /**
     * \param name
     * \param type
     * \param initial_value
     */
    inline ast_variable_decl(std::string name, ast_type* type, ast_expr* initial_value = nullptr) noexcept
            : base_type(name),
              type(this, type),
              initial_value(this, initial_value),
              is_extern(this, false),
              is_extern_visible(this, true) {
        //...
    }

    property<ast_type>                                          type;               //!< variable type
    property<ast_expr>                                          initial_value;      //!< initial value
    property<bool>                                              is_extern;          //!< is defined externally
    property<bool>                                              is_extern_visible;  //!< is visible outside of this module

};


/**
 * Function parameter
 */
struct ast_parameter_decl final : public extend_tree<tree_type_id::ast_parameter_decl, ast_decl> {
public:

    /**
     * \param name
     * \param type
     */
    inline ast_parameter_decl(std::string name, ast_type* type) noexcept
            : base_type(name),
              type(this, type) {
        //...
    }

    property<ast_type>                                          type; //!<

};


/**
 * A local variable within a function
 */
struct ast_local_decl final : public extend_tree<tree_type_id::ast_local_decl, ast_decl> {
public:

    /**
     * \param name
     * \param type
     * \param init_value
     */
    inline ast_local_decl(std::string name, ast_type* type, ast_expr* init_value) noexcept
            : base_type(name),
              type(this, type),
              init_value(this, init_value) {
        //...
    }

    /**
     * \param type
     * \param init_value
     */
    inline ast_local_decl(ast_type* type, ast_expr* init_value) noexcept
            : base_type(std::string("anonymous")),
              type(this, type),
              init_value(this, init_value) {
        //...
    }

    property<ast_type>                                          type;           //!<
    property<ast_expr>                                          init_value;     //!<

};


/**
 * Function declaration
 */
struct ast_function_decl final : public extend_tree<tree_type_id::ast_function_decl, ast_decl> {
public:

    /**
     * \param name function name
     * \param return_type the function return type
     * \param parameters list of parameter declartions
     * \param body the fnction body
     */
    inline ast_function_decl(std::string name, ast_type* return_type, list<ast_parameter_decl>* parameters, ast_stmt* body) noexcept
            : base_type(name),
              return_type(this, return_type),
              parameters(this, parameters),
              body(this, body),
              is_extern(this, false),
              is_extern_visible(this, true),
              is_c_extern(this, false) {
        //...
    }

    property<ast_type>                                          return_type;        //!<
    property<list<ast_parameter_decl>>                          parameters;         //!<
    property<ast_stmt>                                          body;               //!<
    property<bool>                                              is_extern;          //!< is defined externally
    property<bool>                                              is_extern_visible;  //!< is visible outside of this module
    property<bool>                                              is_c_extern;        //!< use c style mangling (none)

};


/**
 * Named type declaration
 */
struct ast_typedef_decl final : public extend_tree<tree_type_id::ast_typedef_decl, ast_decl> {
public:

    /**
     * \param name
     * \param type
     */
    inline ast_typedef_decl(std::string name, ast_type* type)
            : base_type(name),
              type(this, type) {
        //...
    }

    property<ast_type>                                          type;

};


/**
 * The void type
 */
struct ast_void_type final : public extend_tree<tree_type_id::ast_void_type, ast_type> {
public:

    /**
     *
     */
    inline ast_void_type() noexcept : base_type() {
        //...
    }

};


/**
 * An integer type
 */
struct ast_integer_type final : public extend_tree<tree_type_id::ast_integer_type, ast_type> {

    /**
     * \param bitwidth
     * \param is_unsigned
     */
    inline ast_integer_type(uint32_t bitwidth, bool is_unsigned) noexcept
            : base_type(),
              bitwidth(this, bitwidth),
              is_unsigned(this, is_unsigned) {
        //...
    }

    property<uint32_t>                                          bitwidth;       //!<
    property<bool>                                              is_unsigned;    //!<

};


/**
 * A floating point type
 */
struct ast_real_type final : public extend_tree<tree_type_id::ast_real_type, ast_type> {
public:

    /**
     * \param bitwidth
     */
    inline ast_real_type(uint32_t bitwidth) noexcept
            : base_type(),
              bitwidth(this, bitwidth) {
        //...
    }

    property<uint32_t>                                          bitwidth;   //!<

};


/**
 * An array type
 */
struct ast_array_type final : public extend_tree<tree_type_id::ast_array_type, ast_type> {
public:

    /**
     * \param element_type
     * \param size
     */
    inline ast_array_type(ast_type* element_type, uint32_t size) noexcept
            : base_type(),
              element_type(this, element_type),
              size(this, size) {
        //...
    }

    property<ast_type>                                          element_type;   //!<
    property<uint32_t>                                          size;           //!<

};


/**
 * A pointer type
 */
struct ast_pointer_type final : public extend_tree<tree_type_id::ast_pointer_type, ast_type> {
public:

    /**
     * \param element_type
     */
    inline ast_pointer_type(ast_type* element_type) noexcept
            : base_type(),
              element_type(this, element_type) {
        //...
    }

    property<ast_type>                                          element_type;   //!<

};


/**
 * A function type
 */
struct ast_function_type final : public extend_tree<tree_type_id::ast_function_type, ast_type> {
public:

    /**
     * \param return_type
     * \param parameter_types
     */
    inline ast_function_type(ast_type* return_type, list<ast_type>* parameter_types) noexcept
            : base_type(),
              return_type(this, return_type),
              parameter_types(this, parameter_types) {
        //...
    }

    property<ast_type>                                          return_type;        //!<
    property<list<ast_type>>                                    parameter_types;    //!<

};


/**
 * A record type
 */
struct ast_record_type final : public extend_tree<tree_type_id::ast_record_type, ast_type> {
public:

    /**
     * \param is_packed is this type tightly packed ?
     */
    inline ast_record_type(bool is_packed = false) noexcept
            : base_type(),
              field_types(this, new list<ast_type>()),
              is_packed(this, is_packed){
        //...
    }

    /**
     * \param types
     * \param is_packed
     */
    inline ast_record_type(list<ast_type>* types, bool is_packed = false) noexcept
            : base_type(),
              field_types(this, types),
              is_packed(this, is_packed) {
        //...
    }

    property<list<ast_type>>                                    field_types;    //!<
    property<bool>                                              is_packed;      //!<

};


/**
 * A constant integer value
 */
struct ast_integer final : public extend_tree<tree_type_id::ast_integer, ast_expr> {
public:

    /**
     * \param itype
     * \param value
     */
    inline ast_integer(ast_type* itype, llvm::APSInt value) noexcept
            : base_type(itype),
              value(this, value) {
        //...
    }

    property<llvm::APSInt>                                      value;  //!<

};


/**
 * A constant floating point value
 */
struct ast_real final : public extend_tree<tree_type_id::ast_real, ast_expr> {
public:

    /**
     * \param ftype
     * \param value
     */
    inline ast_real(ast_type* ftype, llvm::APFloat value)
            : base_type(ftype),
              value(this, value) {
        //...
    }

    property<llvm::APFloat>                                     value;  //!<

};


/**
 * A constant string value
 */
struct ast_string final : public extend_tree<tree_type_id::ast_string, ast_expr> {
public:

    /**
     * \param stype
     * \param value
     */
    inline ast_string(ast_type* stype, std::string value)
            : base_type(stype),
              value(this, value) {
        //...
    }

    property<std::string>                                       value; //!<

};


/**
 * An array value
 */
struct ast_array final : public extend_tree<tree_type_id::ast_array, ast_expr> {
public:

    /**
     * \param atype    the array type
     * \param values   a list of values
     */
    inline ast_array(ast_type* atype, list<ast_expr>* values)
            : base_type(atype),
              values(this, values) {
        //...
    }

    property<list<ast_expr>>                                    values; //!<

};


/**
 * A record value
 */
struct ast_record final : public extend_tree<tree_type_id::ast_record, ast_expr> {
public:

    /**
     * \param rtype     the record type
     * \param values    a list of values
     */
    inline ast_record(ast_type* rtype, list<ast_expr>* values)
            : base_type(rtype),
              values(this, values) {
        //...
    }

    property<list<ast_expr>>                                    values; //!<

};


/**
 * All binary and unary operators
 */
enum class ast_op : uint32_t {
    none,
    // Low level operators
    bitcast,                                                            //!< Bitcast, (only changes type)
    trunc,                                                              //!< Convert to a smaller integer
    zext,                                                               //!< Zero extend
    sext,                                                               //!< Signed extend
    ftrunc,                                                             //!< Convert to a smaller float
    fext,                                                               //!< Convert to a larger float
    utof,                                                               //!< Unsigned integer to float
    itof,                                                               //!< Signed integer to float
    ftou,                                                               //!< Float to unsigned integer
    ftoi,                                                               //!< Float to signed integer
    ptou,                                                               //!< Pointer to unsigned integer
    utop,                                                               //!< Unsigned integer to pointer
    ineg,                                                               //!< Signed integer negation
    fneg,                                                               //!< Floating point negation
    iadd,                                                               //!< Integer add
    fadd,                                                               //!< Floating point add
    isub,                                                               //!< Integer subtract
    fsub,                                                               //!< Floating point subtract
    imul,                                                               //!< Integer multiply
    fmul,                                                               //!< Floating point multiply
    udiv,                                                               //!< Unsigned integer divide
    idiv,                                                               //!< Signed integer divide
    fdiv,                                                               //!< Floating point division
    umod,                                                               //!< Unsigned integer modulo
    imod,                                                               //!< Signed integer modulo
    fmod,                                                               //!< Floating point modulo
    land,                                                               //!< Logical and
    lor,                                                                //!< Logical or
    lnot,                                                               //!< Logical not
    band,                                                               //!< Binary and
    bor,                                                                //!< Binary or
    bxor,                                                               //!< Binary xor
    bnot,                                                               //!< Binary not
    bshl,                                                               //!< Binary shift left
    bshr,                                                               //!< Binary shift right
    ashr,                                                               //!< Binary shift right (arithmetic)
    cmp_eq,                                                             //!< Integer equal
    cmp_ne,                                                             //!< Integer not equal
    icmp_ult,                                                           //!< Unsigned integer less than
    icmp_ule,                                                           //!< Unsigned integer less than or equal
    icmp_ugt,                                                           //!< Unsigned integer greater than
    icmp_uge,                                                           //!< Unsigned integer greater than or equal
    icmp_slt,                                                           //!< Signed integer less than
    icmp_sle,                                                           //!< Signed integer less than or equal
    icmp_sgt,                                                           //!< Signed integer greater than
    icmp_sge,                                                           //!< Signed integer greater than or equal
    fcmp_oeq,                                                           //!< Floating point ordered equal
    fcmp_one,                                                           //!< Floating point ordered not equal
    fcmp_olt,                                                           //!< Floating point ordered less than
    fcmp_ole,                                                           //!< Floating point ordered less than or equal
    fcmp_ogt,                                                           //!< Floating point ordered greater than
    fcmp_oge,                                                           //!< Floating point ordered greater than or equal
    fcmp_ueq,                                                           //!< Floating point unordered equal
    fcmp_une,                                                           //!< Floating point unordered not equal
    fcmp_ult,                                                           //!< Floating point unordered less than
    fcmp_ule,                                                           //!< Floating point unordered less than or equal
    fcmp_ugt,                                                           //!< Floating point unordered greater than
    fcmp_uge,                                                           //!< Floating point unordered greater than or equal
    fcmp_true,                                                          //!< Floating point always true
    fcmp_false,                                                         //!< Floating point always false
    // High level operators
    __high_level,
    add,                                                                //!< General add
    sub,                                                                //!< General subtract
    mul,                                                                //!< General multiply
    div,                                                                //!< General divide
    mod,                                                                //!< General modulo
    neg,                                                                //!< General negate
    shl,                                                                //!< General shift left
    shr,                                                                //!< General shift right
    eq,                                                                 //!< General equal
    ne,                                                                 //!< General not equal
    le,                                                                 //!< General less than or equal
    lt,                                                                 //!< General less than
    ge,                                                                 //!< General greater than or equal
    gt,                                                                 //!< General greater
    logical_and,                                                        //!< General logical and
    logical_or,                                                         //!< General logical or
    logical_not,                                                        //!< General logical not
    binary_and,                                                         //!< General binary and
    binary_or,                                                          //!< General binary or
    binary_xor,                                                         //!< General binary xor
    binary_not,                                                         //!< General binary not
};

inline bool is_highlevel_op(ast_op op) {
    return ((uint32_t) op) > ((uint32_t) ast_op::__high_level);
}


/**
 * A simple cast operation
 */
struct ast_cast final : public extend_tree<tree_type_id::ast_cast, ast_expr> {
public:

    /**
     * \param type Type to cast to
     * \param expr
     */
    inline ast_cast(ast_type* type, ast_op op, ast_expr* expr)
            : base_type(type),
              op(this, op),
              expr(this, expr) {
        //...
    }

    property<ast_op>                                            op;     //!< The low level casting operator
    property<ast_expr>                                          expr;   //!<

};


/**
 * A binary operator expression
 */
struct ast_binary_op final : public extend_tree<tree_type_id::ast_binary_op, ast_expr> {
public:

    /**
     * \param type
     * \param op
     * \param lhs
     * \param rhs
     */
    inline ast_binary_op(ast_type* type, ast_op op, ast_expr* lhs, ast_expr* rhs) noexcept
            : base_type(type),
              op(this, op),
              lhs(this, lhs),
              rhs(this, rhs) {
        //...
    }

    property<ast_op>                                            op;     //!<
    property<ast_expr>                                          lhs;    //!<
    property<ast_expr>                                          rhs;    //!<

};


/**
 * A unary operator expression
 */
struct ast_unary_op final : public extend_tree<tree_type_id::ast_unary_op, ast_expr> {
public:

    /**
     * \param type
     * \param op
     * \param expr
     */
    inline ast_unary_op(ast_type* type, ast_op op, ast_expr* expr) noexcept
            : base_type(type),
              op(this, op),
              expr(this, expr) {
        //...
    }

    property<ast_op>                                            op;     //!<
    property<ast_expr>                                          expr;   //!<

};


/**
 * An array index expression
 */
struct ast_index final : public extend_tree<tree_type_id::ast_index, ast_expr> {
public:

    /**
     * \param type
     * \param expr
     * \param index
     */
    inline ast_index(ast_type* type, ast_expr* expr, ast_expr* index)
            : base_type(type),
              arr_expr(this, expr),
              index_expr(this, index) {
        //...
    }

    property<ast_expr>                                          arr_expr;   //!<
    property<ast_expr>                                          index_expr; //!<

};


/**
 * A reference to a parameter, local, or global variable
 */
struct ast_declref final : public extend_tree<tree_type_id::ast_declref, ast_expr> {
public:

    /**
     * \param type
     * \param variable
     */
    inline ast_declref(ast_type* type, ast_decl* declaration) noexcept
            : base_type(type),
              declaration(this, declaration){
        //...
    }

    property<ast_decl>                                          declaration;   //!<
};


/*
 * A reference to a record member
 */
struct ast_memberref final : public extend_tree<tree_type_id::ast_memberref, ast_expr> {
public:

    inline ast_memberref(ast_type* type, ast_expr* objexpr, uint32_t member) noexcept
            : base_type(type),
              objexpr(this, objexpr),
              member_index(this, member) {
        //...
    }

    property<ast_expr>                                          objexpr;        //!<
    property<uint32_t>                                          member_index;   //!<

};



/**
 * A pointer dereference expression
 */
struct ast_deref final : public extend_tree<tree_type_id::ast_deref, ast_expr> {
public:

    /**
     * \param type
     * \param expr
     */
    inline ast_deref(ast_type* type, ast_expr* expr) noexcept
            : base_type(type),
              expr(this, expr) {
        //...
    }

    property<ast_expr>                                          expr;   //!<

};


/**
 * An address of expression
 */
struct ast_addressof final : public extend_tree<tree_type_id::ast_addressof, ast_expr> {
public:

    /**
     * \param type
     * \param expr
     */
    inline ast_addressof(ast_type* type, ast_expr* expr) noexcept
            : base_type(type),
              expr(this, expr) {
        //...
    }

    property<ast_expr>                                          expr;   //!<

};


/**
 * A function call by address expression
 */
struct ast_invoke final : public extend_tree<tree_type_id::ast_invoke, ast_expr> {
public:

    /**
     * \param type
     * \param funcexpr
     * \param arguments
     */
    inline ast_invoke(ast_type* type, ast_expr* funcexpr, list<ast_expr>* arguments) noexcept
            : base_type(type),
              funcexpr(this, funcexpr),
              arguments(this, arguments) {
        //...
    }

    property<ast_expr>                                          funcexpr;   //!<
    property<list<ast_expr>>                                    arguments;  //!<

};


/**
 * A function call by name expression
 */
struct ast_call final : public extend_tree<tree_type_id::ast_call, ast_expr> {
public:

    /**
     * \param type
     * \param funcdecl
     * \param arguments
     */
    inline ast_call(ast_type* type, ast_decl* funcdecl, list<ast_expr>* arguments) noexcept
            : base_type(type),
              funcdecl(this, funcdecl),
              arguments(this, arguments) {
        //...
    }

    property<ast_decl>                                          funcdecl;   //!<
    property<list<ast_expr>>                                    arguments;  //!<

};


/**
 * A list of stmts evaluated for side effects, and then an expression
 */
struct ast_stmt_expr final : public extend_tree<tree_type_id::ast_stmt_expr, ast_expr> {
public:

    /**
     * \param type
     * \param statements
     * \param expr
     */
    inline ast_stmt_expr(ast_type* type, list<ast_stmt>* statements, ast_expr* expr) noexcept
            : base_type(type),
              statements(this, statements),
              expr(this, expr) {
        //...
    }

    property<list<ast_stmt>>                                    statements; //!<
    property<ast_expr>                                          expr;       //!<

};


/**
 * A no operation stmt (does nothing)
 */
struct ast_nop_stmt final : public extend_tree<tree_type_id::ast_nop_stmt, ast_stmt> {
public:

    /**
     *
     */
    inline ast_nop_stmt() noexcept : base_type() { }
};


/**
 * An expression statement
 */
struct ast_expr_stmt final : public extend_tree<tree_type_id::ast_expr_stmt, ast_stmt> {
public:

    /**
     * \param expr
     */
    inline ast_expr_stmt(ast_expr* expr) noexcept
            : base_type(),
              expr(this, expr) {
        //...
    }

    property<ast_expr>                                          expr;   //!<

};


/**
 * Variable assignment statement
 */
struct ast_assign_stmt final : public extend_tree<tree_type_id::ast_assign_stmt, ast_stmt> {
public:

    /**
     * \param lhs
     * \param rhs
     */
    inline ast_assign_stmt(ast_expr* lhs, ast_expr* rhs) noexcept
            : base_type(),
              lhs(this, lhs),
              rhs(this, rhs) {
        //...
    }

    property<ast_expr>                                          lhs;    //!<
    property<ast_expr>                                          rhs;    //!<

};


/**
 * Declaration of a local variable
 */
struct ast_decl_stmt final : public extend_tree<tree_type_id::ast_decl_stmt, ast_stmt> {
public:

    inline ast_decl_stmt(ast_local_decl* decl) noexcept
            : base_type(),
              decl(this, decl) {
        //...
    }

    property<ast_local_decl>                                    decl;   //!<
};


/**
 * A block statement with local variable declarations that are initialized
 *  at the begining of the block, and destroyed at the end
 */
struct ast_block_stmt final : public extend_tree<tree_type_id::ast_block_stmt, ast_stmt> {
public:

    /**
     * \param decls list of declarations
     * \param stmts a sequence of statements to be executed in order
     */
    inline ast_block_stmt(list<ast_local_decl>* locals, list<ast_stmt>* stmts) noexcept
            : base_type(),
              decls(this, locals),
              stmts(this, stmts) {
        //...
    }

    /**
     *
     */
    inline ast_block_stmt() noexcept
            : base_type(),
              decls(this, new list<ast_local_decl>()),
              stmts(this, new list<ast_stmt>()) {
        //...
    }

    property<list<ast_local_decl>>                              decls;  //!<
    property<list<ast_stmt>>                                    stmts;  //!<

};


/**
 * An if statement
 */
struct ast_if_stmt final : public extend_tree<tree_type_id::ast_if_stmt, ast_stmt> {
public:

    /**
     * \param condition
     * \param true_stmt     stmt to execute if the condition is true
     * \param false_stmt    stmt to execute if the condition is false
     */
    inline ast_if_stmt(ast_expr* condition, ast_stmt* true_stmt, ast_stmt* false_stmt) noexcept
            : base_type(),
              condition(this, condition),
              true_stmt(this, true_stmt),
              false_stmt(this, false_stmt) {
        //...
    }

    property<ast_expr>                                          condition;  //!<
    property<ast_stmt>                                          true_stmt;  //!<
    property<ast_stmt>                                          false_stmt; //!<

};


/**
 * A while statement
 */
struct ast_while_stmt final : public extend_tree<tree_type_id::ast_while_stmt, ast_stmt> {
public:

    /**
     * \param condition
     * \param stmt
     */
    inline ast_while_stmt(ast_expr* condition, ast_stmt* stmt) noexcept
            : base_type(),
              condition(this, condition),
              stmt(this, stmt) {
        //...
    }

    property<ast_expr>                                          condition;  //!<
    property<ast_stmt>                                          stmt;       //!<

};


/**
 * A for statement
 */
struct ast_for_stmt final : public extend_tree<tree_type_id::ast_for_stmt, ast_stmt> {
public:

    /**
     * \param init_stmt
     * \param condition
     * \param each_stmt
     */
    inline ast_for_stmt(ast_stmt* init_stmt, ast_expr* condition, ast_stmt* each_stmt, ast_stmt* body) noexcept
            : base_type(),
              init_stmt(this, init_stmt),
              condition(this, condition),
              each_stmt(this, each_stmt),
              body(this, body) {
        //...
    }

    property<ast_stmt>                                          init_stmt;  //!<
    property<ast_expr>                                          condition;  //!<
    property<ast_stmt>                                          each_stmt;  //!<
    property<ast_stmt>                                          body;

};


/**
 * A return statement
 */
struct ast_return_stmt final : public extend_tree<tree_type_id::ast_return_stmt, ast_stmt> {
public:

    /**
     * \param expr null if returning nothing
     */
    inline ast_return_stmt(ast_expr* expr) noexcept
            : base_type(),
              expr(this, expr) {
        //...
    }

    property<ast_expr>                                          expr;   //!<

};


/**
 * A break statement
 */
struct ast_break_stmt final : public extend_tree<tree_type_id::ast_break_stmt, ast_stmt> {
public:

    /**
     *
     */
    inline ast_break_stmt() noexcept { }

};


/**
 * A continue statement
 */
struct ast_continue_stmt final : public extend_tree<tree_type_id::ast_continue_stmt, ast_stmt> {
public:

    inline ast_continue_stmt() noexcept { }

};

const char* to_string(xcc::ast_op op);

}

namespace std {
inline const char* to_string(xcc::ast_op op) { return xcc::to_string(op); }
}

namespace xcc {
struct ast_printer final : public dispatch_visitor<void, std::ostream&> {
public:

    static ast_printer instance;

    template<typename T>
    static inline void add(ast_printer::dispatch_function_type<T> func) {
        instance.addfunction(func);
    }

    ast_printer();

    void handle_null_tree(std::ostream&) override final;

protected:

    typedef std::function<void(const char*, std::ostream&)>             pwfunc_t;

    template<typename T>
    inline pwfunc_t pwrap(__tree_property_tree<T>& p) {
        return [&](const char*, std::ostream& s) -> void { this->visit(p, s); };
    }

    template<typename T>
    inline pwfunc_t pwrap(__tree_property_list<T>& p) {
        return [&](const char* fmt, std::ostream& s) -> void {
            for(uint32_t i = 0; i < p->size(); i++) {
                this->visit((*p)[i], s);
                if(i < (p->size()-1)) {
                    this->print(s, fmt);
                }
            }
        };
    }

    inline pwfunc_t pwrap(__tree_property_value<std::string>& p) {
        return [&](const char*, std::ostream& s) -> void { s << (std::string) p; };
    }

    template<typename T>
    inline pwfunc_t pwrap(__tree_property_value<T>& p) {
        return [&](const char*, std::ostream& s) -> void { s << std::to_string((T)p); };
    }

    inline pwfunc_t pwrap(__tree_property_value<llvm::APSInt>& p) {
        return [&](const char*, std::ostream& s) -> void { s << p->toString(10); };
    }

    inline pwfunc_t pwrap(__tree_property_value<llvm::APFloat>& p) {
        return [&](const char*, std::ostream& s) -> void { s << std::to_string(p->convertToDouble()); };
    }

    inline pwfunc_t pwrap(__tree_property_value<bool>& p) {
        return [&](const char*, std::ostream& s) -> void {
            if((bool) p) s << "false";
            else         s << "true";
        };
    }

    inline pwfunc_t pwrap(const char* svalue) {
        return [&](const char*, std::ostream& s) -> void {
            s << svalue;
        };
    }

    inline pwfunc_t pwrap(std::string& str) {
        return [&](const char*, std::ostream& s) -> void {
            s << str;
        };
    }

public:

    template<typename... TTreeTypes>
    static inline void print(std::ostream& s, const char* fmt, TTreeTypes&... props) {
        std::vector<pwfunc_t> vec = { instance.pwrap(props)... };
        instance.formatted_print(fmt, s, vec);
    }

    template<typename TTreeType>
    static inline void print(ptr<TTreeType> t, std::ostream& s) {
        instance.print_internal(dynamic_cast<__tree_base*>(unbox(t)), s);
    }

protected:

    inline void print_internal(__tree_base* t, std::ostream& s) {
        this->_current_indent = 0;
        this->visit(t, s);
    }

    static std::string print_to_string(ast_tree* t);

private:

    uint32_t                                                            _current_indent;

    void formatted_print(const char* fmt, std::ostream& s, std::vector<pwfunc_t>& pfunc);

};


template<typename T,
         typename std::enable_if<std::is_base_of<ast_tree, T>::value, int>::type = 0>
inline T* setloc(T* t, source_span& loc) noexcept {
    if(t != nullptr) {
        t->source_location = loc;
    }
    return t;
}

template<typename T,
         typename std::enable_if<std::is_base_of<ast_tree, T>::value, int>::type = 0>
inline T* setloc(T* t, std::string filename, uint32_t line, uint32_t col, uint32_t width) noexcept {
    if(t != nullptr) {
        t->source_location = {
                { filename, line, col },
                { filename, line, col + width }
        };
    }
    return t;
}

template<typename TDestTreeType,
         typename TSrcTreeType,
         typename std::enable_if<std::is_base_of<ast_tree, TDestTreeType>::value, int>::type = 0,
         typename std::enable_if<std::is_base_of<ast_tree, TSrcTreeType>::value, int>::type = 0>
TDestTreeType* copyloc(TDestTreeType* t, TSrcTreeType* ft) noexcept { t->source_location = ft->source_location; return t; }

template<typename TDestTreeType,
         typename TSrcMinTreeType,
         typename TSrcMaxTreeType,
         typename std::enable_if<std::is_base_of<ast_tree, TDestTreeType>::value, int>::type = 0,
         typename std::enable_if<std::is_base_of<ast_tree, TSrcMinTreeType>::value, int>::type = 0,
         typename std::enable_if<std::is_base_of<ast_tree, TSrcMaxTreeType>::value, int>::type = 0>
TDestTreeType* copyloc(TDestTreeType *t, TSrcMinTreeType* fmin, TSrcMaxTreeType* fmax) noexcept {
    t->source_location->first   = fmin->source_location->first;
    t->source_location->last    = fmax->source_location->last;
    return t;
}



typedef std::hash<ast_type*>                                ast_type_hasher;

struct ast_type_comparer {
public:

    ast_type_comparer() = default;
    virtual ~ast_type_comparer() = default;

    virtual bool operator()(ast_type* const&, ast_type* const&) const noexcept;

private:

    bool same_typelist(list<ast_type>* lhs, list<ast_type>* rhs) const noexcept;

};



}




#endif /* AST_HPP_ */
