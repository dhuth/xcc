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
#include "xi_mangler.hpp"

namespace xcc {

struct xi_lower_walker;
struct ircode_context;

struct member_search_parameters {

    inline member_search_parameters(const char* name, bool search_instance, bool search_static)
            : name(name),
              search_instance(search_instance),
              search_static(search_static),
              found(new list<xi_member_decl>()) {
        //...
    }

    std::string                                                         name;
    std::vector<xi_type_decl*>                                          searched;
    ptr<list<xi_member_decl>>                                           found;

    bool                                                                search_static;
    bool                                                                search_instance;
};

void find_members(xi_type_decl*, member_search_parameters&);
void find_members(xi_struct_decl*, member_search_parameters&);

struct xi_builder final : public ast_builder<itanium_cxxabi_mangler> {
public:

    typedef ast_builder<itanium_cxxabi_mangler>     base_builder_t;

    xi_builder(translation_unit&);
    virtual ~xi_builder() = default;

    xi_const_type*                                  get_const_type(ast_type*) noexcept;
    xi_array_type*                                  get_array_type(ast_type*, list<ast_expr>*) const noexcept;
    xi_infered_type*                                get_infered_type() const noexcept;
    xi_ref_type*                                    get_ref_type(ast_type*) noexcept;
    xi_object_type*                                 get_object_type(xi_type_decl*) noexcept;
    ast_type*                                       get_declaration_type(ast_decl*) noexcept override final;

    xi_function_decl*                               define_global_function(ast_type*, const char*, list<xi_parameter_decl>*);
    xi_parameter_decl*                              define_parameter(ast_type*, const char*);
    xi_parameter_decl*                              define_parameter(ast_type*);

    xi_struct_decl*                                 define_global_struct(const char*);
    xi_struct_decl*                                 define_global_struct(const char*, list<ast_type>*);
    xi_member_decl*                                 define_field(ast_type*, const char*, bool is_static);
    xi_member_decl*                                 define_field(ast_type*, const char*, ast_expr*, bool is_static);

    // parsing
    ast_decl*                                       find_member(ast_namespace_decl* decl, const char* name);
    ast_decl*                                       find_member(xi_type_decl* decl, const char* name);
    xi_type_decl*                                   find_type_decl(const char*);

    // for static checking
    ptr<list<xi_member_decl>>                       find_instance_members(xi_type_decl* decl, const char* name);
    ptr<list<xi_member_decl>>                       find_static_members(xi_type_decl* decl, const char* anem);

    ast_expr*                                       make_default_initializer(ast_type* tp);

    ast_expr*                                       make_op(xi_operator op, ast_expr*);
    ast_expr*                                       make_op(xi_operator op, ast_expr*, ast_expr*);
    ast_expr*                                       make_op(xi_operator op, list<ast_expr>*);
    ast_expr*                                       make_deref_expr(ast_expr*) const override final;
    ast_expr*                                       make_memberref_expr(ast_expr*, const char*);
    ast_expr*                                       make_memberref_expr(ast_type*, const char*);
    ast_expr*                                       make_declref_expr(ast_decl*);
    ast_expr*                                       make_cast_expr(ast_type*, ast_expr*) const override final;
    ast_expr*                                       make_index_expr(ast_expr*, list<ast_expr>*);
    ast_expr*                                       make_call_expr(ast_expr*, list<ast_expr>*) const override final;
    ast_expr*                                       make_ctor_expr(ast_type*, list<ast_expr>*);

    ast_stmt*                                       make_return_stmt(ast_type*, ast_expr*) const noexcept override final;
    ast_stmt*                                       make_assign_stmt(xi_operator, ast_expr*, ast_expr*) const noexcept;
    ast_stmt*                                       make_for_stmt(ast_local_decl*, ast_expr*, ast_stmt*) const noexcept;

    ast_expr*                                       make_instance_memberref_expr(xi_type_decl*, ast_expr*, const char* name, const source_span& span);
    ast_expr*                                       make_static_memberref_expr(xi_type_decl*, const char* name, const source_span& span);

    bool                                            widens(ast_type*, ast_type*) const override final;
    ast_expr*                                       widen(ast_type*, ast_expr*, uint32_t&) const;
    bool                                            sametype(ast_type*, ast_type*) const override final;
    ast_expr*                                       narrow(ast_type*, ast_expr*) const;

    inline bool                                     isrefof(ast_type*, ast_type*);

    ast_decl*                                       find_declaration(const char* name) noexcept final override;

    void                                            push_function_and_body(xi_function_decl*);
    void                                            push_function(xi_function_decl*);
    void                                            push_member(xi_struct_decl*);
    void                                            push_member(xi_class_decl*);
    void                                            push_member(xi_mixin_decl*);
    void                                            push_member(xi_method_decl*);
    void                                            push_member(xi_constructor_decl*);
    void                                            push_member(xi_destructor_decl*);
    void                                            pop_function_and_body();

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
    void                                            lower_body(xi_function_decl*);

public:

    void                                            lower_pass();

private:

    ptr<xi_infered_type>                                                _the_infered_type;

    std::vector<ptr<xi_function_decl>>                                  all_functions;
    std::vector<ptr<xi_type_decl>>                                      all_types;

    friend struct xi_lower_walker;

    xi_lower_walker*                                                    _lower_walker;

    std::map<ast_type*, ptr<xi_const_type>>                             _all_consttypes;
    std::map<ast_type*, ptr<xi_ref_type>>                               _all_reftypes;
    std::map<xi_type_decl*, ptr<xi_object_type>>                        _all_objecttypes;

};

bool xi_builder::isrefof(ast_type* t, ast_type* el) {
    return t->is<xi_ref_type>() && this->sametype(t->as<xi_ref_type>()->element_type, el);
}

bool same_function_by_signature(xi_builder& builder, xi_function_decl* lfunc, xi_function_decl* rfunc, bool check_return_type);

}



#endif /* XI_XI_BUILDER_HPP_ */
