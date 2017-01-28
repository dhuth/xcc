/*
 * ast.hpp
 *
 *  Created on: Jan 20, 2017
 *      Author: derick
 */

#ifndef AST_HPP_
#define AST_HPP_

#include "tree.hpp"
#include <llvm/ADT/APSInt.h>
#include <llvm/ADT/APFloat.h>

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
    inline ast_tree(tree_type_id id) noexcept : base_type(id) {
        //...
    }

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
              name(this, name) {
        //...
    }

    property<std::string>                                       name; //!< declaration name

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


struct ast_record_decl;


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
     */
    inline ast_local_decl(std::string name, ast_type* type) noexcept
            : base_type(name),
              type(this, type) {
        //...
    }

    property<ast_type>                                          type; //!<
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
              is_extern_visible(this, true) {
        //...
    }

    property<ast_type>                                          return_type;        //!<
    property<list<ast_parameter_decl>>                          parameters;         //!<
    property<ast_stmt>                                          body;               //!<
    property<bool>                                              is_extern;          //!< is defined externally
    property<bool>                                              is_extern_visible;  //!< is visible outside of this module

};


struct ast_record_decl;


/**
 * A member of a record declaration
 */
struct ast_record_member_decl final : public extend_tree<tree_type_id::ast_record_member_decl, ast_decl> {
public:

    /**
     * \param name member name
     * \param type member type
     */
    inline ast_record_member_decl(std::string name, ast_type* type) noexcept
            : base_type(name),
              member_index(this, 0),
              type(this, type),
              parent(this, nullptr) {
        //...
    }

    property<uint32_t>                                          member_index;   //!<
    property<ast_type>                                          type;           //!<
    property<ast_decl>                                          parent;         //!< the record declaration that defines contains this member

};


/**
 * Record declartion
 */
struct ast_record_decl final : public extend_tree<tree_type_id::ast_record_decl, ast_decl> {
public:

    /**
     * \param name
     * \param members
     */
    inline ast_record_decl(std::string name, list<ast_record_member_decl>* members) noexcept
            : base_type(name),
              members(this, members) {
        uint32_t index = 0;
        for(auto m: members) {
            m->parent = this;
            m->member_index = index;
            index++;
        }
    }

    property<list<ast_record_member_decl>>                        members;  //!<

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
     * \param declaration
     */
    inline ast_record_type(ast_record_decl* declaration, bool is_packed = false) noexcept
            : base_type(),
              declaration(this, declaration),
              is_packed(this, is_packed){
        //...
    }

    property<ast_record_decl>                                   declaration;    //!<
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
 * All binary and unary operators
 */
enum class ast_op : uint32_t {
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
    add,                                                                //!< Integer add
    fadd,                                                               //!< Floating point add
    sub,                                                                //!< Integer subtract
    fsub,                                                               //!< Floating point subtract
    mul,                                                                //!< Integer multiply
    fmul,                                                               //!< Floating point multiply
    udiv,                                                               //!< Unsigned integer divide
    idiv,                                                               //!< Signed integer divide
    fdiv,                                                               //!< Floating point division
    umod,                                                               //!< Unsigned integer modulo
    imod,                                                               //!< Signed integer modulo
    fmod,                                                               //!< Floating point modulo
    land,                                                               //!< Logical and
    lor,                                                                //!< Logical or
    lxor,                                                               //!< Logical xor
    lnot,                                                               //!< Logical not
    band,                                                               //!< Binary and
    bor,                                                                //!< Binary or
    bxor,                                                               //!< Binary xor
    bnot,                                                               //!< Binary not
    bshl,                                                               //!< Binary shift left
    bshr,                                                               //!< Binary shift right
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
};


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
 * A function call expression
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
 * A block statement with local variable declarations that are initialized
 *  at the begining of the block, and destroyed at the end
 */
struct ast_block_stmt final : public extend_tree<tree_type_id::ast_block_stmt, ast_stmt> {
public:

    /**
     * \param decls list of declarations
     * \param stmts a sequence of statements to be executed in order
     */
    inline ast_block_stmt(list<ast_variable_decl>* decls, list<ast_stmt>* stmts) noexcept
            : base_type(),
              decls(this, decls),
              stmts(this, stmts) {
        //...
    }

    property<list<ast_variable_decl>>                           decls;  //!<
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
    inline ast_for_stmt(ast_stmt* init_stmt, ast_expr* condition, ast_stmt* each_stmt) noexcept
            : base_type(),
              init_stmt(this, init_stmt),
              condition(this, condition),
              each_stmt(this, each_stmt) {
        //...
    }

    property<ast_stmt>                                          init_stmt;  //!<
    property<ast_expr>                                          condition;  //!<
    property<ast_stmt>                                          each_stmt;  //!<

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
              expr(expr) {
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

}




#endif /* AST_HPP_ */
