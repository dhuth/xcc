%defines                                            "xi-parser.hpp"
%define     api.prefix                              {xi}
%define     api.pure

%code requires {
#include    "xi_internal.hpp"
#define     YYSTYPE                         XISTYPE
#define     YY_DECL                         int yylex(YYSTYPE* yylval_param, xi_builder_t& builder)
}

%token
        TOK_EOF                         0   "end of file"
        
        TOK_IDENTIFIER                      "identifier"
        TOK_TYPE                            "a single word type"
        TOK_DECL                            "a named declaration"
        
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
        
        KW_ABSTRACT                         "abstract"
        KW_AUTO                             "auto"
        KW_BYREF                            "byref"
        KW_CLASS                            "class"
        KW_CONST                            "const"
        KW_EXTERN                           "extern"
        KW_FOR                              "for"
        KW_GENERIC                          "generic"
        KW_IF                               "if"
        KW_IN                               "in"
        KW_INTERFACE                        "interface"
        KW_INTERNAL                         "internal"
        KW_NAMESPACE                        "namespace"
        KW_OUT                              "out"
        KW_PRIVATE                          "private"
        KW_PROTECTED                        "protected"
        KW_PUBLIC                           "public"
        KW_RETURN                           "return"
        KW_STRUCT                           "struct"
        KW_TYPEDEF                          "typedef"
        KW_WHILE                            "while"
        KW_VIRTUAL                          "virtual"
;

%union {
        xcc::ast_expr*                      expr;
        xcc::ast_decl*                      decl;
        xcc::ast_type*                      type;
        xcc::ast_stmt*                      stmt;
        xcc::list<xcc::ast_expr>*           expr_list;
        xcc::list<xcc::ast_decl>*           decl_list;
        xcc::list<xcc::ast_type>*           type_list;
        xcc::list<xcc::ast_stmt>*           stmt_list;
        
        xcc::xi_function_decl*              function;
        xcc::xi_parameter_decl*             parameter;
        xcc::list<xcc::xi_parameter_decl>*  parameter_list;
        
        xcc::xi_operator                    op;
        const char*                         text;
}

%type   <text>                              TOK_IDENTIFIER
%type   <decl>                              TOK_DECL
%type   <type>                              TOK_TYPE

%type   <expr>                              LITERAL_INTEGER
%type   <expr>                              LITERAL_FLOAT

%type   <type>                              type
%type   <type>                              postfix-type
%type   <type>                              prefix-type
%type   <type>                              term-type

%type   <expr>                              expr
%type   <expr>                              land-expr
%type   <expr>                              lor-expr
%type   <expr>                              loose-prefix-expr
%type   <expr>                              band-expr
%type   <expr>                              bor-expr
%type   <expr>                              cmp-expr
%type   <expr>                              add-expr
%type   <expr>                              shift-expr
%type   <expr>                              mul-expr
%type   <expr>                              tight-prefix-expr
%type   <expr>                              postfix-expr
%type   <expr>                              term-expr

%type   <op>                                OP_ADD
%type   <op>                                OP_SUB
%type   <op>                                OP_MUL
%type   <op>                                OP_DIV
%type   <op>                                OP_MOD
%type   <op>                                OP_LAND
%type   <op>                                OP_LOR
%type   <op>                                OP_LNOT
%type   <op>                                OP_BAND
%type   <op>                                OP_BOR
%type   <op>                                OP_BXOR
%type   <op>                                OP_BNOT
%type   <op>                                OP_SHL
%type   <op>                                OP_SHR
%type   <op>                                OP_EQ
%type   <op>                                OP_NE
%type   <op>                                OP_LT
%type   <op>                                OP_LE
%type   <op>                                OP_GT
%type   <op>                                OP_GE

%type   <op>                                land-op
%type   <op>                                lor-op
%type   <op>                                loose-prefix-op
%type   <op>                                cmp-op
%type   <op>                                band-op
%type   <op>                                bor-op
%type   <op>                                add-op
%type   <op>                                shift-op
%type   <op>                                mul-op
%type   <op>                                tight-prefix-op

%type   <function>                          global-function-proto-decl

%type   <parameter_list>                    function-parameter-list-opt
%type   <parameter_list>                    function-parameter-list
%type   <parameter>                         function-parameter

%type   <stmt>                              stmt
%type   <stmt>                              block-stmt
%type   <stmt>                              return-stmt
%type   <stmt>                              continue-stmt
%type   <stmt>                              assign-stmt


%code {
extern YY_DECL;
void yyerror(xi_builder_t& builder, const char* msg);

}
%param                              {xi_builder_t&          builder}

%start  translation-unit
%%

translation-unit
        : global-decl-list-opt TOK_EOF
        ;

global-decl-list-opt
        : global-decl-list
        | %empty
        ;
        
global-decl-list
        : global-decl global-decl-list
        | global-decl
        ;

global-decl
        : global-namespace-decl
        | global-variable-decl
        | global-function-decl
        ;

global-namespace-decl
        : KW_NAMESPACE TOK_IDENTIFIER
            OP_LBRACE                       { builder.push_namespace(builder.define_namespace($2)); }
            global-decl-list
            OP_RBRACE                       { builder.pop(); }
        ;
global-variable-decl
        : KW_EXTERN type TOK_IDENTIFIER OP_SEMICOLON                                                { auto var = builder.define_global_variable($2, $3); var->is_extern = true; }
        |           type TOK_IDENTIFIER OP_SEMICOLON                                                { auto var = builder.define_global_variable($1, $2); }
        |           type TOK_IDENTIFIER OP_EQ expr OP_SEMICOLON                                     { auto var = builder.define_global_variable($1, $2, $4); }
        ;
global-function-decl
        : KW_EXTERN global-function-proto-decl OP_SEMICOLON                                         { $2->is_extern = true; }
        |           global-function-proto-decl OP_SEMICOLON                                         { /* do nothing */ }
        |           global-function-proto-decl
                      OP_LBRACE             { builder.push_function($1); }
                        stmt-list-opt
                      OP_RBRACE             { builder.pop_function(); }
        ;

global-function-proto-decl
        : type TOK_IDENTIFIER OP_LPAREN function-parameter-list-opt OP_RPAREN                       { $$ = builder.define_global_function($1, $2, $4); }
        ;

function-parameter-list-opt
        : function-parameter-list                                                                   { $$ = $1; }
        | %empty                                                                                    { $$ = new xcc::list<xcc::xi_parameter_decl>(); }
        ;
function-parameter-list
        : function-parameter-list OP_COMA function-parameter                                        { $1->append($3); $$ = $1; }
        | function-parameter                                                                        { $$ = new xcc::list<xcc::xi_parameter_decl>($1); }
        ;
function-parameter
        : type TOK_IDENTIFIER                                                                       { $$ = builder.define_parameter($1, $2); }
        | type                                                                                      { $$ = builder.define_parameter($1);     }
        ;
        
stmt-list-opt
        : stmt-list
        | %empty
        ;
stmt-list
        : stmt { builder.emit($1); } stmt-list
        | stmt { builder.emit($1); }
        ;
stmt
        : block-stmt                                                                                { $$ = $1; }
        | return-stmt                                                                               { $$ = $1; }
        | continue-stmt                                                                             { $$ = $1; }
        | OP_SEMICOLON                                                                              { $$ = builder.make_nop_stmt(); }
        ;
block-stmt
        : OP_LBRACE { builder.push_block(builder.make_block_stmt()); }
            stmt-list-opt
          OP_RBRACE { $$ = builder.pop_block(); }
        ;
return-stmt
        : KW_RETURN OP_SEMICOLON                                                                    { $$ = builder.make_return_stmt();   }
        | KW_RETURN expr OP_SEMICOLON                                                               { $$ = builder.make_return_stmt($2); }
        ;
continue-stmt
        : KW_CONTINUE OP_SEMICOLON                                                                  { $$ = builder.make_continue_stmt(); }
        ;
break-stmt
        : KW_BREAK OP_SEMICOLON                                                                     { $$ = builder.make_break_stmt(); }
        ;

type
        : postfix-type                                                                              { $$ = $1; }
        ;
postfix-type
        : prefix-type                                                                               { $$ = $1; }
        ;
prefix-type
        : term-type                                                                                 { $$ = $1; }
        ;
term-type
        : TOK_TYPE                                                                                  { $$ = $1; }
        | OP_LPAREN type OP_RPAREN                                                                  { $$ = $2; }
        ;

expr                : land-expr                                 { $$ = $1; }
                    ;
land-expr           : land-expr     land-op lor-expr            { $$ = builder.make_op($2, $1, $3); }
                    | lor-expr                                  { $$ = $1; }
                    ;
lor-expr            : lor-expr      lor-op  loose-prefix-expr   { $$ = builder.make_op($2, $1, $3); }
                    | loose-prefix-expr                         { $$ = $1; }
                    ;
loose-prefix-expr   : loose-prefix-op loose-prefix-expr         { $$ = builder.make_op($1, $2); }
                    | cmp-expr                                  { $$ = $1; }
                    ;
cmp-expr            : band-expr     cmp-op  band-expr           { $$ = builder.make_op($2, $1, $3); }
                    | band-expr                                 { $$ = $1; }
                    ;
band-expr           : band-expr     band-op bor-expr            { $$ = builder.make_op($2, $1, $3); }
                    | bor-expr                                  { $$ = $1; }
                    ;
bor-expr            : bor-expr      bor-op  add-expr            { $$ = builder.make_op($2, $1, $3); }
                    | add-expr                                  { $$ = $1; }
                    ;
add-expr            : add-expr      add-op  shift-expr          { $$ = builder.make_op($2, $1, $3); }
                    | shift-expr                                { $$ = $1; }
                    ;
shift-expr          : shift-expr    shift-op mul-expr           { $$ = builder.make_op($2, $1, $3); }
                    | mul-expr                                  { $$ = $1; }
                    ;
mul-expr            : mul-expr      mul-op  tight-prefix-expr   { $$ = builder.make_op($2, $1, $3); }
                    | tight-prefix-expr                         { $$ = $1; }
                    ;
tight-prefix-expr   : tight-prefix-op tight-prefix-expr         { $$ = builder.make_op($1, $2); }
                    | postfix-expr                              { $$ = $1; }
                    ;
postfix-expr        : term-expr                                 { $$ = $1; }
                    ;
term-expr           : LITERAL_INTEGER                           { $$ = $1; }
                    | LITERAL_FLOAT                             { $$ = $1; }
                    ;


land-op
        : OP_LAND                                                                                   { $$ = $1; }
        ;
lor-op
        : OP_LOR                                                                                    { $$ = $1; }
        ;
loose-prefix-op
        : OP_LNOT                                                                                   { $$ = $1; }
        ;
cmp-op
        : OP_EQ                                                                                     { $$ = $1; }
        | OP_NE                                                                                     { $$ = $1; }
        | OP_LT                                                                                     { $$ = $1; }
        | OP_GT                                                                                     { $$ = $1; }
        | OP_LE                                                                                     { $$ = $1; }
        | OP_GE                                                                                     { $$ = $1; }
        ;
band-op
        : OP_BAND                                                                                   { $$ = $1; }
        ;
bor-op
        : OP_BOR                                                                                    { $$ = $1; }
        | OP_BXOR                                                                                   { $$ = $1; }
        ;
add-op
        : OP_ADD                                                                                    { $$ = $1; }
        | OP_SUB                                                                                    { $$ = $1; }
        ;
shift-op
        : OP_SHL                                                                                    { $$ = $1; }
        | OP_SHR                                                                                    { $$ = $1; }
        ;
mul-op
        : OP_MUL                                                                                    { $$ = $1; }
        | OP_DIV                                                                                    { $$ = $1; }
        | OP_MOD                                                                                    { $$ = $1; }
        ;
tight-prefix-op
        : OP_ADD                                                                                    { $$ = $1; }
        | OP_SUB                                                                                    { $$ = $1; }
        | OP_MUL                                                                                    { $$ = $1; }
        | OP_BAND                                                                                   { $$ = $1; }
        | OP_BNOT                                                                                   { $$ = $1; }
        ;

%%

void yyerror(xi_builder_t& builder, const char* msg) {
    printf("%s\n", msg);
}


