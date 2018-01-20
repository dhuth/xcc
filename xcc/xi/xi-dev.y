%glr-parser
%skeleton                                           "glr.cc"
%defines                                            "xi-parser.hpp"
%define     api.prefix                              {xi}
%define     parse.trace
%define     parse.error                             verbose

%locations

%code requires {
#include    "xi_internal.hpp"
#include    "source.hpp"
#define     YYSTYPE                         XISTYPE
#define     YYLTYPE                         XILTYPE
#define     YY_DECL                         int yylex(YYSTYPE* yylval_param, XILTYPE* yyloc, xi_builder_t& builder)

#define     YYLTYPE_IS_DECLARED             1
/*
#define     YYLLOC_DEFAULT(current, rhs, n)             \
    do                                                  \
        if(n) {                                         \
            (current).first = YYRHSLOC(rhs, 1).first;   \
            (current).last  = YYRHSLOC(rhs, n).last;    \
        }                                               \
        else {                                          \
            (current).first = YYRHSLOC(rhs, 0).first;   \
            (current).last  = YYRHSLOC(rhs, 0).last;    \
        }                                               \
   while(0)
*/
#define SETLOC(e,n)                          xcc::setloc(e, xcc::source_span($##n))
}

%token
        TOK_EOF                         0   "end of file"
        TOK_IDENTIFIER                      "identifier token"
        TOK_TYPE                            "type token"
        TOK_EXPR                            "basic expression token"
        TOK_DECL                            "declaration token"
        
        LITERAL_INTEGER                     "integer value"
        LITERAL_REAL                        "real value"
        LITERAL_STRING                      "string value"
        LITERAL_CHARACTER                   "character value"
        LITERAL_BOOL                        "boolean value"
        
        OP_LBRACE                           "{"
        OP_RBRACE                           "}"
        OP_LBRACKET                         "["
        OP_RBRACKET                         "]"
        OP_LPAREN                           "("
        OP_RPAREN                           ")"
        OP_DOT                              "."
        OP_ARROW                            "->"
        OP_COMA                             ","
        OP_COLON                            ":"
        OP_DOUBLE_COLON                     "::"
        OP_SEMICOLON                        ";"
        
        OP_LOGICAL_AND                      "and"
        OP_LOGICAL_OR                       "or"
        OP_LOGICAL_XOR                      "xor"
        OP_LOGICAL_NOT                      "not"
        
        OP_ADD                              "+"
        OP_SUB                              "-"
        OP_MUL                              "*"
        OP_DIV                              "/"
        OP_MOD                              "%"
        
        OP_SL                               "sl"
        OP_SR                               "sr"
        OP_BINARY_AND                       "&"
        OP_BINARY_OR                        "|"
        OP_BINARY_XOR                       "^"
        OP_BINARY_NOT                       "~"
        
        OP_LT                               "<"
        OP_LE                               "<="
        OP_GT                               ">"
        OP_GE                               ">="
        OP_EQ                               "=="
        OP_NE                               "!="
        
        OP_ASSIGN                           "="
        OP_ASSIGN_ADD                       "+="
        OP_ASSIGN_SUB                       "-="
        OP_ASSIGN_MUL                       "*="
        OP_ASSIGN_DIV                       "/="
        OP_ASSIGN_MOD                       "%="
        OP_ASSIGN_SL                        "sl="
        OP_ASSIGN_SR                        "sr="
        OP_ASSIGN_BINARY_AND                "&="
        OP_ASSIGN_BINARY_OR                 "|="
        OP_ASSIGN_BINARY_XOR                "^="
        OP_ASSIGN_LOGICAL_AND               "and="
        OP_ASSIGN_LOGICAL_OR                "or="
        OP_ASSIGN_LOGICAL_XOR               "xor="
        
        KW_CONST                            "const"
        KW_FUNC                             "func"
        KW_NAMESPACE                        "namespace"
        
        KW_BREAK                            "break"
        KW_CONTINUE                         "continue"
        KW_ELSE                             "else"
        KW_ELIF                             "elif"
        KW_FOR                              "for"
        KW_IF                               "if"
        KW_RETURN                           "return"
        KW_WHILE                            "while"
        KW_VAR                              "var"
;

%union {
    expr_t                                  expr;
    type_t                                  type;
    stmt_t                                  stmt;
    decl_t                                  decl;
    parameter_t                             parameter;
    
    expr_list_t                             expr_list;
    type_list_t                             type_list;
    stmt_list_t                             stmt_list;
    decl_list_t                             decl_list;
    parameter_list_t                        parameter_list;
    
    operator_t                              op;
    
    char*                                   text;
}

%type <decl_list>                           global-decl-list-opt
%type <decl_list>                           global-decl-list
%type <decl>                                global-decl
%type <decl>                                namespace-decl
%type <decl>                                namespace-decl-cont
%type <decl>                                global-function-decl
%type <parameter_list>                      global-parameter-decl-list-opt
%type <parameter_list>                      global-parameter-decl-list
%type <parameter>                           global-parameter-decl

%type <type>                                type
%type <type>                                TOK_TYPE
%type <type>                                postfix-type
%type <type>                                prefix-type
%type <type>                                non-const-prefix-type
%type <type>                                term-type
%type <type>                                global-function-return-type-decl
%type <type_list>                           type-list-opt
%type <type_list>                           type-list

%type <expr>                                expr
%type <expr>                                assign-expr
%type <expr>                                assign-expr-only
%type <expr>                                logical-or-expr
%type <expr>                                logical-and-expr
%type <expr>                                binary-or-expr
%type <expr>                                binary-and-expr
%type <expr>                                eq-expr
%type <expr>                                comp-expr
%type <expr>                                add-expr
%type <expr>                                mul-expr
%type <expr>                                prefix-expr
%type <expr>                                postfix-expr
%type <expr>                                term-expr
%type <expr_list>                           expr-list-opt
%type <expr_list>                           expr-list

%type <expr>                                TOK_EXPR
%type <expr>                                LITERAL_INTEGER
%type <expr>                                LITERAL_REAL
%type <expr>                                LITERAL_BOOL

%type <text>                                TOK_IDENTIFIER

%type <op>                                  OP_ADD
%type <op>                                  OP_SUB
%type <op>                                  OP_MUL
%type <op>                                  OP_DIV
%type <op>                                  OP_MOD
%type <op>                                  OP_SL
%type <op>                                  OP_SR
%type <op>                                  OP_BINARY_AND
%type <op>                                  OP_BINARY_OR
%type <op>                                  OP_BINARY_XOR
%type <op>                                  OP_BINARY_NOT
%type <op>                                  OP_LOGICAL_AND
%type <op>                                  OP_LOGICAL_OR
%type <op>                                  OP_LOGICAL_XOR
%type <op>                                  OP_LOGICAL_NOT
%type <op>                                  OP_LT
%type <op>                                  OP_LE
%type <op>                                  OP_GT
%type <op>                                  OP_GE
%type <op>                                  OP_EQ
%type <op>                                  OP_NE
%type <op>                                  OP_ASSIGN
%type <op>                                  OP_ASSIGN_ADD
%type <op>                                  OP_ASSIGN_SUB
%type <op>                                  OP_ASSIGN_MUL
%type <op>                                  OP_ASSIGN_DIV
%type <op>                                  OP_ASSIGN_MOD
%type <op>                                  OP_ASSIGN_SL
%type <op>                                  OP_ASSIGN_SR
%type <op>                                  OP_ASSIGN_BINARY_AND
%type <op>                                  OP_ASSIGN_BINARY_OR
%type <op>                                  OP_ASSIGN_BINARY_XOR
%type <op>                                  OP_ASSIGN_LOGICAL_AND
%type <op>                                  OP_ASSIGN_LOGICAL_OR
%type <op>                                  OP_ASSIGN_LOGICAL_XOR

%type <op>                                  assign-op

%type <stmt>                                block-stmt
%type <decl>                                var-decl-stmt
%type <stmt>                                non-decl-stmt
%type <stmt>                                expr-stmt
%type <stmt>                                if-stmt
%type <stmt>                                else-stmt
%type <stmt>                                elif-stmt
%type <stmt>                                s-body-stmt
%type <stmt>                                while-stmt
%type <stmt>                                break-stmt
%type <stmt>                                continue-stmt
%type <stmt>                                return-stmt

%type <stmt_list>                           stmt-list-opt
%type <stmt_list>                           stmt-list

%code {
extern YY_DECL;
void yyerror(XILTYPE* loc, xi::parser&, xi_builder_t& builder, const char* msg);

}
%param                              {xi_builder_t&          builder}

%start  translation-unit
%%

/* ---------------- *
 * Translation Unit *
 * ---------------- */

translation-unit
                        : global-decl-list-opt                                  {
                                                                                    for(auto d: $1) {
                                                                                        builder.insert_global(d);
                                                                                    }
                                                                                }
                        ;
global-decl-list-opt
                        : global-decl-list                                      { $$ = $1; }
                        | %empty                                                { $$ = make_list<decl_t>(); }
                        ;
global-decl-list
                        : global-decl-list global-decl                          { $$ = make_list<decl_t>($1, $2); }
                        | global-decl                                           { $$ = make_list<decl_t>($1); }
                        ;
global-decl
                        : namespace-decl                                        { $$ = $1; }
                        | global-function-decl                                  { $$ = $1; }
                        ;

/* ------------------- *
 * Global Declarations *
 * ------------------- */

namespace-decl
                        : KW_NAMESPACE namespace-decl-cont                      { $$ = $2; }
                        ;
namespace-decl-cont
                        : TOK_IDENTIFIER
                            OP_LBRACE global-decl-list-opt OP_RBRACE            { $$ = builder.make_namespace_decl($1, $3); }
                        | TOK_IDENTIFIER
                            OP_DOUBLE_COLON namespace-decl-cont                 { $$ = builder.make_namespace_decl($1, make_list<decl_t>($3)); }
                        ;
global-function-decl
                        :           /* attributes */
                          KW_FUNC
                            TOK_IDENTIFIER
                                    /* generics */
                            OP_LPAREN global-parameter-decl-list-opt OP_RPAREN
                            global-function-return-type-decl
                            block-stmt                                          { $$ = builder.make_xi_function_decl($2, $6, $4, $7); }
                        ;
global-parameter-decl-list-opt
                        : global-parameter-decl-list                            { $$ = $1; }
                        | %empty                                                { $$ = make_list<parameter_t>(); }
                        ;
global-parameter-decl-list
                        : global-parameter-decl-list
                            OP_COMA global-parameter-decl                       { $$ = make_list<parameter_t>($1, $3); }
                        | global-parameter-decl                                 { $$ = make_list<parameter_t>($1); }
                        ;
global-parameter-decl
                        : TOK_IDENTIFIER OP_COLON type                          { $$ = builder.make_xi_parameter_decl($1,      $3); }
                        |                OP_COLON type                          { $$ = builder.make_xi_parameter_decl(nullptr, $2); }
                        ;
global-function-return-type-decl
                        : OP_ARROW type                                         { $$ = $2; }
                        | %empty                                                { $$ = builder.get_void_type(); }
                        ;

/* ---------- *
 * Statements *
 * ---------- */

block-stmt
                        : OP_LBRACE stmt-list-opt OP_RBRACE                     { $$ = builder.make_block_stmt($2); }
                        ;
stmt-list-opt
                        : stmt-list                                             { $$ = $1; }
                        | %empty                                                { $$ = make_list<stmt_t>(); }
                        ;
stmt-list
                        : var-decl-stmt stmt-list-opt                           { $$ = make_list<stmt_t>(builder.make_block_stmt($1->as<xcc::ast_local_decl>(), $2)); }
                        | non-decl-stmt stmt-list-opt                           { $$ = make_list<stmt_t>($1, $2); }
                        ;
var-decl-stmt
                        : KW_VAR                                        // var <id> : <type> ;
                            TOK_IDENTIFIER OP_COLON type
                          OP_SEMICOLON                                          { $$ = builder.make_local_decl($2, $4, nullptr); }
                        | KW_VAR                                        // var <id> : <type> = <expr> ;
                            TOK_IDENTIFIER OP_COLON type OP_ASSIGN expr
                          OP_SEMICOLON                                          { $$ = builder.make_local_decl($2, $4, $6); }
                        | KW_VAR                                        // var <id> = <expr> ;
                            TOK_IDENTIFIER OP_ASSIGN expr
                          OP_SEMICOLON                                          { $$ = builder.make_local_decl($2, builder.get_auto_type(), $4); }
                        ;
non-decl-stmt
                        : block-stmt                                            { $$ = $1; }
                        | expr-stmt                                             { $$ = $1; }
                        | if-stmt                                               { $$ = $1; }
                        | while-stmt                                            { $$ = $1; }
                        | break-stmt                                            { $$ = $1; }
                        | continue-stmt                                         { $$ = $1; }
                        | return-stmt                                           { $$ = $1; }
                        ;
if-stmt
                        : KW_IF expr s-body-stmt                                { $$ = builder.make_xi_if_stmt($2, $3, nullptr); }
                        | KW_IF expr s-body-stmt else-stmt                      { $$ = builder.make_xi_if_stmt($2, $3, $4); }
                        | KW_IF expr s-body-stmt elif-stmt                      { $$ = builder.make_xi_if_stmt($2, $3, $4); }
                        ;
else-stmt
                        : KW_ELSE s-body-stmt                                   { $$ = $2; }
                        ;
elif-stmt
                        : KW_ELIF expr s-body-stmt                              { $$ = builder.make_xi_if_stmt($2, $3, nullptr); }
                        | KW_ELIF expr s-body-stmt else-stmt                    { $$ = builder.make_xi_if_stmt($2, $3, $4); }
                        | KW_ELIF expr s-body-stmt elif-stmt                    { $$ = builder.make_xi_if_stmt($2, $3, $4); }
                        ;
while-stmt
                        : KW_WHILE expr s-body-stmt                             { $$ = builder.make_xi_while_stmt($2, $3); }
                        ;
s-body-stmt
                        :  block-stmt                                           { $$ = $1; }
                        ;
expr-stmt
                        : assign-expr OP_SEMICOLON                              { $$ = builder.make_expr_stmt($1); }
                        ;
break-stmt
                        : KW_BREAK OP_SEMICOLON                                 { $$ = builder.make_break_stmt(); }
                        ;
return-stmt
                        : KW_RETURN expr OP_SEMICOLON                           { $$ = builder.make_xi_return_stmt($2); }
                        | KW_RETURN expr OP_COMA expr-list-opt OP_SEMICOLON     { $$ = builder.make_xi_return_stmt(builder.make_xi_tuple_expr(make_list<expr_t>($2, $4))); }
                        | KW_RETURN OP_SEMICOLON                                { $$ = builder.make_xi_return_stmt(nullptr); }
                        ;
continue-stmt
                        : KW_CONTINUE OP_SEMICOLON                              { $$ = builder.make_continue_stmt(); }
                        ;

/* ----------- *
 * Expressions *
 * ----------- */

expr-list-opt
                        : expr-list                                             { $$ = $1; }
                        | %empty                                                { $$ = make_list<expr_t>(); }
                        ;
expr-list
                        : expr-list OP_COMA expr                                { $$ = make_list<expr_t>($1, $3); }
                        | expr                                                  { $$ = make_list<expr_t>($1); }
                        ;

assign-expr-only        : expr assign-op expr                                   { $$ = builder.make_xi_op($2, $1, $3); }
                        ;

assign-expr             : expr assign-op expr                                   { $$ = builder.make_xi_op($2, $1, $3); }
                        | expr                                                  { $$ = $1; }
                        ;

expr
                        : logical-or-expr                                       { $$ = $1; }
                        ;
logical-or-expr
                        : logical-or-expr OP_LOGICAL_OR logical-and-expr        { $$ = builder.make_xi_op($2, $1, $3); }
                        | logical-and-expr                                      { $$ = $1; }
                        ;
logical-and-expr
                        : logical-and-expr OP_LOGICAL_AND binary-or-expr        { $$ = builder.make_xi_op($2, $1, $3); }
                        | binary-or-expr                                        { $$ = $1; }
                        ;
binary-or-expr
                        : binary-or-expr OP_BINARY_OR binary-and-expr           { $$ = builder.make_xi_op($2, $1, $3); }
                        | binary-or-expr OP_BINARY_XOR binary-and-expr          { $$ = builder.make_xi_op($2, $1, $3); }
                        | binary-and-expr                                       { $$ = $1; }
                        ;
binary-and-expr
                        : binary-and-expr OP_BINARY_AND eq-expr                 { $$ = builder.make_xi_op($2, $1, $3); }
                        | eq-expr                                               { $$ = $1; }
                        ;
eq-expr
                        : comp-expr OP_EQ comp-expr                             { $$ = builder.make_xi_op($2, $1, $3); }
                        | comp-expr OP_NE comp-expr                             { $$ = builder.make_xi_op($2, $1, $3); }
                        | comp-expr                                             { $$ = $1; }
                        ;
comp-expr
                        : add-expr OP_LT add-expr                               { $$ = builder.make_xi_op($2, $1, $3); }
                        | add-expr OP_LE add-expr                               { $$ = builder.make_xi_op($2, $1, $3); }
                        | add-expr OP_GT add-expr                               { $$ = builder.make_xi_op($2, $1, $3); }
                        | add-expr                                              { $$ = $1; }
                        ;
add-expr
                        : add-expr OP_ADD mul-expr                              { $$ = builder.make_xi_op($2, $1, $3); }
                        | add-expr OP_SUB mul-expr                              { $$ = builder.make_xi_op($2, $1, $3); }
                        | mul-expr                                              { $$ = $1; }
                        ;
mul-expr
                        : mul-expr OP_MUL postfix-expr                          { $$ = builder.make_xi_op($2, $1, $3); }
                        | mul-expr OP_DIV postfix-expr                          { $$ = builder.make_xi_op($2, $1, $3); }
                        | mul-expr OP_MOD postfix-expr                          { $$ = builder.make_xi_op($2, $1, $3); }
                        | postfix-expr                                          { $$ = $1; }
                        ;
postfix-expr
                        : postfix-expr
                            OP_LPAREN   expr-list-opt OP_RPAREN                 { $$ = builder.make_xi_op(operator_t::__invoke__, $3); }
                        | postfix-expr
                            OP_LBRACKET expr-list     OP_RBRACKET               { $$ = builder.make_xi_op(operator_t::__index__, $3); }
                        | postfix-expr OP_DOT   TOK_IDENTIFIER                  { $$ = builder.make_xi_member_id_expr($1, $3); }
                        | postfix-expr OP_ARROW TOK_IDENTIFIER                  { $$ = builder.make_xi_deref_member_id_expr($1, $3); }
                        | prefix-expr                                           { $$ = $1; }
                        ;
prefix-expr
                        : OP_MUL        prefix-expr                             { $$ = builder.make_xi_op(operator_t::__deref__, $2); }
                        | OP_BINARY_AND prefix-expr                             { $$ = builder.make_xi_op(operator_t::__address_of__, $2); }
                        | term-expr                                             { $$ = $1; }
                        ;
term-expr
                        : OP_LPAREN expr OP_RPAREN                              { $$ = $2; }
                        | OP_LPAREN expr OP_COMA expr-list-opt OP_RPAREN        { $$ = builder.make_xi_tuple_expr(make_list<expr_t>($2, $4)); }
                        | TOK_EXPR                                              { $$ = $1; }
                        | LITERAL_INTEGER                                       { $$ = $1; }
                        | LITERAL_REAL                                          { $$ = $1; }
                        | LITERAL_BOOL                                          { $$ = $1; }
                        | TOK_IDENTIFIER                                        { $$ = builder.make_xi_id_expr($1); }
                        ;
assign-op
                        : OP_ASSIGN                                             { $$ = $1; }
                        | OP_ASSIGN_ADD                                         { $$ = $1; }
                        | OP_ASSIGN_SUB                                         { $$ = $1; }
                        | OP_ASSIGN_MUL                                         { $$ = $1; }
                        | OP_ASSIGN_DIV                                         { $$ = $1; }
                        | OP_ASSIGN_MOD                                         { $$ = $1; }
                        | OP_ASSIGN_SL                                          { $$ = $1; }
                        | OP_ASSIGN_SR                                          { $$ = $1; }
                        | OP_ASSIGN_BINARY_AND                                  { $$ = $1; }
                        | OP_ASSIGN_BINARY_OR                                   { $$ = $1; }
                        | OP_ASSIGN_BINARY_XOR                                  { $$ = $1; }
                        | OP_ASSIGN_LOGICAL_AND                                 { $$ = $1; }
                        | OP_ASSIGN_LOGICAL_OR                                  { $$ = $1; }
                        | OP_ASSIGN_LOGICAL_XOR                                 { $$ = $1; }
                        ;

/* ---- *
 * Type *
 * ---- */

type
                        : postfix-type                                          { $$ = $1; }
                        ;
postfix-type
                        : prefix-type                                           { $$ = $1; }
                        ;
prefix-type
                        : KW_CONST non-const-prefix-type                        { $$ = builder.get_const_type($2); }
                        |          non-const-prefix-type                        { $$ = $1; }
                        ;
non-const-prefix-type
                        : OP_MUL        prefix-type                             { $$ = builder.get_pointer_type($2); }
                        | OP_BINARY_AND prefix-type                             { $$ = builder.get_reference_type($2); }
                        | term-type                                             { $$ = $1; }
                        ;
term-type
                        : TOK_TYPE                                              { $$ = $1; }
                        | TOK_IDENTIFIER                                        { $$ = builder.get_id_type($1); }
                        | OP_LPAREN type OP_RPAREN                              { $$ = $2; }
                        | OP_LPAREN
                            type OP_COMA type-list-opt
                          OP_RPAREN                                             { $$ = builder.get_tuple_type(make_list<type_t>($2, $4)); }
                        ;
type-list-opt
                        : type-list                                             { $$ = $1; }
                        | %empty                                                { $$ = make_list<type_t>(); }
                        ;
type-list
                        : type-list OP_COMA type                                { $$ = make_list<type_t>($1, $3); }
                        | type                                                  { $$ = make_list<type_t>($1);     }
                        ;


%%

void xi::parser::error(const YYLTYPE& loc, const std::string& msg) {
    std::cout << msg;
}


void yyerror(XILTYPE* loc, xi::parser&, xi_builder_t& builder, const char* msg) {
    printf("at %d:%d: %s\n", loc->begin.line, loc->begin.column, msg);
    assert(false);
}


