%defines                                            "xi-parser.hpp"
%define     api.prefix                              {xi}
%define     api.pure

%locations

%code requires {
#include    "xi_internal.hpp"
#include    "source.hpp"
#define     YYSTYPE                         XISTYPE
#define     XILTYPE                         xcc::source_span
#define     YY_DECL                         int yylex(YYSTYPE* yylval_param, XILTYPE* yyloc, xi_builder_t& builder)

#define     YYLTYPE_IS_DECLARED             1
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
        OP_ASSIGN_SHL                       "<<="
        OP_ASSIGN_SHR                       ">>="
        OP_ASSIGN_LAND                      "&&="
        OP_ASSIGN_LOR                       "||="
        OP_ASSIGN_BAND                      "&="
        OP_ASSIGN_BOR                       "|="
        
        KW_ABSTRACT                         "abstract"
        KW_AUTO                             "auto"
        KW_BYREF                            "byref"
        KW_BYVAL                            "byval"
        KW_BREAK                            "break"
        KW_CLASS                            "class"
        KW_CONST                            "const"
        KW_CONTINUE                         "continue"
        KW_ELSE                             "else"
        KW_ELIF                             "elif"
        KW_EXTERN                           "extern"
        KW_FOR                              "for"
        KW_FUNC                             "func"
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

%type   <expr_list>                         dim-expr-list

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

%type   <expr_list>                         expr-list-opt
%type   <expr_list>                         expr-list

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
%type   <op>                                OP_ASSIGN
%type   <op>                                OP_ASSIGN_ADD
%type   <op>                                OP_ASSIGN_SUB
%type   <op>                                OP_ASSIGN_MUL
%type   <op>                                OP_ASSIGN_DIV
%type   <op>                                OP_ASSIGN_MOD
%type   <op>                                OP_ASSIGN_SHL
%type   <op>                                OP_ASSIGN_SHR
%type   <op>                                OP_ASSIGN_LAND
%type   <op>                                OP_ASSIGN_LOR
%type   <op>                                OP_ASSIGN_BAND
%type   <op>                                OP_ASSIGN_BOR

%type   <op>                                assign-op
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
%type   <stmt>                              break-stmt
%type   <stmt>                              return-stmt
%type   <stmt>                              continue-stmt
%type   <stmt>                              assign-stmt
%type   <stmt>                              if-stmt
%type   <stmt>                              for-stmt
%type   <stmt>                              while-stmt
%type   <stmt>                              local-stmt
%type   <stmt>                              else-stmt
%type   <stmt>                              elif-stmt

%type   <stmt>                              begin-block-stmt


%code {
extern YY_DECL;
void yyerror(XILTYPE* loc, xi_builder_t& builder, const char* msg);
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
        | global-typedef-decl
        ;

global-namespace-decl
        : KW_NAMESPACE TOK_IDENTIFIER
            OP_LBRACE                       { builder.push_namespace(builder.define_namespace($2)); }
            global-decl-list
            OP_RBRACE                       { builder.pop(); }
        ;
global-typedef-decl
        : KW_TYPEDEF type TOK_IDENTIFIER OP_SEMICOLON                                               { builder.define_named_type($3, $2); }
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
        : KW_FUNC type TOK_IDENTIFIER OP_LPAREN function-parameter-list-opt OP_RPAREN               { $$ = builder.define_global_function($2, $3, $5); }
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
        : stmt        { builder.emit($1); } stmt-list-opt
        | local-stmt  { builder.emit($1); }
        | %empty
        ;
stmt
        : block-stmt                                                                                { $$ = $1; }
        | return-stmt                                                                               { $$ = $1; }
        | continue-stmt                                                                             { $$ = $1; }
        | break-stmt                                                                                { $$ = $1; }
        | assign-stmt                                                                               { $$ = $1; }
        | if-stmt                                                                                   { $$ = $1; }
        | while-stmt                                                                                { $$ = $1; }
        | OP_SEMICOLON                                                                              { $$ = builder.make_nop_stmt(); }
        ;
block-stmt
        : OP_LBRACE begin-block-stmt { builder.push_block($2->as<xcc::ast_block_stmt>()); }
            stmt-list-opt
          OP_RBRACE { $$ = $2; builder.pop(); }
        ;
begin-block-stmt
        : %empty { $$ = builder.make_block_stmt(); }
        ;
return-stmt
        : KW_RETURN OP_SEMICOLON                                                                    { $$ = builder.make_return_stmt(nullptr, nullptr);   }
        | KW_RETURN expr OP_SEMICOLON                                                               { $$ = builder.make_return_stmt(builder.get_return_type(), $2); }
        ;
continue-stmt
        : KW_CONTINUE OP_SEMICOLON                                                                  { $$ = builder.make_continue_stmt(); }
        ;
break-stmt
        : KW_BREAK OP_SEMICOLON                                                                     { $$ = builder.make_break_stmt(); }
        ;
assign-stmt
        : expr assign-op expr OP_SEMICOLON                                                          { $$ = builder.make_assign_stmt($2, $1, $3); }
        ;
        
if-stmt
        : KW_IF OP_LPAREN expr OP_RPAREN stmt                                                       { $$ = builder.make_if_stmt($3, $5, nullptr); }
        | KW_IF OP_LPAREN expr OP_RPAREN stmt else-stmt                                             { $$ = builder.make_if_stmt($3, $5, $6);      }
        | KW_IF OP_LPAREN expr OP_RPAREN stmt elif-stmt                                             { $$ = builder.make_if_stmt($3, $5, $6);      }
        ;
else-stmt
        : KW_ELSE stmt                                                                              { $$ = $2; }
        ;
elif-stmt
        : KW_ELIF OP_LPAREN expr OP_RPAREN stmt                                                     { $$ = builder.make_if_stmt($3, $5, nullptr); }
        | KW_ELIF OP_LPAREN expr OP_RPAREN stmt elif-stmt                                           { $$ = builder.make_if_stmt($3, $5, $6);      }
        | KW_ELIF OP_LPAREN expr OP_RPAREN stmt else-stmt                                           { $$ = builder.make_if_stmt($3, $5, $6);      }
        ;
while-stmt
        : KW_WHILE OP_LPAREN expr OP_RPAREN stmt                                                    { $$ = builder.make_while_stmt($3, $5); }
        ;
local-stmt
        : type TOK_IDENTIFIER OP_SEMICOLON begin-block-stmt
                {
                    builder.push_block($4->as<xcc::ast_block_stmt>());
                    auto vardecl = builder.define_local_variable($1, $2);
                    builder.emit(
                            builder.make_assign_stmt(
                                xcc::xi_operator::assign,
                                builder.make_declref_expr(vardecl),
                                builder.make_zero($1)));
                }
            stmt-list-opt
                { $$ = $4; builder.pop(); }
        | type TOK_IDENTIFIER OP_ASSIGN expr OP_SEMICOLON begin-block-stmt
                {
                    builder.push_block($6->as<xcc::ast_block_stmt>());
                    auto vardecl = builder.define_local_variable($1, $2);
                    builder.emit(
                            builder.make_assign_stmt(
                                xcc::xi_operator::assign,
                                builder.make_declref_expr(vardecl),
                                $4));
                }
            stmt-list-opt
                { $$ = $6; builder.pop(); }
        ;




type
        : postfix-type                                                                              { $$ = $1; }
        ;
postfix-type
        : postfix-type OP_LBRACKET dim-expr-list OP_RBRACKET                                        { $$ = builder.get_array_type($1, $3); }
        | prefix-type                                                                               { $$ = $1; }
        ;
prefix-type
        : term-type                                                                                 { $$ = $1; }
        ;
term-type
        : TOK_TYPE                                                                                  { $$ = $1; }
        | OP_LPAREN type OP_RPAREN                                                                  { $$ = $2; }
        ;




dim-expr-list
        : dim-expr-list OP_COMA                                                                     { $1->append(nullptr); $$=$1;            }
        | dim-expr-list OP_COMA expr                                                                { $1->append($3);      $$=$1;            }
        | expr                                                                                      { $$ = new xcc::list<xcc::ast_expr>($1); }
        | %empty                                                                                    { $$ = new xcc::list<xcc::ast_expr>();   }
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
postfix-expr        : postfix-expr OP_LPAREN expr-list-opt OP_RPAREN
                                                                { $$ = builder.make_call_expr($1, $3); }
                    | postfix-expr OP_LBRACKET expr-list-opt OP_RBRACKET
                                                                { $$ = builder.make_index_expr($1, $3); }
                    | term-expr                                 { $$ = $1; }
                    ;
term-expr           : LITERAL_INTEGER                           { $$ = $1; }
                    | LITERAL_FLOAT                             { $$ = $1; }
                    | TOK_IDENTIFIER                            { $$ = builder.make_declref_expr(builder.find_declaration($1)); }
                    ;


expr-list-opt
                    : expr-list                                 { $$ = $1; }
                    | %empty                                    { $$ = new xcc::list<xcc::ast_expr>(); }
                    ;
expr-list
                    : expr-list OP_COMA expr                    { $1->append($3); $$ = $1; }
                    | expr                                      { $$ = new xcc::list<xcc::ast_expr>($1); }
                    ;




assign-op
        : OP_ASSIGN                                                                                 { $$ = $1; }
        | OP_ASSIGN_ADD                                                                             { $$ = $1; }
        | OP_ASSIGN_SUB                                                                             { $$ = $1; }
        | OP_ASSIGN_MUL                                                                             { $$ = $1; }
        | OP_ASSIGN_DIV                                                                             { $$ = $1; }
        | OP_ASSIGN_MOD                                                                             { $$ = $1; }
        | OP_ASSIGN_SHL                                                                             { $$ = $1; }
        | OP_ASSIGN_SHR                                                                             { $$ = $1; }
        | OP_ASSIGN_LAND                                                                            { $$ = $1; }
        | OP_ASSIGN_LOR                                                                             { $$ = $1; }
        | OP_ASSIGN_BAND                                                                            { $$ = $1; }
        | OP_ASSIGN_BOR                                                                             { $$ = $1; }
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

void yyerror(XILTYPE* loc, xi_builder_t& builder, const char* msg) {
    printf("at %d:%d: %s\n", loc->first.line_number, loc->first.column_number, msg);
}


