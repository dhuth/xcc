/*
 * xi_lower_type.cpp
 *
 *  Created on: Mar 10, 2017
 *      Author: derick
 */
#include "xi_lower.hpp"


namespace xcc {

ast_type* xi_lower_walker::lower_const_type(xi_const_type* ct) {
    return ct->type;
}

#define FOR_EACH_FIELD(fvar, expr, doit)\
    for(auto m: expr) if(m != nullptr && m->is<xi_field_decl>() && (!(m->as<xi_field_decl>()->is_static))) {\
        auto fvar = m->as<xi_field_decl>();\
        doit;\
    }\

ast_type* xi_lower_walker::lower_object_type(xi_object_type* tp) {
    switch(tp->declaration->get_tree_type()) {
    case tree_type_id::xi_mixin_decl:
        break;
    case tree_type_id::xi_struct_decl:
        {
            auto sdecl = tp->declaration->as<xi_struct_decl>();
            if(!sdecl->instance_record_type) {
                auto recfields = box(new list<ast_type>());
                if(sdecl->supertype) {
                    FOR_EACH_FIELD(f, sdecl->supertype->members,recfields->append(f->type))
                }
                FOR_EACH_FIELD(f, sdecl->members, recfields->append(f->type))
                auto rectype = this->_ast_builder.get_record_type(recfields);
                sdecl->instance_record_type = rectype;
                return rectype;
            }
            else {
                return sdecl->instance_record_type;
            }
        }
        break;
    case tree_type_id::xi_class_decl:
        break;
    }

    throw std::runtime_error(std::string("unhandled declaration: ") + tp->declaration->get_tree_type_name() +
                             std::string(" in xi_lower_walker::lower_object_type\n"));
}

#undef FOR_EACH_FIELD


ast_type* xi_lower_walker::lower_array_type(xi_array_type* tp) {
    //TODO:
    ast_type* final_type = tp->element_type;
    for(auto dim: tp->dimensions) {
        auto folded_dim = this->_ast_builder.fold(dim);
        if(folded_dim->is<ast_integer>()) {
            final_type = this->_ast_builder.get_array_type(final_type, this->_ast_builder.foldu32(dim));
        }
        else {
            final_type = this->_ast_builder.get_pointer_type(final_type);
        }
    }
    return final_type;
    //throw std::runtime_error("Not implemented\n");
}

ast_type* xi_lower_walker::lower_ref_type(xi_ref_type* tp) {
    return this->_ast_builder.get_pointer_type(tp->element_type);
}


}
