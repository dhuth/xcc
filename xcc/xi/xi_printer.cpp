/*
 * xi_printer.cpp
 *
 *  Created on: Jan 23, 2018
 *      Author: derick
 */

#include "xi_tree.hpp"

namespace xcc {

static void print_xi_qname(xi_qname* q, std::ostream& s) {
    s << q->names[0];
    for(size_t i = 1; i < q->names->size(); i++) {
        s << "::" << q->names[i];
    }
}

static void print_xi_auto_type(xi_auto_type*, std::ostream& s) {
    s << "auto";
}

static void print_xi_reference_type(xi_reference_type* t, std::ostream& s) {
    ast_printer::print(s, "%0&", t->type);
}

static void print_xi_object_type(xi_object_type* t, std::ostream& s) {
    ast_printer::print(s, "%0", t->declaration->name);
}

static void print_xi_tuple_type(xi_tuple_type* t, std::ostream& s) {
    ast_printer::print(s, "(%{0:, })", t->types);
}


static void print_xi_parameter_decl(xi_parameter_decl* p, std::ostream& s) {
    ast_printer::print(s, "%0: %1", p->name, p->type);
}

static void print_xi_function_decl(xi_function_decl* f, std::ostream& s) {
    ast_printer::print(s, "func %0(%{1:, })->%2 %3", f->name, f->parameters, f->return_type, f->body);
}

static void print_xi_struct_decl(xi_struct_decl* d, std::ostream& s) {
    ast_printer::print(s, "struct %0 : %{1:, } {\\>\\n%{2:\\n}\\<\\n}", d->name, d->base_types, d->members);
}

static void print_xi_field_decl(xi_field_decl* f, std::ostream& s) {
    ast_printer::print(s, "%0: %1;", f->name, f->type);
}



static void print_xi_id_expr(xi_id_expr* e, std::ostream& s) {
    ast_printer::print(s, "([%0] %1)", e->type, e->name);
}

static void print_xi_op_expr(xi_op_expr* e, std::ostream& s) {
    auto opstr = std::to_string((xi_op_expr::xi_operator) e->op);
    ast_printer::print(s, "([%0] op%1 %{2:, })", e->type, opstr, e->operands);
}

void setup_xi_printer() {
    ast_printer::add(print_xi_qname);
    ast_printer::add(print_xi_auto_type);
    ast_printer::add(print_xi_reference_type);
    ast_printer::add(print_xi_object_type);
    ast_printer::add(print_xi_tuple_type);

    ast_printer::add(print_xi_parameter_decl);
    ast_printer::add(print_xi_function_decl);
    ast_printer::add(print_xi_struct_decl);
    ast_printer::add(print_xi_field_decl);

    ast_printer::add(print_xi_id_expr);
    ast_printer::add(print_xi_op_expr);
}

}

