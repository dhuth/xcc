/*
 * xi_tree.hpp
 *
 *  Created on: Feb 6, 2017
 *      Author: derick
 */

#ifndef XI_XI_TREE_HPP_
#define XI_XI_TREE_HPP_

#include "ast.hpp"

namespace xcc {

enum class xi_operator : uint32_t {
    none,
    dot,
    add,
    sub,
    mul,
    div,
    mod,
    shl,
    shr,
    eq,
    ne,
    lt,
    le,
    gt,
    ge,
    lnot,
    land,
    lor,
    bnot,
    band,
    bor,
    bxor,
    assign,
    assign_add,
    assign_sub,
    assign_mul,
    assign_div,
    assign_mod,
    assign_shl,
    assign_shr,
    assign_land,
    assign_lor,
    assign_band,
    assign_bor,
    index,
    invoke,
};

struct xi_const_type : public extend_tree<tree_type_id::xi_const_type, ast_type> {
public:

    inline xi_const_type(ast_type* type)
            : base_type(),
              type(this, type) {
        //...
    }

    property<ast_type>                              type;

};

struct xi_array_type : public extend_tree<tree_type_id::xi_array_type, ast_type> {
public:

    inline xi_array_type(ast_type* eltype, list<ast_expr>* dims)
            : base_type(),
              element_type(this, eltype),
              dimensions(this, dims) {
        //...
    }

    property<ast_type>                              element_type;
    property<list<ast_expr>>                        dimensions;

};

struct xi_name_decl : public extend_tree<tree_type_id::xi_name_decl, ast_decl> {
public:

    inline xi_name_decl(const char* name)
            : base_type(name) {
        //...
    }
};

struct xi_infered_type : public extend_tree<tree_type_id::xi_infered_type, ast_type> {
public:

    inline xi_infered_type()
            : base_type() {
        //...
    }
};

struct xi_ref_type : public extend_tree<tree_type_id::xi_ref_type, ast_type> {
public:

    inline xi_ref_type(ast_type* eltype)
            : base_type(),
              element_type(this, eltype) {
        //...
    }

    property<ast_type>                              element_type;

};

struct xi_member_decl : public extend_tree<tree_type_id::xi_member_decl, ast_decl> {
public:

    inline xi_member_decl(tree_type_id passing_id, const char* name, xi_type_decl* parent_decl)
            : base_type(passing_id, name),
              parent_decl(parent_decl) {
        //...
    }

    inline xi_member_decl(tree_type_id passing_id, const char* name)
            : base_type(passing_id, name),
              parent_decl(nullptr) {
        //...
    }

    xi_type_decl*                                   parent_decl;

};

struct xi_type_decl : public extend_tree<tree_type_id::xi_type_decl, xi_member_decl> {
public:

    inline xi_type_decl(tree_type_id passing_id, const char* name, list<ast_type>* basetypes)
            : base_type(passing_id, name, nullptr),
              basetypes(this, basetypes),
              mixins(this, nullptr),
              members(this, new list<xi_member_decl>()),
              instance_field_count(this, 0),
              static_field_count(this, 0) {
        //...
    }

    property<list<ast_type>>                        basetypes;
    property<list<ast_decl>>                        mixins;
    property<list<xi_member_decl>>                  members;
    property<uint32_t>                              instance_field_count;
    property<uint32_t>                              static_field_count;

};

struct xi_struct_decl : public extend_tree<tree_type_id::xi_struct_decl, xi_type_decl> {
public:

    inline xi_struct_decl(const char* name)
            : base_type(name, new list<ast_type>()),
              supertype(this, nullptr),
              instance_record_type(this, nullptr),
              static_record_type(this, nullptr){
        //...
    }

    inline xi_struct_decl(tree_type_id passing_id, const char* name)
            : base_type(name, new list<ast_type>()),
              supertype(this, nullptr),
              instance_record_type(this, nullptr),
              static_record_type(this, nullptr) {
        //...
    }

    inline xi_struct_decl(const char* name, list<ast_type>* basetypes)
            : base_type(name, basetypes),
              supertype(this, nullptr),
              instance_record_type(this, nullptr),
              static_record_type(this, nullptr) {
        //...
    }

    inline xi_struct_decl(tree_type_id passing_id, const char* name, list<ast_type>* basetypes)
            : base_type(passing_id, name, basetypes),
              supertype(this, nullptr),
              instance_record_type(this, nullptr),
              static_record_type(this, nullptr) {
        //...
    }

    property<xi_type_decl>                          supertype;
    property<ast_record_type>                       instance_record_type;
    property<ast_record_type>                       static_record_type;

};

struct xi_class_decl : public extend_tree<tree_type_id::xi_struct_decl, xi_struct_decl> {
public:

    inline xi_class_decl(const char* name, list<ast_type>* basetypes)
            : base_type(name, basetypes) {
        //...
    }

};

struct xi_mixin_decl : public extend_tree<tree_type_id::xi_mixin_decl, xi_type_decl> {
public:

    inline xi_mixin_decl(const char* name, list<ast_type>* basetypes)
            : base_type(name, basetypes) {
        //...
    }

};

struct xi_object_type : public extend_tree<tree_type_id::xi_object_type, ast_type> {
public:

    inline xi_object_type(xi_type_decl* decl)
            : base_type(),
              declaration(this, decl) {
        //...
    }

    property<xi_type_decl>                          declaration;

};

struct xi_field_decl : public extend_tree<tree_type_id::xi_field_decl, xi_member_decl> {
public:

    inline xi_field_decl(const char* name, ast_type* type, ast_expr* init_value, bool is_static)
            : base_type(name),
              type(this, type),
              init_value(this, init_value),
              is_static(this, is_static),
              field_index(this, 0) {
        //...
    }

    property<ast_type>                              type;
    property<ast_expr>                              init_value;
    property<bool>                                  is_static;
    property<uint32_t>                              field_index;

};

struct xi_parameter_decl : public extend_tree<tree_type_id::xi_parameter_decl, ast_decl> {
public:

    enum class direction : uint32_t {
        byval,
        byref,
        in,
        out,
    };

    inline xi_parameter_decl(const char* name, ast_type* type, direction dir = direction::byval)
            : base_type(name),
              type(this, type),
              parameter_direction(this, dir),
              generated_parameter(this, nullptr) {
        //...
    }

    property<ast_type>                              type;
    property<direction>                             parameter_direction;
    property<ast_parameter_decl>                    generated_parameter;

};

struct xi_function_decl : public extend_tree<tree_type_id::xi_function_decl, ast_decl> {
public:

    inline xi_function_decl(const char* name, ast_type* rtype, list<xi_parameter_decl>* parameters)
            : base_type(name),
              return_type(this, rtype),
              parameters(this, parameters),
              body(this, new ast_block_stmt(new list<ast_local_decl>(), new list<ast_stmt>())),
              is_extern(this, false),
              is_extern_visible(this, true),
              is_c_extern(this, false),
              generated_function(this, nullptr) {
        //...
    }

    property<ast_type>                              return_type;
    property<list<xi_parameter_decl>>               parameters;
    property<ast_block_stmt>                        body;
    property<bool>                                  is_extern;
    property<bool>                                  is_extern_visible;
    property<bool>                                  is_c_extern;
    property<ast_function_decl>                     generated_function;
};

struct xi_method_decl : public extend_tree<tree_type_id::xi_method_decl, xi_member_decl> {
public:

    inline xi_method_decl(tree_type_id passing_id, const char* name, ast_type* rtype, xi_parameter_decl* self_parameter, list<xi_parameter_decl>* parameters)
            : base_type(passing_id, name),
              function(this, new xi_function_decl(name, rtype, parameters)),
              self_parameter(this, self_parameter),
              is_static(this, false),
              is_virtual(this, false) {
        //...
    }

    inline xi_method_decl(const char* name, ast_type* rtype, xi_parameter_decl* self_parameter, list<xi_parameter_decl>* parameters)
            : base_type(name),
              function(this, new xi_function_decl(name, rtype, parameters)),
              self_parameter(this, self_parameter),
              is_static(this, false),
              is_virtual(this, false) {
        //...
    }

    property<xi_function_decl>                      function;
    property<xi_parameter_decl>                     self_parameter;
    property<bool>                                  is_static;
    property<bool>                                  is_virtual;

};

struct xi_constructor_decl : public extend_tree<tree_type_id::xi_constructor_decl, xi_method_decl> {
public:

    inline xi_constructor_decl(const char* name, ast_type* rtype, xi_parameter_decl* self_parameter, list<xi_parameter_decl>* parameters)
            : base_type(name, rtype, self_parameter, parameters) {
        //...
    }

};

struct xi_destructor_decl : public extend_tree<tree_type_id::xi_destructor_decl, xi_method_decl> {
public:

    inline xi_destructor_decl(const char* name, ast_type* rtype, xi_parameter_decl* self_parameter, list<xi_parameter_decl>* parameters)
            : base_type(name, rtype, self_parameter, parameters) {
        //...
    }

};

struct xi_op_expr : public extend_tree<tree_type_id::xi_op_expr, ast_expr> {
public:

    inline xi_op_expr(ast_type* type, xi_operator op, list<ast_expr>* operands)
            : base_type(type),
              op(this, op),
              operands(this, operands) {
        //...
    }

    property<xi_operator>                           op;
    property<list<ast_expr>>                        operands;
};

struct xi_index_expr : public extend_tree<tree_type_id::xi_index_expr, ast_expr> {
public:

    inline xi_index_expr(ast_type* type, ast_expr* arrexpr, list<ast_expr>* idxexpr)
            : base_type(type),
              array_expr(this, arrexpr),
              index_expr_list(this, idxexpr) {
        //...
    }

    property<ast_expr>                              array_expr;
    property<list<ast_expr>>                        index_expr_list;
};

struct xi_zero_initializer_expr : public extend_tree<tree_type_id::xi_zero_initializer_expr, ast_expr> {
public:

    inline xi_zero_initializer_expr(ast_type* type, xi_type_decl* decl)
            : base_type(type),
              declaration(this, decl) {
        //...
    }

    property<xi_type_decl>                          declaration;

};

struct xi_named_memberref_expr : public extend_tree<tree_type_id::xi_named_memberref_expr, ast_expr> {
public:

    inline xi_named_memberref_expr(ast_expr* objexpr, const char* member_name)
            : base_type(nullptr),
              objexpr(this, objexpr),
              member_name(this, std::string(member_name)) {
        //...
    }

    property<ast_expr>                              objexpr;
    property<std::string>                           member_name;

};

struct xi_static_named_memberref_expr : public extend_tree<tree_type_id::xi_static_named_memberref_expr, ast_expr> {
public:

    inline xi_static_named_memberref_expr(ast_type* objtype, const char* member_name)
            : base_type(nullptr),
              objtype(this, objtype),
              member_name(this, std::string(member_name)) {
        //...
    }

    property<ast_type>                              objtype;
    property<std::string>                           member_name;
};

struct xi_function_group_expr : public extend_tree<tree_type_id::xi_function_group_expr, ast_expr> {
public:

    inline xi_function_group_expr()
            : base_type(nullptr),
              functions(this, new list<ast_decl>()) {
        //...
    }

    property<list<ast_decl>>                        functions;

};

struct xi_assign_stmt : public extend_tree<tree_type_id::xi_assign_stmt, ast_stmt> {
public:

    inline xi_assign_stmt(xi_operator op, ast_expr* lhs, ast_expr* rhs)
            : base_type(),
              op(this, op),
              lhs(this, lhs),
              rhs(this, rhs) {
        //...
    }

    property<xi_operator>                           op;
    property<ast_expr>                              lhs;
    property<ast_expr>                              rhs;

};

struct xi_foriter_stmt : public extend_tree<tree_type_id::xi_foriter_stmt, ast_stmt> {
public:

    inline xi_foriter_stmt(ast_local_decl* iterdecl, ast_expr* iterexpr, ast_stmt* body)
            : base_type(),
              iterdecl(this, iterdecl),
              iterexpr(this, iterexpr),
              body(this, body) {
        //...
    }

    property<ast_local_decl>                        iterdecl;
    property<ast_expr>                              iterexpr;
    property<ast_stmt>                              body;

};

std::string to_string(xcc::xi_operator op);



}

namespace std {
inline std::string to_string(xcc::xi_operator op) { return xcc::to_string(op); }
}



#endif /* XI_XI_TREE_HPP_ */
