/*
 * ast.hpp
 *
 *  Created on: Jan 20, 2017
 *      Author: derick
 */

#ifndef AST_HPP_
#define AST_HPP_

#include "source.hpp"

#include <iostream>
#include <sstream>
#include <unordered_map>

#include <llvm/ADT/APSInt.h>
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/StringRef.h>
#include "tree.hpp"

//#include "llvm_metadata.hpp"

namespace xcc {

/**
 * Properties for locatable ast elements (decls, exprs, & stmts)
 */
struct ast_locatable {

    explicit inline ast_locatable(tree_t* t) noexcept
            : source_location(t) {
        /* do nothing */
    }

    explicit inline ast_locatable(tree_t* t, source_span source_location) noexcept
            : source_location(t, source_location) {
        /* do nothing */
    }

    explicit inline ast_locatable(tree_t* t, const ast_locatable& l) noexcept
            : ast_locatable(t, l.source_location) {
        /* do nothing */
    }

    ast_locatable(const ast_locatable&) = delete;

    property<source_span>                           source_location;    //! the location of this node in the source code
};


template<typename T, typename U = T>
using enable_if_locatable_t =
        typename enable_if_base_of<ast_locatable, T, U>::type;


/**
 * Properties for ast elements that can have definitions outside of the current
 * translation unit
 */
struct ast_externable {

    explicit inline ast_externable(tree_t* p, bool is_extern = false, bool is_extern_visible = true) noexcept
            : is_extern(p, is_extern),
              is_extern_visible(p, is_extern_visible) {
        // do nothing
    }

    explicit inline ast_externable(tree_t* p, const ast_externable& e) noexcept
            : ast_externable(p, e.is_extern, e.is_extern_visible) {
        /* do nothing */
    }

    ast_externable(const ast_externable&) = delete;

    property<bool>                                  is_extern;          //! is defined externally
    property<bool>                                  is_extern_visible;  //! is visible outside of this module
};


template<typename T, typename U = T>
using enable_if_externable_t =
        typename enable_if_base_of<ast_externable, T, U>::type;


/**
 * Base class for all ast nodes
 */
struct ast_tree : public extend_tree<tree_type_id::ast_tree> {
public:

    /**
     * Passing constructor for ast_tree
     * @param id overriding tree type id
     */
    explicit inline ast_tree(tree_type_id id) noexcept
            : base_type(id) {
        // do nothing
    }

    /**
     * Passing cloning constructor for ast_tree
     */
    explicit inline ast_tree(const ast_tree& t) noexcept
            : base_type(t._type) {
        // do nothing
    }

};


/**
 * Base class for all type ast nodes
 */
struct ast_type : public implement_tree<tree_type_id::ast_type> {
public:

    /**
     * Passing constructor
     * @param id overriding tree type id
     */
    inline ast_type(tree_type_id id) noexcept
            : base_type(id) {
        //...
    }


    /**
     * Passing constructor for cloning
     */
    inline ast_type(const ast_type& t) noexcept
            : base_type((base_type&) t) {
        // do nothing
    }

};


/**
 * Base class for all declaration ast nodes
 */
struct ast_decl : public implement_tree<tree_type_id::ast_decl>,
                  public ast_locatable {
public:

    /**
     * Passing constructor
     * @param id overriding tree type id
     * @param name declaration name
     */
    inline ast_decl(tree_type_id id, std::string name) noexcept
            : base_type(id),
              ast_locatable(this),
              name(this, name),
              generated_name(this, "") {
        //...
    }

    /**
     * Passing constructor for cloning
     */
    inline ast_decl(const ast_decl& d) noexcept
            : base_type((base_type&) d),
              ast_locatable(this, d),
              name(this, d.name),
              generated_name(this, d.generated_name) {
        // do nothing
    }

    property<std::string>                                       name;               //!< declaration name
    property<std::string>                                       generated_name;     //!< mangled declaration name

};

enum class ast_value_catagory : uint64_t {
    none,
    rvalue,
    lvalue
};


/**
 * Base class for all expression ast nodes
 */
struct ast_expr : public implement_tree<tree_type_id::ast_expr>,
                  public ast_locatable {
public:

    /**
     * Passing constructor
     * @param id overriding tree type id
     * @param type expression type
     */
    inline ast_expr(tree_type_id id, ast_type* type = nullptr, ast_value_catagory vcat=ast_value_catagory::none) noexcept
            : base_type(id),
              ast_locatable(this),
              value_type(this, vcat),
              type(this, type) {
        //...
    }

    /**
     * Passing constructor for cloning
     */
    inline ast_expr(const ast_expr& e) noexcept
            : base_type((base_type&) e),
              ast_locatable(this, e),
              value_type(this, e.value_type),
              type(this, e.type) {
        // do nothing
    }

    property<ast_value_catagory>                                value_type;
    property<ast_type>                                          type;   //!< Expression type

};


/**
 * Base class for all statement ast nodes
 */
struct ast_stmt : public implement_tree<tree_type_id::ast_stmt>,
                  public ast_locatable {
public:

    /**
     * Passing constructor
     * @param id overriding type id
     */
    inline ast_stmt(tree_type_id id) noexcept
            : base_type(id),
              ast_locatable(this) {
        //...
    }

    /**
     * Passing constructor for cloning
     */
    inline ast_stmt(const ast_stmt& s) noexcept
            : base_type((base_type&) s),
              ast_locatable(this, s){
        // do nothing
    }

};


inline bool is_type(const tree_t* t) noexcept { return t->is<ast_type>(); }
inline bool is_decl(const tree_t* t) noexcept { return t->is<ast_decl>(); }
inline bool is_expr(const tree_t* t) noexcept { return t->is<ast_expr>(); }
inline bool is_stmt(const tree_t* t) noexcept { return t->is<ast_stmt>(); }

template<typename T, typename U = void> struct enable_if_decl : enable_if_base_of<ast_decl, T, U> { };
template<typename T, typename U = void> struct enable_if_type : enable_if_base_of<ast_type, T, U> { };
template<typename T, typename U = void> struct enable_if_expr : enable_if_base_of<ast_expr, T, U> { };
template<typename T, typename U = void> struct enable_if_stmt : enable_if_base_of<ast_stmt, T, U> { };

template<typename T, typename U = void> using enable_if_decl_t = typename enable_if_decl<T, U>::type;
template<typename T, typename U = void> using enable_if_type_t = typename enable_if_type<T, U>::type;
template<typename T, typename U = void> using enable_if_expr_t = typename enable_if_expr<T, U>::type;
template<typename T, typename U = void> using enable_if_stmt_t = typename enable_if_stmt<T, U>::type;


/**
 * A Namespace
 */
struct ast_namespace_decl : public implement_tree<tree_type_id::ast_namespace_decl> {
public:

    explicit inline ast_namespace_decl(std::string name) noexcept
            : base_type(name),
              declarations(this, new list<ast_decl>()) {
        // do nothing
    }

    explicit inline ast_namespace_decl(std::string name, list<ast_decl>* declarations) noexcept
            : base_type(name),
              declarations(this, declarations) {
        // do nothing
    }

    explicit inline ast_namespace_decl(tree_type_id id, std::string name) noexcept
            : base_type(id, name),
              declarations(this, new list<ast_decl>()) {
        // do nothing
    }

    explicit inline ast_namespace_decl(tree_type_id id, std::string name, list<ast_decl>* declarations) noexcept
            : base_type(id, name),
              declarations(this, declarations) {
        // do nothing
    }

    explicit inline ast_namespace_decl(const ast_namespace_decl& n) noexcept
            : base_type((base_type&) n),
              declarations(this, n.declarations) {
        // do nothing
    }

    property<list<ast_decl>>                                    declarations;   //!<

};


/**
 * Global variable declaration
 */
struct ast_variable_decl final : public implement_tree<tree_type_id::ast_variable_decl>,
                                 public ast_externable {
public:

    /**
     * @param name
     * @param type
     * @param initial_value
     */
    explicit inline ast_variable_decl(std::string name, ast_type* type, ast_expr* initial_value = nullptr) noexcept
            : base_type(name),
              ast_externable(this, false, true),
              type(this, type),
              initial_value(this, initial_value) {
        // do nothing
    }

    /**
     * Passing constructor for cloning
     */
    explicit inline ast_variable_decl(const ast_variable_decl& v) noexcept
            : base_type((base_type&) v),
              ast_externable(this, v),
              type(this, v.type),
              initial_value(this, v.initial_value) {
        // do nothing
    }

    property<ast_type>                                          type;               //!< variable type
    property<ast_expr>                                          initial_value;      //!< initial value

};


/**
 * Function parameter
 */
struct ast_parameter_decl : public implement_tree<tree_type_id::ast_parameter_decl> {
public:

    /**
     * @param name
     * @param type
     */
    explicit inline ast_parameter_decl(std::string name, ast_type* type) noexcept
            : base_type(name),
              type(this, type) {
        // do nothing
    }

    /**
     * Passing constructor for derived types
     * @param id
     * @param name
     * @param type
     */
    explicit inline ast_parameter_decl(tree_type_id id, std::string name, ast_type* type) noexcept
            : base_type(id, name),
              type(this, type) {
        // do nothing
    }

    /**
     * Passing constructor for cloning
     */
    explicit inline ast_parameter_decl(const ast_parameter_decl& p) noexcept
            : base_type((base_type&) p),
              type(this, p.type) {
        // do nothing
    }

    property<ast_type>                                          type; //!<

};


/**
 * A local variable within a function
 */
struct ast_local_decl final : public implement_tree<tree_type_id::ast_local_decl> {
public:

    /**
     * @param name
     * @param type
     * @param init_value
     */
    inline ast_local_decl(std::string name, ast_type* type, ast_expr* init_value) noexcept
            : base_type(name),
              type(this, type),
              init_value(this, init_value) {
        //...
    }

    /**
     * @param type
     * @param init_value
     */
    inline ast_local_decl(ast_type* type, ast_expr* init_value) noexcept
            : base_type(std::string("anonymous")),
              type(this, type),
              init_value(this, init_value) {
        //...
    }

    /**
     * Passing constructor for cloning
     */
    inline ast_local_decl(const ast_local_decl& d) noexcept
            : base_type((base_type&) d),
              type(this, d.type),
              init_value(this, d.init_value) {
        // do nothing
    }

    property<ast_type>                                          type;           //!<
    property<ast_expr>                                          init_value;     //!<

};


/**
 * Holds a temporary value
 */
struct ast_temp_decl final : public implement_tree<tree_type_id::ast_temp_decl> {
public:

    explicit inline ast_temp_decl(std::string name, ast_type* type, ast_expr* value) noexcept
            : base_type(name),
              type(this, type),
              value(this, value) {
        /* do nothing */
    }

    explicit inline ast_temp_decl(const ast_temp_decl& t) noexcept
            : base_type((base_type&) t),
              type(this, t.type),
              value(this, t.value) {
        /* do nothing */
    }

    property<ast_type>                                          type;       //!
    property<ast_expr>                                          value;      //!

};


/**
 * Function declaration
 */
struct ast_function_decl final : public implement_tree<tree_type_id::ast_function_decl>,
                                 public ast_externable {
public:

    /**
     * @param name function name
     * @param return_type the function return type
     * @param parameters list of parameter declarations
     * @param body the function body
     */
    explicit inline ast_function_decl(std::string name, ast_type* return_type, list<ast_parameter_decl>* parameters, ast_stmt* body) noexcept
            : base_type(name),
              ast_externable(this, false, true),
              return_type(this, return_type),
              parameters(this, parameters),
              body(this, body),
              is_varargs(this, false),
              is_c_extern(this, false) {
        // do nothing
    }

    explicit inline ast_function_decl(const ast_function_decl& f) noexcept
            : base_type((base_type&) f),
              ast_externable(this, f),
              return_type(this, f.return_type),
              parameters(this, f.parameters),
              body(this, f.body),
              is_varargs(this, f.is_varargs),
              is_c_extern(this, f.is_c_extern) {
        // do nothing
    }

    property<ast_type>                                          return_type;        //!<
    property<list<ast_parameter_decl>>                          parameters;         //!<
    property<ast_stmt>                                          body;               //!<
    property<bool>                                              is_varargs;         //!<
    property<bool>                                              is_c_extern;        //!< use c style mangling (none)

};


/**
 * The void type
 */
struct ast_void_type final : public implement_tree<tree_type_id::ast_void_type> {
public:

    /**
     *
     */
    inline ast_void_type() noexcept : base_type() {
        //...
    }

    inline ast_void_type(const ast_void_type& v) noexcept
            : base_type((base_type&) v) {
        // do nothing
    }

};


/**
 * An integer type
 */
struct ast_integer_type final : public implement_tree<tree_type_id::ast_integer_type> {

    /**
     * @param bitwidth
     * @param is_unsigned
     */
    inline ast_integer_type(uint32_t bitwidth, bool is_unsigned) noexcept
            : base_type(),
              bitwidth(this, bitwidth),
              is_unsigned(this, is_unsigned) {
        // do nothing
    }

    inline ast_integer_type(const ast_integer_type& i) noexcept
            : base_type((base_type&) i),
              bitwidth(this, i.bitwidth),
              is_unsigned(this, i.is_unsigned) {
        // do nothing
    }

    property<uint32_t>                                          bitwidth;       //!<
    property<bool>                                              is_unsigned;    //!<

};


/**
 * A floating point type
 */
struct ast_real_type final : public implement_tree<tree_type_id::ast_real_type> {
public:

    /**
     * @param bitwidth
     */
    inline ast_real_type(uint32_t bitwidth) noexcept
            : base_type(),
              bitwidth(this, bitwidth) {
        // do nothing
    }

    inline ast_real_type(const ast_real_type& r) noexcept
            : base_type((base_type&) r),
              bitwidth(this, r.bitwidth) {
        // do nothing
    }

    property<uint32_t>                                          bitwidth;   //!<

};


/**
 * An array type
 */
struct ast_array_type final : public implement_tree<tree_type_id::ast_array_type> {
public:

    /**
     * @param element_type
     * @param size
     */
    inline ast_array_type(ast_type* element_type, uint32_t size) noexcept
            : base_type(),
              element_type(this, element_type),
              size(this, size) {
        // do nothing
    }

    inline ast_array_type(const ast_array_type& a) noexcept
            : base_type((base_type&) a),
              element_type(this, a.element_type),
              size(this, a.size) {
        // do nothing
    }

    property<ast_type>                                          element_type;   //!<
    property<uint32_t>                                          size;           //!<

};


/**
 * A pointer type
 */
struct ast_pointer_type final : public implement_tree<tree_type_id::ast_pointer_type> {
public:

    /**
     * @param element_type
     */
    inline ast_pointer_type(ast_type* element_type) noexcept
            : base_type(),
              element_type(this, element_type) {
        // do nothing
    }

    inline ast_pointer_type(const ast_pointer_type& p) noexcept
            : base_type((base_type&) p),
              element_type(this, p.element_type) {
        // do nothing
    }

    property<ast_type>                                          element_type;   //!<

};


/**
 * A function type
 */
struct ast_function_type final : public implement_tree<tree_type_id::ast_function_type> {
public:

    /**
     * @param return_type
     * @param parameter_types
     */
    inline ast_function_type(ast_type* return_type, list<ast_type>* parameter_types, bool is_varargs) noexcept
            : base_type(),
              return_type(this, return_type),
              parameter_types(this, parameter_types),
              is_varargs(this, is_varargs) {
        // do nothing
    }

    inline ast_function_type(const ast_function_type& f) noexcept
            : base_type((base_type&) f),
              return_type(this, f.return_type),
              parameter_types(this, f.parameter_types),
              is_varargs(this, f.is_varargs) {
        // do nothing
    }

    property<ast_type>                                          return_type;        //!<
    property<list<ast_type>>                                    parameter_types;    //!<
    property<bool>                                              is_varargs;         //!<

};


/**
 * A record type
 */
struct ast_record_type final : public implement_tree<tree_type_id::ast_record_type> {
public:

    /**
     * @param is_packed is this type tightly packed ?
     */
    inline ast_record_type(bool is_packed = false) noexcept
            : base_type(),
              field_types(this, new list<ast_type>()),
              is_packed(this, is_packed){
        // do nothing
    }

    /**
     * @param types
     * @param is_packed
     */
    inline ast_record_type(list<ast_type>* types, bool is_packed = false) noexcept
            : base_type(),
              field_types(this, types),
              is_packed(this, is_packed) {
        // do nothing
    }

    inline ast_record_type(const ast_record_type& r) noexcept
            : base_type((base_type&) r),
              field_types(this, r.field_types),
              is_packed(this, r.is_packed) {
        // do nothing
    }

    property<list<ast_type>>                                    field_types;    //!<
    property<bool>                                              is_packed;      //!<

};


inline bool is_void_type(ast_tree* t)     noexcept { return t->is<ast_void_type>(); }
inline bool is_integer_type(ast_tree* t)  noexcept { return t->is<ast_integer_type>(); }
inline bool is_real_type(ast_tree* t)     noexcept { return t->is<ast_real_type>(); }
inline bool is_array_type(ast_tree* t)    noexcept { return t->is<ast_array_type>(); }
inline bool is_pointer_type(ast_tree* t)  noexcept { return t->is<ast_pointer_type>(); }
inline bool is_function_type(ast_tree* t) noexcept { return t->is<ast_function_type>(); }
inline bool is_record_type(ast_tree* t)   noexcept { return t->is<ast_record_type>(); }


/**
 * A constant integer value
 */
struct ast_integer final : public implement_tree<tree_type_id::ast_integer> {
public:

    /**
     * @param itype
     * @param value
     */
    inline ast_integer(ast_type* itype, llvm::APSInt value) noexcept
            : base_type(itype, ast_value_catagory::rvalue),
              value(this, value) {
        // do nothing
    }

    /**
     *
     * @param itype
     * @param value
     */
    inline ast_integer(ast_type* itype, llvm::APInt value) noexcept
            : base_type(itype, ast_value_catagory::rvalue),
              value(this, llvm::APSInt(value, !itype->as<ast_integer_type>()->is_unsigned)) {
    }

    inline ast_integer(const ast_integer& i) noexcept
            : base_type((base_type&) i),
              value(this, i.value) {
        // do nothing
    }

    property<llvm::APSInt>                                      value;  //!<

};


/**
 * A constant floating point value
 */
struct ast_real final : public implement_tree<tree_type_id::ast_real> {
public:

    /**
     * @param ftype
     * @param value
     */
    inline ast_real(ast_type* ftype, llvm::APFloat value)
            : base_type(ftype, ast_value_catagory::rvalue),
              value(this, value) {
        // do nothing
    }

    inline ast_real(const ast_real& r) noexcept
            : base_type((base_type&) r),
              value(this, r.value) {
        // do nothing
    }

    property<llvm::APFloat>                                     value;  //!<

};


/**
 * A constant string value
 */
struct ast_string final : public implement_tree<tree_type_id::ast_string> {
public:

    /**
     * @param stype
     * @param value
     */
    inline ast_string(ast_type* stype, std::string value)
            : base_type(stype, ast_value_catagory::rvalue),
              value(this, value) {
        // do nothing
    }

    inline ast_string(const ast_string& s) noexcept
            : base_type((base_type&) s),
              value(this, s.value) {
        // do nothing
    }

    property<std::string>                                       value; //!<

};


/**
 * An array value
 */
struct ast_array final : public implement_tree<tree_type_id::ast_array> {
public:

    /**
     * @param atype    the array type
     * @param values   a list of values
     */
    inline ast_array(ast_type* atype, list<ast_expr>* values)
            : base_type(atype, ast_value_catagory::rvalue),
              values(this, values) {
        // do nothing
    }

    inline ast_array(const ast_array& a) noexcept
            : base_type((base_type&) a),
              values(this, a.values) {
        // do nothing
    }

    property<list<ast_expr>>                                    values; //!<

};


/**
 * A record value
 */
struct ast_record final : public implement_tree<tree_type_id::ast_record> {
public:

    /**
     * @param rtype     the record type
     * @param values    a list of values
     */
    inline ast_record(ast_type* rtype, list<ast_expr>* values)
            : base_type(rtype, ast_value_catagory::rvalue),
              values(this, values) {
        // do nothing
    }

    inline ast_record(const ast_record& r) noexcept
            : base_type((base_type&) r),
              values(this, r.values) {
        // do nothing
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
    lxor,                                                               //!< Logical xor
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
     * @param type Type to cast to
     * @param expr
     */
    inline ast_cast(ast_type* type, ast_op op, ast_expr* expr)
            : base_type(type, ast_value_catagory::rvalue),
              op(this, op),
              expr(this, expr) {
        // do nothing
    }

    inline ast_cast(const ast_cast& c) noexcept
            : base_type((base_type&) c),
              op(this, c.op),
              expr(this, c.expr) {
        // do nothing
    }

    property<ast_op>                                            op;     //!< The low level casting operator
    property<ast_expr>                                          expr;   //!<

};


/**
 * A binary operator expression
 */
struct ast_binary_op final : public implement_tree<tree_type_id::ast_binary_op> {
public:

    /**
     * @param type
     * @param op
     * @param lhs
     * @param rhs
     */
    inline ast_binary_op(ast_type* type, ast_op op, ast_expr* lhs, ast_expr* rhs) noexcept
            : base_type(type, ast_value_catagory::rvalue),
              op(this, op),
              lhs(this, lhs),
              rhs(this, rhs) {
        // do nothing
    }

    inline ast_binary_op(const ast_binary_op& b) noexcept
            : base_type((base_type&) b),
              op(this, b.op),
              lhs(this, b.lhs),
              rhs(this, b.rhs) {
        // do nothing
    }

    property<ast_op>                                            op;     //!<
    property<ast_expr>                                          lhs;    //!<
    property<ast_expr>                                          rhs;    //!<

};


/**
 * A unary operator expression
 */
struct ast_unary_op final : public implement_tree<tree_type_id::ast_unary_op> {
public:

    /**
     * @param type
     * @param op
     * @param expr
     */
    inline ast_unary_op(ast_type* type, ast_op op, ast_expr* expr) noexcept
            : base_type(type, ast_value_catagory::rvalue),
              op(this, op),
              expr(this, expr) {
        // do nothing
    }

    inline ast_unary_op(const ast_unary_op& u) noexcept
            : base_type((base_type&) u),
              op(this, u.op),
              expr(this, u.expr) {
        // do nothing
    }

    property<ast_op>                                            op;     //!<
    property<ast_expr>                                          expr;   //!<

};


/**
 * An array index expression
 */
struct ast_index final : public implement_tree<tree_type_id::ast_index> {
public:

    /**
     * @param type
     * @param expr
     * @param index
     */
    inline ast_index(ast_type* type, ast_expr* expr, ast_expr* index)
            : base_type(type, ast_value_catagory::lvalue),
              arr_expr(this, expr),
              index_expr(this, index) {
        // do nothing
    }

    inline ast_index(const ast_index& i) noexcept
            : base_type((base_type&) i),
              arr_expr(this, i.arr_expr),
              index_expr(this, i.index_expr) {
        // do nothing
    }

    property<ast_expr>                                          arr_expr;   //!<
    property<ast_expr>                                          index_expr; //!<

};


/**
 * A reference to a parameter, local, or global variable
 */
struct ast_declref final : public implement_tree<tree_type_id::ast_declref> {
public:

    /**
     * @param type
     * @param variable
     */
    inline ast_declref(ast_type* type, ast_decl* declaration) noexcept
            : base_type(type, ast_value_catagory::lvalue),
              declaration(this, declaration){
        //...
    }

    inline ast_declref(const ast_declref& r) noexcept
            : base_type((base_type&) r),
              declaration(this, r.declaration) {
        // do nothing
    }

    weak_ref<ast_decl>                                          declaration;   //!<
};


/**
 * A reference to a record member
 */
struct ast_memberref final : public implement_tree<tree_type_id::ast_memberref> {
public:

    inline ast_memberref(ast_type* type, ast_expr* objexpr, uint32_t member) noexcept
            : base_type(type, ast_value_catagory::lvalue),
              objexpr(this, objexpr),
              member_index(this, member) {
        // do nothing
    }

    inline ast_memberref(const ast_memberref& r) noexcept
            : base_type((base_type&) r),
              objexpr(this, r.objexpr),
              member_index(this, r.member_index) {
        // do nothing
    }

    property<ast_expr>                                          objexpr;        //!<
    property<uint32_t>                                          member_index;   //!<

};



/**
 * A pointer dereference expression
 */
struct ast_deref final : public implement_tree<tree_type_id::ast_deref> {
public:

    /**
     * @param type
     * @param expr
     */
    inline ast_deref(ast_type* type, ast_expr* expr) noexcept
            : base_type(type, ast_value_catagory::lvalue),
              expr(this, expr) {
        // do nothing
    }

    inline ast_deref(const ast_deref& r) noexcept
            : base_type((base_type&) r),
              expr(this, r.expr) {
        // do nothing
    }

    property<ast_expr>                                          expr;   //!<

};


/**
 * An address of expression
 */
struct ast_addressof final : public implement_tree<tree_type_id::ast_addressof> {
public:

    /**
     * @param type
     * @param expr
     */
    inline ast_addressof(ast_type* type, ast_expr* expr) noexcept
            : base_type(type, ast_value_catagory::rvalue),
              expr(this, expr) {
        // do nothing
    }

    inline ast_addressof(const ast_addressof& a) noexcept
            : base_type((base_type&) a),
              expr(this, a.expr) {
        // do nothing
    }

    property<ast_expr>                                          expr;   //!<

};


/**
 * A function call by address expression
 */
struct ast_invoke final : public implement_tree<tree_type_id::ast_invoke> {
public:

    /**
     * @param type
     * @param funcexpr
     * @param arguments
     */
    inline ast_invoke(ast_type* type, ast_expr* funcexpr, list<ast_expr>* arguments) noexcept
            : base_type(type, ast_value_catagory::rvalue),
              funcexpr(this, funcexpr),
              arguments(this, arguments) {
        // do nothing
    }

    inline ast_invoke(const ast_invoke& i) noexcept
            : base_type((base_type&) i),
              funcexpr(this, i.funcexpr),
              arguments(this, i.arguments) {
        // do nothing
    }

    property<ast_expr>                                          funcexpr;   //!<
    property<list<ast_expr>>                                    arguments;  //!<

};


/**
 * A function call by name expression
 */
struct ast_call final : public implement_tree<tree_type_id::ast_call> {
public:

    /**
     * @param type
     * @param funcdecl
     * @param arguments
     */
    explicit inline ast_call(ast_type* type, ast_declref* funcdecl, list<ast_expr>* arguments) noexcept
            : base_type(type, ast_value_catagory::lvalue),
              funcdecl(this, funcdecl),
              arguments(this, arguments) {
        // do nothing
    }

    explicit inline ast_call(const ast_call& c) noexcept
            : base_type((base_type&) c),
              funcdecl(this, c.funcdecl),
              arguments(this, c.arguments) {
        // do nothing
    }

    property<ast_declref>                                       funcdecl;   //!<
    property<list<ast_expr>>                                    arguments;  //!<

};


/**
 * A list of stmts evaluated for side effects, and then an expression
 */
struct ast_stmt_expr final : public implement_tree<tree_type_id::ast_stmt_expr> {
public:


    inline ast_stmt_expr(ast_type* type, ast_temp_decl* temp, ast_stmt* statement) noexcept
            : base_type(type, ast_value_catagory::rvalue),
              temp(this, temp),
              statement(this, statement) {
        // do nothing
    }

    inline ast_stmt_expr(const ast_stmt_expr& s) noexcept
            : base_type((base_type&) s),
              temp(this, s.temp),
              statement(this, s.statement) {
        // do nothing
    }

    property<ast_temp_decl>                                     temp;       //!< A temporary variable with an initial value
    property<ast_stmt>                                          statement;  //!< A statement to to run after computing temp

};


/**
 * A no operation stmt (does nothing)
 */
struct ast_nop_stmt final : public implement_tree<tree_type_id::ast_nop_stmt> {
public:

    /**
     *
     */
    inline ast_nop_stmt() noexcept : base_type() { }

    inline ast_nop_stmt(const ast_nop_stmt& s) noexcept
            : base_type((base_type&) s) {
        // do nothing
    }

};


/**
 * An expression statement
 */
struct ast_expr_stmt final : public implement_tree<tree_type_id::ast_expr_stmt> {
public:

    /**
     * @param expr
     */
    inline ast_expr_stmt(ast_expr* expr) noexcept
            : base_type(),
              expr(this, expr) {
        // do nothing
    }

    inline ast_expr_stmt(const ast_expr_stmt& s) noexcept
            : base_type((base_type&) s),
              expr(this, s.expr) {
        // do nothing
    }

    property<ast_expr>                                          expr;   //!<

};


/**
 * Variable assignment statement
 */
struct ast_assign_stmt final : public implement_tree<tree_type_id::ast_assign_stmt> {
public:

    /**
     * @param lhs
     * @param rhs
     */
    inline ast_assign_stmt(ast_expr* lhs, ast_expr* rhs) noexcept
            : base_type(),
              lhs(this, lhs),
              rhs(this, rhs) {
        // do nothing
    }

    inline ast_assign_stmt(const ast_assign_stmt& s) noexcept
            : base_type((base_type&) s),
              lhs(this, s.lhs),
              rhs(this, s.rhs) {
        // do nothing
    }

    property<ast_expr>                                          lhs;    //!<
    property<ast_expr>                                          rhs;    //!<

};


/**
 * Declaration of a local variable
 */
struct ast_decl_stmt final : public implement_tree<tree_type_id::ast_decl_stmt> {
public:

    inline ast_decl_stmt(ast_local_decl* decl) noexcept
            : base_type(),
              decl(this, decl) {
        // do nothing
    }

    inline ast_decl_stmt(const ast_decl_stmt& s) noexcept
            : base_type((base_type&) s),
              decl(this, s.decl) {
        // do nothing
    }

    property<ast_local_decl>                                    decl;   //!<
};


/**
 * A block statement with local variable declarations that are initialized
 *  at the beginning of the block, and destroyed at the end
 */
struct ast_block_stmt final : public implement_tree<tree_type_id::ast_block_stmt> {
public:

    /**
     * @param decls list of declarations
     * @param stmts a sequence of statements to be executed in order
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
        // do nothing
    }

    inline ast_block_stmt(const ast_block_stmt& s) noexcept
            : base_type((base_type&) s),
              decls(this, s.decls),
              stmts(this, s.stmts) {
        // do nothing
    }

    property<list<ast_local_decl>>                              decls;  //!<
    property<list<ast_stmt>>                                    stmts;  //!<

};


/**
 * An if statement
 */
struct ast_if_stmt final : public implement_tree<tree_type_id::ast_if_stmt> {
public:

    /**
     * @param condition
     * @param true_stmt     stmt to execute if the condition is true
     * @param false_stmt    stmt to execute if the condition is false
     */
    inline ast_if_stmt(ast_expr* condition, ast_stmt* true_stmt, ast_stmt* false_stmt) noexcept
            : base_type(),
              condition(this, condition),
              true_stmt(this, true_stmt),
              false_stmt(this, false_stmt) {
        // do nothing
    }

    inline ast_if_stmt(const ast_if_stmt& i) noexcept
            : base_type((base_type&) i),
              condition(this, i.condition),
              true_stmt(this, i.true_stmt),
              false_stmt(this, i.false_stmt) {
        // do nothing
    }

    property<ast_expr>                                          condition;  //!<
    property<ast_stmt>                                          true_stmt;  //!<
    property<ast_stmt>                                          false_stmt; //!<

};


/**
 * A while statement
 */
struct ast_while_stmt final : public implement_tree<tree_type_id::ast_while_stmt> {
public:

    /**
     * @param condition
     * @param stmt
     */
    inline ast_while_stmt(ast_expr* condition, ast_stmt* stmt) noexcept
            : base_type(),
              condition(this, condition),
              stmt(this, stmt) {
        // do nothing
    }

    inline ast_while_stmt(const ast_while_stmt& w) noexcept
            : base_type((base_type&) w),
              condition(this, w.condition),
              stmt(this, w.stmt) {
        // do nothing
    }

    property<ast_expr>                                          condition;  //!<
    property<ast_stmt>                                          stmt;       //!<

};


/**
 * A for statement
 */
struct ast_for_stmt final : public implement_tree<tree_type_id::ast_for_stmt> {
public:

    /**
     * @param init_stmt
     * @param condition
     * @param each_stmt
     */
    inline ast_for_stmt(ast_stmt* init_stmt, ast_expr* condition, ast_stmt* each_stmt, ast_stmt* body) noexcept
            : base_type(),
              init_stmt(this, init_stmt),
              condition(this, condition),
              each_stmt(this, each_stmt),
              body(this, body) {
        // do nothing
    }

    inline ast_for_stmt(const ast_for_stmt& f) noexcept
            : base_type((base_type&) f),
              init_stmt(this, f.init_stmt),
              condition(this, f.condition),
              each_stmt(this, f.each_stmt),
              body(this, f.body) {
        // do nothing
    }

    property<ast_stmt>                                          init_stmt;  //!<
    property<ast_expr>                                          condition;  //!<
    property<ast_stmt>                                          each_stmt;  //!<
    property<ast_stmt>                                          body;

};


/**
 * A return statement
 */
struct ast_return_stmt final : public implement_tree<tree_type_id::ast_return_stmt> {
public:

    /**
     * @param expr null if returning nothing
     */
    inline ast_return_stmt(ast_expr* expr) noexcept
            : base_type(),
              expr(this, expr) {
        // do nothing
    }

    inline ast_return_stmt(const ast_return_stmt& r) noexcept
            : base_type((base_type&) r),
              expr(this, r.expr) {
        // do nothing
    }

    property<ast_expr>                                          expr;   //!<

};


/**
 * A break statement
 */
struct ast_break_stmt final : public implement_tree<tree_type_id::ast_break_stmt> {
public:

    /**
     *
     */
    inline ast_break_stmt() noexcept { }

    inline ast_break_stmt(const ast_break_stmt& s) noexcept
            : base_type((base_type&) s) {
        // do nothing
    }

};


/**
 * A continue statement
 */
struct ast_continue_stmt final : public implement_tree<tree_type_id::ast_continue_stmt> {
public:

    inline ast_continue_stmt() noexcept { }

    inline ast_continue_stmt(const ast_continue_stmt& s) noexcept
            : base_type((base_type&) s) {
        // do nothing
    }

};

const char* to_string(xcc::ast_op op);

}





namespace std {
inline const char* to_string(xcc::ast_op op) { return xcc::to_string(op); }
}

namespace xcc {

/**
 * Set the source location of an ast_tree node
 * @param t     the node
 * @param loc   the source location
 * @return      a pointer to the same node
 */
template<typename T>
inline enable_if_locatable_t<T, T*> setloc(T* t, const source_span& loc) noexcept {
    if(t != nullptr) {
        t->source_location = loc;
    }
    return t;
}

/**
 * Set teh source location of an ast_tree node
 * @param t         the node
 * @param filename  the name of the source file
 * @param line      the lime number
 * @param col       the column number at the begining of the node
 * @param width     the textual width of the node
 * @return          a pointer to the same node
 */
template<typename T>
inline enable_if_locatable_t<T, T*> setloc(T* t, std::string filename, uint32_t line, uint32_t col, uint32_t width) noexcept {
    if(t != nullptr) {
        t->source_location = {
                { filename, line, col },
                { filename, line, col + width }
        };
    }
    return t;
}

/**
 * Copy the location of one ast_tree node to another
 * @param t     the destination node
 * @param ft    the source node
 * @return      a pointer to the destination node
 */
template<typename TDestTreeType,
         typename TSrcTreeType,
         enable_if_locatable_t<TDestTreeType, int> = 0,
         enable_if_locatable_t<TSrcTreeType, int> = 0>
TDestTreeType* copyloc(TDestTreeType* t, TSrcTreeType* ft) noexcept {
    t->source_location = ft->source_location;
    return t;
}

/**
 * Copy the location from a begining and end ast_tree nodes to a destination node
 * @param t     the destination node
 * @param fmin  the first node
 * @param fmax  the last node
 * @return      a pointer to the destination node
 */
template<typename TDestTreeType,
         typename TSrcMinTreeType,
         typename TSrcMaxTreeType,
         enable_if_locatable_t<TDestTreeType, int> = 0,
         enable_if_locatable_t<TSrcMinTreeType, int> = 0,
         enable_if_locatable_t<TSrcMaxTreeType, int> = 0>
TDestTreeType* copyloc(TDestTreeType *t, TSrcMinTreeType* fmin, TSrcMaxTreeType* fmax) noexcept {
    t->source_location->first   = fmin->source_location->first;
    t->source_location->last    = fmax->source_location->last;
    return t;
}

}




#endif /* AST_HPP_ */
