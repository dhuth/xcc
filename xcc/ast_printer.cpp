/*
 * ast_printer.cpp
 *
 *  Created on: Feb 20, 2017
 *      Author: derick
 */


#include "ast.hpp"

namespace xcc {

ast_printer ast_printer::instance;

const char* to_string(ast_op op) {
    switch(op) {
    case ast_op::none:                      return "none";
    case ast_op::bitcast:                   return "bitcast";
    case ast_op::ineg:                      return "ineg";
    case ast_op::fneg:                      return "fneg";
    case ast_op::iadd:                      return "iadd";
    case ast_op::isub:                      return "isub";
    case ast_op::imul:                      return "imul";
    case ast_op::idiv:                      return "idiv";
    case ast_op::udiv:                      return "udiv";
    case ast_op::imod:                      return "imod";
    case ast_op::umod:                      return "umod";
    case ast_op::fadd:                      return "fadd";
    case ast_op::fsub:                      return "fsub";
    case ast_op::fmul:                      return "fmul";
    case ast_op::fdiv:                      return "fdiv";
    case ast_op::fmod:                      return "fmod";
    case ast_op::band:                      return "band";
    case ast_op::bor:                       return "bor";
    case ast_op::bxor:                      return "bxor";
    case ast_op::bnot:                      return "bnot";
    case ast_op::land:                      return "land";
    case ast_op::lor:                       return "lor";
    case ast_op::lnot:                      return "lnot";
    case ast_op::bshl:                      return "bshl";
    case ast_op::bshr:                      return "bshr";
    case ast_op::ashr:                      return "ashr";
    case ast_op::itof:                      return "itof";
    case ast_op::utof:                      return "utof";
    case ast_op::utop:                      return "utop";
    case ast_op::ftoi:                      return "ftoi";
    case ast_op::ftou:                      return "ftou";
    case ast_op::ptou:                      return "ptou";
    case ast_op::trunc:                     return "trunc";
    case ast_op::ftrunc:                    return "ftrunc";
    case ast_op::sext:                      return "sext";
    case ast_op::zext:                      return "zext";
    case ast_op::fext:                      return "fext";
    case ast_op::cmp_eq:                    return "cmp_eq";
    case ast_op::cmp_ne:                    return "cmp_ne";
    case ast_op::icmp_slt:                  return "icmp_slt";
    case ast_op::icmp_sle:                  return "icmp_sle";
    case ast_op::icmp_sgt:                  return "icmp_sgt";
    case ast_op::icmp_sge:                  return "icmp_sge";
    case ast_op::icmp_ult:                  return "icmp_ult";
    case ast_op::icmp_ule:                  return "icmp_ule";
    case ast_op::icmp_ugt:                  return "icmp_ugt";
    case ast_op::icmp_uge:                  return "icmp_uge";
    case ast_op::fcmp_true:                 return "fcmp_true";
    case ast_op::fcmp_false:                return "fcmp_false";
    case ast_op::fcmp_oeq:                  return "fcmp_oeq";
    case ast_op::fcmp_one:                  return "fcmp_one";
    case ast_op::fcmp_olt:                  return "fcmp_olt";
    case ast_op::fcmp_ole:                  return "fcmp_ole";
    case ast_op::fcmp_ogt:                  return "fcmp_ogt";
    case ast_op::fcmp_oge:                  return "fcmp_oge";
    case ast_op::fcmp_ueq:                  return "fcmp_ueq";
    case ast_op::fcmp_une:                  return "fcmp_une";
    case ast_op::fcmp_ult:                  return "fcmp_ult";
    case ast_op::fcmp_ule:                  return "fcmp_ule";
    case ast_op::fcmp_ugt:                  return "fcmp_ugt";
    case ast_op::fcmp_uge:                  return "fcmp_uge";
    case ast_op::__high_level:              return "__hight_level";
    case ast_op::add:                       return "add";
    case ast_op::sub:                       return "sub";
    case ast_op::mul:                       return "mul";
    case ast_op::div:                       return "div";
    case ast_op::mod:                       return "mod";
    case ast_op::neg:                       return "neg";
    case ast_op::shr:                       return "shr";
    case ast_op::shl:                       return "shl";
    case ast_op::eq:                        return "eq";
    case ast_op::ne:                        return "ne";
    case ast_op::lt:                        return "lt";
    case ast_op::le:                        return "le";
    case ast_op::gt:                        return "gt";
    case ast_op::ge:                        return "ge";
    case ast_op::binary_and:                return "binary_and";
    case ast_op::binary_or:                 return "binary_or";
    case ast_op::binary_xor:                return "binary_xor";
    case ast_op::binary_not:                return "binary_not";
    case ast_op::logical_and:               return "logical_and";
    case ast_op::logical_or:                return "logical_or";
    case ast_op::logical_not:               return "logical_not";
    }
    assert(false);
    return nullptr;
}

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

static void print_void_type(ast_void_type*, std::ostream& s) {
    s << "void";
}

static void print_integer_type(ast_integer_type* itype, std::ostream& s) {
    if(itype->is_unsigned) {
        s << "u" << (uint32_t) itype->bitwidth;
    }
    else {
        s << "i" << (uint32_t) itype->bitwidth;
    }
}

static void print_real_type(ast_real_type* rtype, std::ostream& s) {
    s << "f" << (uint32_t) rtype->bitwidth;
}

static void print_pointer_type(ast_pointer_type* ptype, std::ostream& s) {
    ast_printer::print(s, "%0*", ptype->element_type);
}

static void print_array_type(ast_array_type* atype, std::ostream& s) {
    ast_printer::print(s, "%0[%1]", atype->element_type, atype->size);
}

static void print_function_type(ast_function_type* ftype, std::ostream& s) {
    ast_printer::print(s, "%0(%{1:, })", ftype->return_type, ftype->parameter_types);
}

static void print_record_type(ast_record_type* rtype, std::ostream& s) {
    ast_printer::print(s, "type {%{0:, }}", rtype->field_types);
}

static void print_namespace_decl(ast_namespace_decl* ns, std::ostream& s) {
    ast_printer::print(s, "namespace %0 {\\>\\n%{1:\\n}\\<\\n}", ns->name, ns->declarations);
}

static void print_variable_decl(ast_variable_decl* var, std::ostream& s) {
    ast_printer::print(s, "global %0 %1[is_extern=%3, is_extern_visable=%4] = %2;", var->type, var->name, var->initial_value, var->is_extern, var->is_extern_visible);
}

static void print_local_decl(ast_local_decl* local, std::ostream& s) {
    ast_printer::print(s, "local %0 %1 = %2;", local->type, local->name, local->init_value);
}

static void print_temp_decl(ast_temp_decl* temp, std::ostream& s) {
    ast_printer::print(s, "temp %0 %1 = %2;", temp->type, temp->name, temp->value);
}

static void print_parameter_decl(ast_parameter_decl* p, std::ostream& s) {
    ast_printer::print(s, "%0 %1", p->type, p->name);
}

static void print_function_decl(ast_function_decl* func, std::ostream& s) {
    ast_printer::print(s, "%0 = function %1(%{2: ,}) [is_extern=%4, is_extern_visible=%5] %3",
            func->name,
            func->return_type,
            func->parameters,
            func->body,
            func->is_extern,
            func->is_extern_visible);
}

static void print_nop_stmt(ast_nop_stmt*, std::ostream& s) {
    ast_printer::print(s, ";");
}

static void print_expr_stmt(ast_expr_stmt* stmt, std::ostream& s) {
    ast_printer::print(s, "%0;", stmt->expr);
}

static void print_assign_stmt(ast_assign_stmt* stmt, std::ostream& s) {
    ast_printer::print(s, "%0 := %1;", stmt->lhs, stmt->rhs);
}

static void print_block_stmt(ast_block_stmt* stmt, std::ostream& s) {
    ast_printer::print(s, "{\\>\\n%{0:;\\n}\\n%{1:\\n}\\<\\n}", stmt->decls, stmt->stmts);
}

static void print_if_stmt(ast_if_stmt* stmt, std::ostream& s) {
    ast_printer::print(s, "if(%0):%1\\nelse:%2", stmt->condition, stmt->true_stmt, stmt->false_stmt);
}

static void print_while_stmt(ast_while_stmt* stmt, std::ostream& s) {
    ast_printer::print(s, "while(%0):%1", stmt->condition, stmt->stmt);
}

static void print_for_stmt(ast_for_stmt* stmt, std::ostream& s) {
    ast_printer::print(s, "for(%0;%1;%2):%3", stmt->init_stmt, stmt->condition, stmt->each_stmt, stmt->body);
}

static void print_return_stmt(ast_return_stmt* stmt, std::ostream& s) {
    ast_printer::print(s, "return %0;", stmt->expr);
}

static void print_continue_stmt(ast_continue_stmt*, std::ostream& s) {
    ast_printer::print(s, "continue;");
}

static void print_break_stmt(ast_break_stmt*, std::ostream& s) {
    ast_printer::print(s, "break;");
}

static void print_integer_expr(ast_integer* e, std::ostream& s) {
    ast_printer::print(s, "[%0] %1", e->type, e->value);
}

static void print_real_expr(ast_real* e, std::ostream& s) {
    ast_printer::print(s, "[%0] %1", e->type, e->value);
}

static void print_record_expr(ast_record* e, std::ostream& s) {
    ast_printer::print(s, "{%{0:, }}", e->values);
}

static void print_array_expr(ast_array* e, std::ostream& s) {
    ast_printer::print(s, "{%{0:, }}", e->values);
}

static void print_cast_expr(ast_cast* e, std::ostream& s) {
    ast_printer::print(s, "([%0] cast %1 %2)", e->type, e->op, e->expr);
}

static void print_binary_op_expr(ast_binary_op* e, std::ostream& s) {
    ast_printer::print(s, "([%0] %1: %2, %3)", e->type, e->op, e->lhs, e->rhs);
}

static void print_unary_op_expr(ast_unary_op* e, std::ostream& s) {
    ast_printer::print(s, "([%0] %1: %2)", e->type, e->op, e->expr);
}

static void print_index_expr(ast_index* e, std::ostream& s) {
    ast_printer::print(s, "([%0] index: %1, %2)", e->type, e->arr_expr, e->index_expr);
}

static void print_declref_expr(ast_declref* e, std::ostream& s) {
    ast_printer::print(s, "([%0] ref: %1)", e->type, e->declaration->name);
}

static void print_memberref_expr(ast_memberref* e, std::ostream& s) {
    ast_printer::print(s, "([%0] member: %1, %2)", e->type, e->objexpr, e->member_index);
}

static void print_deref_expr(ast_deref* e, std::ostream& s) {
    ast_printer::print(s, "([%0] deref: %1)", e->type, e->expr);
}

static void print_addressof_expr(ast_addressof* e, std::ostream& s) {
    ast_printer::print(s, "([%0] addressof: %1)", e->type, e->expr);
}

static void print_invoke_expr(ast_invoke* e, std::ostream& s) {
    ast_printer::print(s, "([%0] call: %1, (%{2:, }))", e->type, e->funcexpr, e->arguments);
}

static void print_stmt_expr(ast_stmt_expr* e, std::ostream& s) {
    ast_printer::print(s, "([%0] %1 %2 ref: %3)", e->type, e->temp, e->statement, e->temp->name);
}

ast_printer::ast_printer()
        : dispatch_visitor<void, std::ostream&>(), _current_indent(0) {

    ast_printer::add(&print_void_type);
    ast_printer::add(&print_integer_type);
    ast_printer::add(&print_real_type);
    ast_printer::add(&print_pointer_type);
    ast_printer::add(&print_array_type);
    ast_printer::add(&print_function_type);
    ast_printer::add(&print_record_type);

    ast_printer::add(&print_namespace_decl);
    ast_printer::add(&print_variable_decl);
    ast_printer::add(&print_local_decl);
    ast_printer::add(&print_temp_decl);
    ast_printer::add(&print_parameter_decl);
    ast_printer::add(&print_function_decl);

    ast_printer::add(&print_nop_stmt);
    ast_printer::add(&print_expr_stmt);
    ast_printer::add(&print_assign_stmt);
    ast_printer::add(&print_block_stmt);
    ast_printer::add(&print_if_stmt);
    ast_printer::add(&print_while_stmt);
    ast_printer::add(&print_for_stmt);
    ast_printer::add(&print_return_stmt);
    ast_printer::add(&print_break_stmt);
    ast_printer::add(&print_continue_stmt);

    ast_printer::add(&print_integer_expr);
    ast_printer::add(&print_real_expr);
    ast_printer::add(&print_record_expr);
    ast_printer::add(&print_array_expr);
    ast_printer::add(&print_cast_expr);
    ast_printer::add(&print_binary_op_expr);
    ast_printer::add(&print_unary_op_expr);
    ast_printer::add(&print_index_expr);
    ast_printer::add(&print_declref_expr);
    ast_printer::add(&print_memberref_expr);
    ast_printer::add(&print_deref_expr);
    ast_printer::add(&print_addressof_expr);
    ast_printer::add(&print_invoke_expr);
    ast_printer::add(&print_stmt_expr);
}

void ast_printer::handle_null_tree(std::ostream& s) {
    s << "NULL";
}


}
