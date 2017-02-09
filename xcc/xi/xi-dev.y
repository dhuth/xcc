%defines                                            "xi-parser.hpp"
%define     api.prefix                              {xi}
%define     api.pure

%code requires {
#include    "xi_internal.hpp"
#define     YYSTYPE                         XISTYPE
#define     YY_DECL                         int yylex(YYSTYPE* yylval_param, xcc::translation_unit& tu, xi_builder_t& builder)
}

%token
        TOK_EOF                         0   "end of file"
        
        TOK_IDENTIFIER                      "identifier"
        TOK_TYPE                            "a single word type"
        
        LITERAL_INTEGER
        LITERAL_FLOAT
        
        OP_LBRACE                           "{"
        OP_RBRACE                           "}"
        OP_LBRACKET                         "["
        OP_RBRACKET                         "]"
        OP_LPAREN                           "("
        OP_RPAREN                           ")"
        
        OP_COLON                            ":"
        OP_DOUBLE_COLON                     "::"
        OP_SEMICOLON                        ";"
        
        OP_COMA                             ","
        OP_DOT                              "."
        
        OP_ADD                              "+"
        OP_SUB                              "-"
        OP_MUL                              "*"
        OP_DIV                              "/"
        OP_MOD                              "%"
        
        OP_LAND                             "&&"
        OP_LOR                              "||"
        OP_LNOT                             "!"
        OP_BAND                             "&"
        OP_BOR                              "|"
        OP_BXOR                             "^"
        OP_BNOT                             "~"
        
        OP_SHL                              "<<"
        OP_SHR                              ">>"
        
        OP_EQ                               "=="
        OP_NE                               "!="
        OP_LT                               "<"
        OP_GT                               ">"
        OP_LE                               "<="
        OP_GE                               ">="
        
        OP_ASSIGN                           "="
        OP_ASSIGN_ADD                       "+="
        OP_ASSIGN_SUB                       "-="
        OP_ASSIGN_MUL                       "*="
        OP_ASSIGN_DIV                       "/="
        OP_ASSIGN_MOD                       "%="
        
        KW_CONST                            "const"
        KW_EXTERN                           "extern"
        KW_NAMESPACE                        "namespace"
        KW_TYPEDEF                          "typedef"
;

%union {
        xcc::ast_expr*                      expr;
        xcc::ast_decl*                      decl;
        xcc::ast_type*                      type;
        xcc::ast_stmt*                      stmt;
        xcc::ast_function_decl*             function;
        xcc::ast_parameter_decl*            parameter;
        xcc::list<xcc::ast_expr>*           expr_list;
        xcc::list<xcc::ast_decl>*           decl_list;
        xcc::list<xcc::ast_type>*           type_list;
        xcc::list<xcc::ast_stmt>*           stmt_list;
        xcc::list<xcc::ast_parameter_decl>* parameter_list;
        xcc::xi_operator                    op;
        const char*                         text;
}

%type   <text>                              TOK_IDENTIFIER
%type   <type>                              TOK_TYPE
%type   <expr>                              LITERAL_INTEGER
%type   <expr>                              LITERAL_FLOAT

%type   <op>                                OP_ADD
%type   <op>                                OP_SUB
%type   <op>                                OP_MUL
%type   <op>                                OP_DIV
%type   <op>                                OP_MOD
%type   <op>                                OP_LAND
%type   <op>                                OP_LOR
%type   <op>                                OP_BAND
%type   <op>                                OP_BOR
%type   <op>                                OP_BXOR
%type   <op>                                OP_SHL
%type   <op>                                OP_SHR
%type   <op>                                OP_EQ
%type   <op>                                OP_NE
%type   <op>                                OP_LT
%type   <op>                                OP_LE
%type   <op>                                OP_GT
%type   <op>                                OP_GE

%type   <type>                              type
%type   <type>                              postfix-type
%type   <type>                              prefix-type
%type   <type>                              basic-type

%type   <expr>                              expr
%type   <expr>                              lor-expr
%type   <expr>                              land-expr
%type   <expr>                              bor-expr
%type   <expr>                              bxor-expr
%type   <expr>                              band-expr
%type   <expr>                              eq-expr
%type   <expr>                              cmp-expr
%type   <expr>                              add-expr
%type   <expr>                              mul-expr
%type   <expr>                              member-expr
%type   <expr>                              prefix-expr
%type   <expr>                              postfix-expr
%type   <expr>                              term-expr

%type   <stmt>                              stmt

%type   <stmt_list>                         stmt-list-opt
%type   <stmt_list>                         stmt-list

%type   <parameter_list>                    function-parameter-list-opt
%type   <parameter_list>                    function-parameter-list
%type   <parameter>                         function-parameter
%type   <function>                          function-proto

%code {
extern YY_DECL;
extern void yyerror(xcc::translation_unit& tu, xi_builder_t& builder, const char* msg);
}
%param                              {xcc::translation_unit& tu}
%param                              {xi_builder_t&          builder}

%start  translation-unit
%%


type
        :   postfix-type                                                                            { $$ = $1; }
        ;
postfix-type
        :   prefix-type                                                                             { $$ = $1; }
        |   postfix-type OP_MUL                                                                     { $$ = builder.get_pointer_type($1); }
        ;
prefix-type
        :   basic-type                                                                              { $$ = $1; }
        |   KW_CONST prefix-type                                                                    { $$ = builder.get_const_type($2);     }
        ;
basic-type
        :   TOK_IDENTIFIER                                                                          { $$ = builder.identifier_as_type($1); }
        |   TOK_TYPE                                                                                { $$ = $1; }
        |   OP_LPAREN type OP_RPAREN                                                                { $$ = $2; }
        ;


expr
        :   lor-expr                                                                                { $$ = $1; }
        ;
lor-expr
        :   lor-expr        OP_LOR      land-expr                                                   { $$ = builder.make_op($2, $1, $3); }
        |   land-expr                                                                               { $$ = $1; }
        ;
land-expr
        :   land-expr       OP_LAND     bor-expr                                                    { $$ = builder.make_op($2, $1, $3); }
        |   bor-expr                                                                                { $$ = $1; }
        ;
bor-expr
        :   bor-expr        OP_BOR      bxor-expr                                                   { $$ = builder.make_op($2, $1, $3); }
        |   bxor-expr                                                                               { $$ = $1; }
        ;
bxor-expr
        :   bxor-expr       OP_BXOR     band-expr                                                   { $$ = builder.make_op($2, $1, $3); }
        |   band-expr                                                                               { $$ = $1; }
        ;
band-expr
        :   band-expr       OP_BAND     eq-expr                                                     { $$ = builder.make_op($2, $1, $3); }
        |   eq-expr                                                                                 { $$ = $1; }
        ;
eq-expr
        :   eq-expr         OP_EQ       cmp-expr                                                    { $$ = builder.make_op($2, $1, $3); }
        |   eq-expr         OP_NE       cmp-expr                                                    { $$ = builder.make_op($2, $1, $3); }
        |   cmp-expr                                                                                { $$ = $1; }
        ;
cmp-expr
        :   cmp-expr        OP_LT       add-expr                                                    { $$ = builder.make_op($2, $1, $3); }
        |   cmp-expr        OP_LE       add-expr                                                    { $$ = builder.make_op($2, $1, $3); }
        |   cmp-expr        OP_GT       add-expr                                                    { $$ = builder.make_op($2, $1, $3); }
        |   cmp-expr        OP_GE       add-expr                                                    { $$ = builder.make_op($2, $1, $3); }
        |   add-expr                                                                                { $$ = $1; }
        ;
add-expr
        :   add-expr        OP_ADD      mul-expr                                                    { $$ = builder.make_op($2, $1, $3); }
        |   add-expr        OP_SUB      mul-expr                                                    { $$ = builder.make_op($2, $1, $3); }
        |   mul-expr                                                                                { $$ = $1; }
        ;
mul-expr
        :   mul-expr        OP_MUL      member-expr                                                 { $$ = builder.make_op($2, $1, $3); }
        |   mul-expr        OP_DIV      member-expr                                                 { $$ = builder.make_op($2, $1, $3); }
        |   mul-expr        OP_MOD      member-expr                                                 { $$ = builder.make_op($2, $1, $3); }
        |   member-expr                                                                             { $$ = $1; }
        ;
member-expr
        :   member-expr     OP_DOT      TOK_IDENTIFIER
        |   prefix-expr                                                                             { $$ = $1; }
        ;
prefix-expr
        :   OP_SUB          prefix-expr                                                             { $$ = builder.make_op($1, $2); }
        |   OP_ADD          prefix-expr                                                             { $$ = builder.make_op($1, $2); }
        |   OP_MUL          prefix-expr                                                             { $$ = builder.make_op($1, $2); }
        |   OP_LAND         prefix-expr                                                             { $$ = builder.make_op($1, $2); }
        |   postfix-expr                                                                            { $$ = $1; }
        ;
postfix-expr
        :   term-expr                                                                               { $$ = $1; }
        ;
term-expr
        :   LITERAL_INTEGER                                                                         { $$ = $1; }
        |   LITERAL_FLOAT                                                                           { $$ = $1; }
        |   TOK_IDENTIFIER                                                                          { $$ = builder.identifier_as_expr($1); }
        |   OP_LPAREN expr OP_RPAREN                                                                { $$ = $2; }
        ;




stmt
        :   OP_SEMICOLON                                                                            { $$ = builder.make_nop_stmt(); }
        ;




stmt-list-opt
        :   %empty                                                                                  { $$ = new xcc::list<xcc::ast_stmt>(); }
        |   stmt-list                                                                               { $$ = $1; }
        ;
stmt-list
        :   stmt-list stmt                                                                          { $1->append($2); $$ = $1; }
        |   stmt                                                                                    { $$ = new xcc::list<xcc::ast_stmt>(); }
        ;




global-variable
        :   KW_EXTERN type TOK_IDENTIFIER OP_SEMICOLON                                              { auto var = builder.define_global_variable($2, $3);     var->is_extern = true; }
        |             type TOK_IDENTIFIER OP_SEMICOLON                                              { auto var = builder.define_global_variable($1, $2);                            }
        |             type TOK_IDENTIFIER OP_ASSIGN expr OP_SEMICOLON                               { auto var = builder.define_global_variable($1, $2, $4);                        }
        ;




global-function
        :   KW_EXTERN function-proto OP_SEMICOLON                                                   { $2->is_extern = true; }
        |             function-proto OP_SEMICOLON
        |             function-proto OP_LBRACKET { builder.begin_function_def($1); } stmt-list-opt OP_RBRACKET { builder.end_function_def($1, $4); }
        ;
function-proto
        :   type TOK_IDENTIFIER OP_LPAREN function-parameter-list-opt OP_RPAREN                     { $$ = builder.define_function($1, $2, $4); }
        ;
function-parameter-list-opt
        :   %empty                                                                                  { $$ = new xcc::list<xcc::ast_parameter_decl>(); }
        |   function-parameter-list                                                                 { $$ = $1; }
        ;
function-parameter-list
        :   function-parameter-list OP_COMA function-parameter                                      { $1->append($3); $$ = $1; }
        |   function-parameter                                                                      { $$ = new xcc::list<xcc::ast_parameter_decl>($1); }
        ;
function-parameter
        :   type TOK_IDENTIFIER                                                                     { $$ = builder.define_parameter($1, $2); }
        ;



typedef
        :   KW_TYPEDEF type TOK_IDENTIFIER OP_SEMICOLON                                             { builder.define_typedef($3, $2); }
        ;




global-decl-list-opt
        :   global-decl-list
        |   %empty
        ;
global-decl-list
        :   global-decl global-decl-list
        |   global-decl
        ;
global-decl
        :   global-variable
        |   global-typedef
        |   global-function
        ;
global-typedef
        :   typedef
        ;


translation-unit
        :   global-decl-list-opt TOK_EOF
        ;
%%

void yyerror(xcc::translation_unit& tu, xi_builder_t& builder, const char* msg) {
    printf("%s\n", msg);
}


