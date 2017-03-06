/*
 * xi_builder.hpp
 *
 *  Created on: Feb 1, 2017
 *      Author: derick
 */

#ifndef XI_XI_BUILDER_HPP_
#define XI_XI_BUILDER_HPP_

#include <functional>
#include <unordered_map>
#include "frontend.hpp"
#include "xi_tree.hpp"
#include "ast_builder.hpp"

namespace xcc {

struct xi_lower_walker;
struct ircode_context;

struct xi_builder final : public ast_builder<> {
public:

    xi_builder(translation_unit&);
    virtual ~xi_builder() = default;

    xi_const_type*                                  get_const_type(ast_type*) const noexcept;
    xi_array_type*                                  get_array_type(ast_type*, list<ast_expr>*) const noexcept;
    xi_infered_type*                                get_infered_type() const noexcept;
    xi_ref_type*                                    get_ref_type(ast_type*) noexcept;
    xi_object_type*                                 get_object_type(xi_type_decl*) noexcept;
    ast_type*                                       get_declaration_type(ast_decl*) noexcept override final;

    xi_function_decl*                               define_global_function(ast_type*, const char*, list<xi_parameter_decl>*);
    xi_parameter_decl*                              define_parameter(ast_type*, const char*);
    xi_parameter_decl*                              define_parameter(ast_type*);

    xi_struct_decl*                                 define_global_struct(const char*, list<ast_type>*);
    xi_member_decl*                                 define_field(ast_type*, const char*);
    xi_member_decl*                                 define_field(ast_type*, const char*, ast_expr*);

    void                                            set_type_widens(ast_type*, ast_type*);

    xi_type_decl*                                   find_type_decl(const char*);
    //ast_type*                                       find_type(const char*);

    ast_expr*                                       make_op(xi_operator op, ast_expr*);
    ast_expr*                                       make_op(xi_operator op, ast_expr*, ast_expr*);
    ast_expr*                                       make_op(xi_operator op, list<ast_expr>*);
    ast_expr*                                       make_name_expr(const char*);
    ast_expr*                                       make_memberref_expr(ast_expr*, const char*);
    ast_expr*                                       make_cast_expr(ast_type*, ast_expr*) const override final;
    ast_expr*                                       make_index_expr(ast_expr*, list<ast_expr>*);
    ast_expr*                                       make_call_expr(ast_expr*, list<ast_expr>*) const override final;

    ast_stmt*                                       make_return_stmt(ast_type*, ast_expr*) const noexcept override final;
    ast_stmt*                                       make_assign_stmt(xi_operator, ast_expr*, ast_expr*) const noexcept;
    ast_stmt*                                       make_for_stmt(ast_local_decl*, ast_expr*, ast_stmt*) const noexcept;

    ast_expr*                                       widen(ast_type*, ast_expr*) const override final;
    ast_expr*                                       narrow(ast_type*, ast_expr*) const;

    void                                            push_function(xi_function_decl*);
    void                                            push_member(xi_struct_decl*);
    void                                            pop_function();

    void                                            generate();

    void                                            dump_parse();
    void                                            dump_parse(std::ostream& s);
    void                                            dump_unit();
    void                                            dump_unit(std::ostream& s);

public: //TODO: make private

    ast_expr*                                       lower(ast_expr*);
    ast_type*                                       lower(ast_type*);
    ast_decl*                                       lower(ast_decl*);
    ast_stmt*                                       lower(ast_stmt*);

    ast_parameter_decl*                             lower_parameter(xi_parameter_decl*);
    ast_function_decl*                              lower_function(xi_function_decl*);
    ast_record_decl*                                lower_struct(xi_struct_decl*);
    void                                            lower_body(xi_function_decl*);

public:

    //void                                            read_metadata_pass(ircode_context&, std::vector<std::string>&);
    void                                            flatten_pass();
    //void                                            write_metadata_pass(ircode_context&);
    void                                            lower_pass();

private:

    ptr<xi_infered_type>                                                _the_infered_type;

    std::vector<ptr<xi_function_decl>>                                  all_functions;
    std::vector<ptr<xi_type_decl>>                                      all_types;

    friend struct xi_lower_walker;

    xi_lower_walker*                                                    _lower_walker;

    std::map<ast_type*, ptr<xi_ref_type>>                               _all_reftypes;
    std::map<xi_type_decl*, ptr<xi_object_type>>                        _all_objecttypes;

    std::map<ast_type*, std::vector<ast_type*>>                         _type_rules_widens;

};

}



#endif /* XI_XI_BUILDER_HPP_ */
