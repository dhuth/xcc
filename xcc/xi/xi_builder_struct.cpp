/*
 * xi_builder_struct.cpp
 *
 *  Created on: Feb 26, 2017
 *      Author: derick
 */

#include <algorithm>
#include "xi_builder.hpp"
#include "xi_builder_struct.hpp"
#include "xi_pass_finalize_types.hpp"

namespace xcc {

xi_struct_context::xi_struct_context(ast_context* parent, xi_struct_decl* decl)
        : ast_context(parent),
          _struct(decl) {
    //...
}

void xi_struct_context::insert(const char* name, ast_decl* decl) {
    assert(decl->is<xi_member_decl>());
    auto member = decl->as<xi_member_decl>();
    member->parent_decl = this->_struct;
    this->_struct->members->append(member);
}

ast_type* xi_struct_context::get_return_type() {
    assert(false);
    return nullptr;
}

ptr<ast_decl> xi_struct_context::find_first_impl(const char* name) {
    for(auto el: this->_struct->members) {
        std::string elname = el->name;
        if(elname == name) {
            return el;
        }
    }
    return nullptr;
}

void xi_struct_context::find_all_impl(ptr<list<ast_decl>> plist, const char* name) {
    for(auto el: this->_struct->members) {
        std::string elname = el->name;
        if(elname == name) {
            plist->append(el);
        }
    }
}

xi_struct_decl* xi_builder::define_global_struct(const char* name) {
    auto decl = this->context->find_of<xi_struct_decl>(name, false);
    if(unbox(decl) == nullptr) {
        auto new_decl = box(new xi_struct_decl(name));
        this->all_types.push_back(new_decl->as<xi_type_decl>());
        this->context->insert(name, new_decl);
        return new_decl;
    }
    else {
        assert(decl->basetypes->size() == 0); //TODO: this should be a parse-type exception
        //...
    }
    return decl;
}

xi_struct_decl* xi_builder::define_global_struct(const char* name, list<ast_type>* basetypes) {
    auto decl = this->context->find_of<xi_struct_decl>(name, false);
    if(unbox(decl) == nullptr) {
        auto new_decl = box(new xi_struct_decl(name, basetypes));
        this->all_types.push_back(new_decl->as<xi_type_decl>());
        this->context->insert(name, new_decl);
        return new_decl;
    }
    else {
        assert(decl->basetypes->size() == 0); // TODO: this should be a parse-type exception.
        decl->basetypes = basetypes;
    }
    return decl;
}

xi_member_decl* xi_builder::define_field(ast_type* tp, const char* name, bool is_static) {
    auto newfield = new xi_field_decl(name, tp, nullptr, is_static);
    this->context->insert(name, newfield);
    return newfield;
}

xi_member_decl* xi_builder::define_field(ast_type* tp, const char* name, ast_expr* init_expr, bool is_static) {
    auto newfield = new xi_field_decl(name, tp, init_expr, is_static);
    this->context->insert(name, newfield);
    return newfield;
}

void xi_builder::push_member(xi_struct_decl* decl) {
    this->push_context<xi_struct_context>(decl);
}

//TODO: this belongs in a source file for class types
void xi_builder::push_member(xi_class_decl* decl) {
    throw std::runtime_error("Not implemented");
}

//TODO: this belongs in a source file for mixin types
void xi_builder::push_member(xi_mixin_decl* decl) {
    throw std::runtime_error("Not implemented");
}

//TODO: these belong in a source file for methods
void xi_builder::push_member(xi_method_decl* decl) {
    throw std::runtime_error("Not implemented");
}

void xi_builder::push_member(xi_constructor_decl* decl) {
    throw std::runtime_error("Not implemented");
}

void xi_builder::push_member(xi_destructor_decl* decl) {
    throw std::runtime_error("Not implemented");
}

void xi_finalize_types_pass::finalize_struct(xi_struct_decl* decl) {
    // -------------------------
    // 1. Set supertype & mixins
    // -------------------------

    for(auto tp: decl->basetypes) {
        assert(tp->is<xi_object_type>()); //TODO: is error, not assert
        xi_type_decl* tp_decl = tp->as<xi_object_type>()->declaration;
        if(tp_decl->is<xi_mixin_decl>()) {
            if(decl->mixins->find(tp_decl) == decl->mixins->end()) {
                decl->mixins->append(this->visit(tp_decl)->as<ast_decl>());
            }
        }
        else if(tp_decl->is<xi_struct_decl>()) {
            assert(decl->supertype == nullptr); //TODO: is an error, not assert
            decl->supertype = this->visit(tp_decl)->as<xi_type_decl>();
        }
    }

    // --------------------------------
    // 2. Generate instance record type
    // --------------------------------

    uint32_t instance_idx = 0;
    ptr<list<ast_type>> instance_record_type_list = new list<ast_type>();
    if(decl->supertype != nullptr) {
        instance_idx = decl->supertype->instance_field_count;
        for(auto tp: decl->supertype->as<xi_struct_decl>()->instance_record_type->field_types) {
            instance_record_type_list->append(tp);
        }
    }
    for(auto m: decl->members) {
        if(m->is<xi_field_decl>() && !(m->as<xi_field_decl>()->is_static)) {
            auto f = m->as<xi_field_decl>();
            f->field_index = instance_idx;
            instance_record_type_list->append(f->type);
            instance_idx++;
        }
    }
    decl->instance_field_count = instance_idx;
    decl->instance_record_type = this->builder.get_record_type(instance_record_type_list);

    // -------------------------
    // 3. Organize static fields
    // -------------------------

    uint32_t static_idx = 0;
    ptr<list<ast_type>> static_record_type_list = new list<ast_type>();
    if(decl->supertype != nullptr) {
        //TODO: ???
    }
    for(auto m: decl->members) {
        if(m->is<xi_field_decl>() && m->as<xi_field_decl>()->is_static) {
            auto f = m->as<xi_field_decl>();
            f->field_index = static_idx;
            static_record_type_list->append(f->type);
            static_idx++;
        }
    }
    decl->static_field_count = static_idx;
    decl->static_record_type = this->builder.get_record_type(static_record_type_list);

    // -------------------------------------------
    // 4. TODO: check for constructor / destructor
    // -------------------------------------------

    //...
}


}

