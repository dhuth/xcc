#ifndef XI_INTERNAL_HPP_
#define XI_INTERNAL_HPP_

#include "xi_tree.hpp"
#include "xi_builder.hpp"

#include "location.hh"

#include <type_traits>

// Parser type defs

typedef xcc::ast_type*                                          type_t;
typedef xcc::ast_expr*                                          expr_t;
typedef xcc::ast_stmt*                                          stmt_t;
typedef xcc::ast_decl*                                          decl_t;
typedef xcc::xi_qname*                                          qname_t;
typedef xcc::xi_member_decl*                                    member_t;
typedef xcc::xi_function_decl*                                  function_decl_t;
typedef xcc::xi_method_decl*                                    method_decl_t;
typedef xcc::xi_struct_decl*                                    struct_decl_t;
typedef xcc::xi_parameter_decl*                                 parameter_t;

typedef xcc::list<xcc::ast_type>*                               type_list_t;
typedef xcc::list<xcc::ast_expr>*                               expr_list_t;
typedef xcc::list<xcc::ast_stmt>*                               stmt_list_t;
typedef xcc::list<xcc::ast_decl>*                               decl_list_t;
typedef xcc::list<xcc::xi_member_decl>*                         member_list_t;
typedef xcc::list<xcc::xi_parameter_decl>*                      parameter_list_t;

typedef xcc::xi_op_expr::xi_operator                            operator_t;

typedef xcc::xi_builder                                         xi_builder_t;

// Parser helper functions

template<typename Tptr, typename T = typename std::remove_pointer<Tptr>::type>
inline xcc::list<T>* make_list() {
    return new xcc::list<T>();
}

template<typename Tptr, typename T = typename std::remove_pointer<Tptr>::type>
inline xcc::list<T>* make_list(Tptr el) {
    return new xcc::list<T>(el);
}

template<typename Tptr, typename T = typename std::remove_pointer<Tptr>::type>
inline xcc::list<T>* make_list(xcc::list<T>* l, Tptr el) {
    l->push_back(el);
    return l;
}

template<typename Tptr, typename T = typename std::remove_pointer<Tptr>::type>
inline xcc::list<T>* make_list(Tptr lexpr, Tptr rexpr) {
    return make_list<Tptr>(make_list<Tptr>(lexpr), rexpr);
}

template<typename Tptr, typename T = typename std::remove_pointer<Tptr>::type>
inline xcc::list<T>* make_list(Tptr el, xcc::list<T>* l) {
    l->push_front(el);
    return l;
}

inline xcc::xi_qname* make_xi_qname(const char* name) {
    return new xcc::xi_qname(std::string(name));
}

inline xcc::xi_qname* make_xi_qname(xcc::xi_qname* qname, const char* name) {
    return new xcc::xi_qname(qname, std::string(name));
}

template<typename Tptr>
inline Tptr setloc(Tptr e, const xi::location& loc) {
    xcc::source_span span;
    span.first.line_number      = loc.begin.line;
    span.first.column_number    = loc.begin.column;
    span.last.line_number       = loc.end.line;
    span.last.column_number     = loc.end.column;

    if(loc.begin.filename != nullptr) {
        span.first.filename = *(loc.begin.filename);
    }
    if(loc.end.filename != nullptr) {
        span.last.filename = *(loc.end.filename);
    }

    return xcc::setloc(e, span);
}

template<typename Tptr>
inline Tptr setloc(Tptr e, const xi::location& begin, const xi::location& end) {
    xcc::source_span span;
    span.first.line_number      = begin.begin.line;
    span.first.column_number    = begin.begin.column;
    span.last.line_number       = end.end.line;
    span.last.column_number     = end.end.column;

    if(begin.begin.filename != nullptr) {
        span.first.filename = *(begin.begin.filename);
    }
    if(end.end.filename != nullptr) {
        span.last.filename = *(end.end.filename);
    }

    return xcc::setloc(e, span);
}

#endif
