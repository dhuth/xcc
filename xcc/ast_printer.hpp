/*
 * ast_printer.hpp
 *
 *  Created on: May 18, 2018
 *      Author: derick
 */

#ifndef XCC_AST_PRINTER_HPP_
#define XCC_AST_PRINTER_HPP_

#include "ast.hpp"

namespace xcc {

/**
 * A class for printing ast_tree nodes.
 */
struct ast_printer : public dispatch_visitor<void, std::ostream&> {
public:

    ast_printer();

    void handle_null_tree(std::ostream&) override final;

protected:

    typedef std::function<void(const char*, std::ostream&)>             pwfunc_t;

    template<typename T>
    inline pwfunc_t pwrap(reference<T>& p) {
        return [&](const char*, std::ostream& s) -> void { this->visit(p, s); };
    }

    template<typename T>
    inline pwfunc_t pwrap(reference<tree_list<T>>& p) {
        return [&](const char* fmt, std::ostream& s) -> void {
            for(uint32_t i = 0; i < p->size(); i++) {
                this->visit(p[i], s);
                if(i < (p->size()-1)) {
                    this->print(s, fmt);
                }
            }
        };
    }

    inline pwfunc_t pwrap(property<std::string>& p) {
        return [=](const char*, std::ostream& s) -> void { s << (std::string) p; };
    }

    template<typename T>
    inline pwfunc_t pwrap(property_value<T>& p) {
        return [=](const char*, std::ostream& s) -> void { s << std::to_string((T)p); };
    }

    inline pwfunc_t pwrap(property<llvm::APSInt>& p) {
        return [=](const char*, std::ostream& s) -> void { s << p->toString(10); };
    }

    inline pwfunc_t pwrap(property<llvm::APFloat>& p) {
        return [=](const char*, std::ostream& s) -> void { s << std::to_string(p->convertToDouble()); };
    }

    inline pwfunc_t pwrap(property<bool>& p) {
        return [=](const char*, std::ostream& s) -> void {
            if((bool) p) s << "false";
            else         s << "true";
        };
    }

    inline pwfunc_t pwrap(const char* svalue) {
        return [=](const char*, std::ostream& s) -> void {
            s << svalue;
        };
    }

    inline pwfunc_t pwrap(std::string str) {
        return [=](const char*, std::ostream& s) -> void {
            s << str;
        };
    }

public:

    template<typename... TTreeTypes>
    inline void print(std::ostream& s, const char* fmt, TTreeTypes&&... props) {
        std::vector<pwfunc_t> vec = { this->pwrap(std::forward<TTreeTypes>(props))... };
        this->formatted_print(fmt, s, vec);
    }

    template<typename TTreeType>
    inline void print(ptr<TTreeType> t, std::ostream& s) {
        this->print_internal(dynamic_cast<__tree_base*>(unbox(t)), s);
    }

protected:

    inline void print_internal(__tree_base* t, std::ostream& s) {
        this->_current_indent = 0;
        this->visit(t, s);
    }

    static std::string print_to_string(ast_tree* t);

private:

    uint32_t                                                            _current_indent;

    void formatted_print(const char* fmt, std::ostream& s, std::vector<pwfunc_t>& pfunc);

    void print_void_type(ast_void_type*, std::ostream&);
    void print_integer_type(ast_integer_type*, std::ostream&);
    void print_real_type(ast_real_type*, std::ostream&);
    void print_pointer_type(ast_pointer_type*, std::ostream&);
    void print_array_type(ast_array_type*, std::ostream&);
    void print_function_type(ast_function_type*, std::ostream&);
    void print_record_type(ast_record_type*, std::ostream&);

    void print_namespace_decl(ast_namespace_decl*, std::ostream&);
    void print_variable_decl(ast_variable_decl*, std::ostream&);
    void print_local_decl(ast_local_decl*, std::ostream&);
    void print_temp_decl(ast_temp_decl*, std::ostream&);
    void print_parameter_decl(ast_parameter_decl*, std::ostream&);
    void print_function_decl(ast_function_decl*, std::ostream&);

    void print_nop_stmt(ast_nop_stmt*, std::ostream&);
    void print_expr_stmt(ast_expr_stmt*, std::ostream&);
    void print_assign_stmt(ast_assign_stmt*, std::ostream&);
    void print_block_stmt(ast_block_stmt*, std::ostream&);
    void print_if_stmt(ast_if_stmt*, std::ostream&);
    void print_while_stmt(ast_while_stmt*, std::ostream&);
    void print_for_stmt(ast_for_stmt*, std::ostream&);
    void print_return_stmt(ast_return_stmt*, std::ostream&);
    void print_break_stmt(ast_break_stmt*, std::ostream&);
    void print_continue_stmt(ast_continue_stmt*, std::ostream&);

    void print_integer_expr(ast_integer*, std::ostream&);
    void print_real_expr(ast_real*, std::ostream&);
    void print_record_expr(ast_record*, std::ostream&);
    void print_array_expr(ast_array*, std::ostream&);
    void print_cast_expr(ast_cast*, std::ostream&);
    void print_binary_op_expr(ast_binary_op*, std::ostream&);
    void print_unary_op_expr(ast_unary_op*, std::ostream&);
    void print_index_expr(ast_index*, std::ostream&);
    void print_declref_expr(ast_declref*, std::ostream&);
    void print_memberref_expr(ast_memberref*, std::ostream&);
    void print_deref_expr(ast_deref*, std::ostream&);
    void print_addressof_expr(ast_addressof*, std::ostream&);
    void print_invoke_expr(ast_invoke*, std::ostream&);
    void print_stmt_expr(ast_stmt_expr*, std::ostream&);
};


}


#endif /* XCC_AST_PRINTER_HPP_ */
