/*
 * ast_printer.cpp
 *
 *  Created on: Feb 20, 2017
 *      Author: derick
 */


#include "ast_printer.hpp"

namespace xcc {

static void parse_index(const char* fmt, uint32_t& char_index, char** new_fmt, uint32_t& idx) {
    idx = 0;
    while(std::isdigit(fmt[char_index])) {
        idx = idx*10 + (fmt[char_index] - '0');
        char_index++;
    }
}

static void parse_list_format(const char* fmt, uint32_t& char_index, char** new_fmt, uint32_t& idx) {
    char_index++;
    parse_index(fmt, char_index, new_fmt, idx);
    assert(fmt[char_index] == ':');
    char_index++;
    uint32_t slen = 0;
    while(fmt[char_index + slen] != '}') {
        slen++;
    }

    *new_fmt = (char*) std::malloc(sizeof(char) * (slen + 1));
    (*new_fmt)[slen] = '\0';
    std::memcpy((void*)*new_fmt, (const void*)(&fmt[char_index]), slen);

    char_index += slen;
}

static void parse_format(const char* fmt, uint32_t& char_index, char** new_fmt, uint32_t& idx) {
    char_index++;
    if(fmt[char_index] == '{') {
        parse_list_format(fmt, char_index, new_fmt, idx);
    }
    else {
        parse_index(fmt, char_index, new_fmt, idx);
        *new_fmt = nullptr;
        char_index--;
    }
}

void ast_printer::formatted_print(const char* fmt, std::ostream& s, std::vector<pwfunc_t>& pfunc) {
    uint32_t char_index = 0;
    while(fmt[char_index] != '\0') {
        char c = fmt[char_index];
        switch(c) {
        case '%':
            char*       new_fmt;
            uint32_t    idx;

            parse_format(fmt, char_index, &new_fmt, idx);
            pfunc[idx](new_fmt, s);

            if(new_fmt != nullptr) {
                std::free(new_fmt);
            }
            break;
        case '\\':
            char_index++;
            c = fmt[char_index];
            switch(c) {
            case 'n':
                s << '\n' << std::string(2*this->_current_indent, ' ');
                break;
            case '>':
                this->_current_indent++;
                break;
            case '<':
                this->_current_indent--;
                break;
            default:
                s << c;
            }
            break;
        default:
            s << c;
        }
        char_index++;
    }
}

void ast_printer::print_void_type(ast_void_type*, std::ostream& s) {
    s << "void";
}

void ast_printer::print_integer_type(ast_integer_type* itype, std::ostream& s) {
    if(itype->is_unsigned) {
        s << "u" << (uint32_t) itype->bitwidth;
    }
    else {
        s << "i" << (uint32_t) itype->bitwidth;
    }
}

void ast_printer::print_real_type(ast_real_type* rtype, std::ostream& s) {
    s << "f" << (uint32_t) rtype->bitwidth;
}

void ast_printer::print_pointer_type(ast_pointer_type* ptype, std::ostream& s) {
    this->print(s, "%0*", ptype->element_type);
}

void ast_printer::print_array_type(ast_array_type* atype, std::ostream& s) {
    this->print(s, "%0[%1]", atype->element_type, atype->size);
}

void ast_printer::print_function_type(ast_function_type* ftype, std::ostream& s) {
    if(ftype->is_varargs) {
        this->print(s, "%0(%{1:, } ...)", ftype->return_type, ftype->parameter_types);
    }
    else {
        this->print(s, "%0(%{1:, })", ftype->return_type, ftype->parameter_types);
    }
}

void ast_printer::print_record_type(ast_record_type* rtype, std::ostream& s) {
    this->print(s, "type {%{0:, }}", rtype->field_types);
}

void ast_printer::print_namespace_decl(ast_namespace_decl* ns, std::ostream& s) {
    this->print(s, "namespace %0 {\\>\\n%{1:\\n}\\<\\n}", ns->name, ns->declarations);
}

void ast_printer::print_variable_decl(ast_variable_decl* var, std::ostream& s) {
    this->print(s, "global %0 %1[is_extern=%3, is_extern_visable=%4] = %2;", var->type, var->name, var->initial_value, var->is_extern, var->is_extern_visible);
}

void ast_printer::print_local_decl(ast_local_decl* local, std::ostream& s) {
    this->print(s, "local %0 %1 = %2;", local->type, local->name, local->init_value);
}

void ast_printer::print_temp_decl(ast_temp_decl* temp, std::ostream& s) {
    this->print(s, "temp %0 %1 = %2;", temp->type, temp->name, temp->value);
}

void ast_printer::print_parameter_decl(ast_parameter_decl* p, std::ostream& s) {
    this->print(s, "%0 %1", p->type, p->name);
}

void ast_printer::print_function_decl(ast_function_decl* func, std::ostream& s) {
    // print return type and name
    this->print(s, "%1 %0");

    // print args
    if(func->is_varargs) {
        this->print(s, "(%{0: ,} ...) ", func->parameters);
    }
    else {
        this->print(s, "(%{0: .}) ", func->parameters);
    }

    // print visibility
    if(func->is_extern)             s << "e";
    if(func->is_c_extern)           s << "c";
    if(func->is_extern_visible)     s << "v";

    // print body
    this->print(s, "%0", func->body);
}

void ast_printer::print_nop_stmt(ast_nop_stmt*, std::ostream& s) {
    this->print(s, ";");
}

void ast_printer::print_expr_stmt(ast_expr_stmt* stmt, std::ostream& s) {
    this->print(s, "%0;", stmt->expr);
}

void ast_printer::print_assign_stmt(ast_assign_stmt* stmt, std::ostream& s) {
    this->print(s, "%0 := %1;", stmt->lhs, stmt->rhs);
}

void ast_printer::print_block_stmt(ast_block_stmt* stmt, std::ostream& s) {
    this->print(s, "{\\>\\n%{0:;\\n}\\n%{1:\\n}\\<\\n}", stmt->decls, stmt->stmts);
}

void ast_printer::print_if_stmt(ast_if_stmt* stmt, std::ostream& s) {
    this->print(s, "if(%0):%1\\nelse:%2", stmt->condition, stmt->true_stmt, stmt->false_stmt);
}

void ast_printer::print_while_stmt(ast_while_stmt* stmt, std::ostream& s) {
    this->print(s, "while(%0):%1", stmt->condition, stmt->stmt);
}

void ast_printer::print_for_stmt(ast_for_stmt* stmt, std::ostream& s) {
    this->print(s, "for(%0;%1;%2):%3", stmt->init_stmt, stmt->condition, stmt->each_stmt, stmt->body);
}

void ast_printer::print_return_stmt(ast_return_stmt* stmt, std::ostream& s) {
    this->print(s, "return %0;", stmt->expr);
}

void ast_printer::print_continue_stmt(ast_continue_stmt*, std::ostream& s) {
    this->print(s, "continue;");
}

void ast_printer::print_break_stmt(ast_break_stmt*, std::ostream& s) {
    this->print(s, "break;");
}

void ast_printer::print_integer_expr(ast_integer* e, std::ostream& s) {
    this->print(s, "[%0] %1", e->type, e->value);
}

void ast_printer::print_real_expr(ast_real* e, std::ostream& s) {
    this->print(s, "[%0] %1", e->type, e->value);
}

void ast_printer::print_record_expr(ast_record* e, std::ostream& s) {
    this->print(s, "{%{0:, }}", e->values);
}

void ast_printer::print_array_expr(ast_array* e, std::ostream& s) {
    this->print(s, "{%{0:, }}", e->values);
}

void ast_printer::print_cast_expr(ast_cast* e, std::ostream& s) {
    this->print(s, "([%0] cast %1 %2)", e->type, e->op, e->expr);
}

void ast_printer::print_binary_op_expr(ast_binary_op* e, std::ostream& s) {
    this->print(s, "([%0] %1: %2, %3)", e->type, e->op, e->lhs, e->rhs);
}

void ast_printer::print_unary_op_expr(ast_unary_op* e, std::ostream& s) {
    this->print(s, "([%0] %1: %2)", e->type, e->op, e->expr);
}

void ast_printer::print_index_expr(ast_index* e, std::ostream& s) {
    this->print(s, "([%0] index: %1, %2)", e->type, e->arr_expr, e->index_expr);
}

void ast_printer::print_declref_expr(ast_declref* e, std::ostream& s) {
    this->print(s, "([%0] ref: %1)", e->type, e->declaration->name);
}

void ast_printer::print_memberref_expr(ast_memberref* e, std::ostream& s) {
    this->print(s, "([%0] member: %1, %2)", e->type, e->objexpr, e->member_index);
}

void ast_printer::print_deref_expr(ast_deref* e, std::ostream& s) {
    this->print(s, "([%0] deref: %1)", e->type, e->expr);
}

void ast_printer::print_addressof_expr(ast_addressof* e, std::ostream& s) {
    this->print(s, "([%0] addressof: %1)", e->type, e->expr);
}

void ast_printer::print_invoke_expr(ast_invoke* e, std::ostream& s) {
    this->print(s, "([%0] call: %1, (%{2:, }))", e->type, e->funcexpr, e->arguments);
}

void ast_printer::print_stmt_expr(ast_stmt_expr* e, std::ostream& s) {
    this->print(s, "([%0] %1 %2 ref: %3)", e->type, e->temp, e->statement, e->temp->name);
}

ast_printer::ast_printer()
        : dispatch_visitor<void, std::ostream&>(), _current_indent(0) {

    this->addmethod(&ast_printer::print_void_type);
    this->addmethod(&ast_printer::print_integer_type);
    this->addmethod(&ast_printer::print_real_type);
    this->addmethod(&ast_printer::print_pointer_type);
    this->addmethod(&ast_printer::print_array_type);
    this->addmethod(&ast_printer::print_function_type);
    this->addmethod(&ast_printer::print_record_type);

    this->addmethod(&ast_printer::print_namespace_decl);
    this->addmethod(&ast_printer::print_variable_decl);
    this->addmethod(&ast_printer::print_local_decl);
    this->addmethod(&ast_printer::print_temp_decl);
    this->addmethod(&ast_printer::print_parameter_decl);
    this->addmethod(&ast_printer::print_function_decl);

    this->addmethod(&ast_printer::print_nop_stmt);
    this->addmethod(&ast_printer::print_expr_stmt);
    this->addmethod(&ast_printer::print_assign_stmt);
    this->addmethod(&ast_printer::print_block_stmt);
    this->addmethod(&ast_printer::print_if_stmt);
    this->addmethod(&ast_printer::print_while_stmt);
    this->addmethod(&ast_printer::print_for_stmt);
    this->addmethod(&ast_printer::print_return_stmt);
    this->addmethod(&ast_printer::print_break_stmt);
    this->addmethod(&ast_printer::print_continue_stmt);

    this->addmethod(&ast_printer::print_integer_expr);
    this->addmethod(&ast_printer::print_real_expr);
    this->addmethod(&ast_printer::print_record_expr);
    this->addmethod(&ast_printer::print_array_expr);
    this->addmethod(&ast_printer::print_cast_expr);
    this->addmethod(&ast_printer::print_binary_op_expr);
    this->addmethod(&ast_printer::print_unary_op_expr);
    this->addmethod(&ast_printer::print_index_expr);
    this->addmethod(&ast_printer::print_declref_expr);
    this->addmethod(&ast_printer::print_memberref_expr);
    this->addmethod(&ast_printer::print_deref_expr);
    this->addmethod(&ast_printer::print_addressof_expr);
    this->addmethod(&ast_printer::print_invoke_expr);
    this->addmethod(&ast_printer::print_stmt_expr);
}

void ast_printer::handle_null_tree(std::ostream& s) {
    s << "NULL";
}


}
