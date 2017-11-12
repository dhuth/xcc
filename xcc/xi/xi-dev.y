%defines                                            "xi-parser.hpp"
%define     api.prefix                              {xi}
%define     api.pure
%define     parse.trace
%define     parse.error                             verbose

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

#define SETLOC                              xcc::setloc

#define XI_STATE_UNUSED_ID                  2

void xi_lex_state_push(int);
void xi_lex_state_pop(int);

void xi_lex_searchspace_set(xcc::ast_namespace_decl*);
void xi_lex_searchspace_set(xcc::ast_type*);
void xi_lex_searchspace_clear();

}

%token
        TOK_EOF                         0   "end of file"
        
        TOK_ERROR                           "error"
        TOK_IDENTIFIER                      "identifier"
        TOK_TYPE                            "a single word type"
        TOK_DECL                            "a named declaration"
        TOK_EXPR                            "a named expression"
        TOK_NAMESPACE                       "a namespace"
        
        LITERAL_INTEGER
        LITERAL_FLOAT
        LITERAL_STRING

        OP_LBRACE                           "{"
        OP_RBRACE                           "}"
        OP_LBRACKET                         "["
        OP_RBRACKET                         "]"
        OP_LPAREN                           "("
        OP_RPAREN                           ")"
        
        OP_ARROW                            "->"
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
        
        OP_LAND                             "and"
        OP_LOR                              "or"
        OP_LXOR                             "xor"
        OP_LNOT                             "not"
        OP_BAND                             "&"
        OP_BOR                              "|"
        OP_BXOR                             "^"
        OP_BNOT                             "~"
        
        OP_SHL                              "sl"
        OP_SHR                              "sr"
        
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
        OP_ASSIGN_SHL                       "sl="
        OP_ASSIGN_SHR                       "sr="
        OP_ASSIGN_LAND                      "and="
        OP_ASSIGN_LOR                       "or="
        OP_ASSIGN_BAND                      "&="
        OP_ASSIGN_BOR                       "|="
        
        OP_INDEX                            "[]"
        OP_INVOKE                           "()"
        
        
        KW_CLASS                            "class"
        KW_CONCEPT                          "concept"
        KW_CONST                            "const"
        KW_EXPORT_CFUNC                     "export cfunc"
        KW_EXTERN                           "extern"
        KW_FUNC                             "func"
        KW_IMPORT_CFUNC                     "import cfunc"
        KW_INTERNAL                         "internal"
        KW_NAMESPACE                        "namespace"
        KW_STATIC                           "static"
        KW_STRUCT                           "struct"
        KW_TRAIT                            "trait"
        KW_TYPEDEF                          "typedef"

        // Statement context keywords
        
        KW_BREAK                            "break"
        KW_CONTINUE                         "continue"
        KW_ELSE                             "else"
        KW_ELIF                             "elif"
        KW_FOR                              "for"
        KW_IF                               "if"
        KW_IN                               "in"
        KW_LOCAL                            "local"
        KW_RETURN                           "return"
        KW_WHILE                            "while"
        KW_YIELD                            "yield"
        
        // Expression context keywords
        
        KW_ITER                             "iter"
        KW_MAP                              "map"
        KW_MATCH                            "match"
        KW_RANGE                            "range"
        KW_SIZEOF                           "sizeof"
        KW_TYPEOF                           "typeof"
        KW_ZERO                             "zero"
        KW_ZIP                              "zip"
;

%union {
        expr_t*                             expr;
        decl_t*                             decl;
        type_t*                             type;
        stmt_t*                             stmt;
        xcc::ast_namespace_decl*            namespace_decl;
        id_list_t*                          text_list;
        expr_list_t*                        expr_list;
        decl_list_t*                        decl_list;
        type_list_t*                        type_list;
        stmt_list_t*                        stmt_list;
        
        xcc::xi_function_decl*              function;
        xcc::xi_parameter_decl*             parameter;
        xcc::xi_struct_decl*                struct_decl;
        xcc::xi_class_decl*                 class_decl;
        xcc::xi_trait_decl*                 trait_decl;
        xcc::list<xcc::xi_parameter_decl>*  parameter_list;
        
        operator_t                          op;
        char*                               text;
}

%type   <text>                              TOK_IDENTIFIER
%type   <decl>                              TOK_DECL
%type   <namespace_decl>                    TOK_NAMESPACE
%type   <type>                              TOK_TYPE
%type   <expr>                              TOK_EXPR

%type   <text>                              unused-id
%type   <text_list>                         unused-id-list-opt
%type   <text_list>                         unused-id-list
%type   <text_list>                         unused-id-list-p1

%type   <expr>                              LITERAL_INTEGER
%type   <expr>                              LITERAL_FLOAT
%type   <expr>                              LITERAL_STRING

%type   <namespace_decl>                    q-namespace-prefix
%type   <namespace_decl>                    q-namespace

%type   <type>                              type
%type   <type>                              prefix-type
%type   <type>                              postfix-type
%type   <type>                              term-type
%type   <type>                              q-type-prefix
%type   <type>                              q-type
%type   <type_list>                         type-list-opt
%type   <type_list>                         type-list
%type   <type_list>                         type-list-p1

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
%type   <expr_list>                         expr-list-p1

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

%type   <function>                          global-function-decl-rest
%type   <function>                          global-function-prototype-decl

%type   <type>                              function-return-type-decl-opt
%type   <parameter_list>                    function-parameter-list-opt
%type   <parameter_list>                    function-parameter-list
%type   <parameter>                         function-parameter

%type   <stmt>                              stmt
%type   <stmt>                              local-stmt
%type   <stmt>                              block-stmt
%type   <stmt>                              begin-block-stmt
%type   <stmt>                              expr-stmt
%type   <stmt>                              assign-stmt
%type   <stmt>                              continue-stmt
%type   <stmt>                              break-stmt
%type   <stmt>                              return-stmt
%type   <stmt>                              if-stmt
%type   <stmt>                              elif-stmt
%type   <stmt>                              else-stmt
%type   <stmt>                              noif-stmt
%type   <stmt>                              op-colon-noif-stmt


%code {
extern YY_DECL;
void yyerror(XILTYPE* loc, xi_builder_t& builder, const char* msg);

}
%param                              {xi_builder_t&          builder}

%start  translation-unit
%%

/* ---------------- *
 * Translation Unit *
 * ---------------- */

translation-unit
        :   global-decl-list-opt TOK_EOF
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
        :   global-namespace-decl
        //|   global-variable-decl
        |   global-function-decl
        //|   global-struct-decl
        //|   global-class-decl
        //|   global-concept-decl
        //|   global-trait-decl
        ;


/* --------------------- *
 * Namespace Declaration *
 * --------------------- */

global-namespace-decl
        :   KW_NAMESPACE global-namespace-decl-cont
        ;
global-namespace-decl-cont
        :   unused-id                                                                               { builder.push_namespace(builder.define_namespace($1)); }
                OP_LBRACE
                global-decl-list-opt
                OP_RBRACE                                                                           { builder.pop(); }
        |   unused-id OP_DOUBLE_COLON                                                               { builder.push_namespace(builder.define_namespace($1)); }
                global-namespace-decl-cont                                                          { builder.pop(); }
        ;
q-namespace-prefix
        :   TOK_NAMESPACE                                                                           { $$ = $1; xi_lex_searchspace_set($1); }
        |   q-namespace-prefix
                OP_DOUBLE_COLON
                TOK_NAMESPACE                                                                       { $$ = $3; xi_lex_searchspace_set($3); }
        ;
q-namespace
        :   q-namespace-prefix                                                                      { $$ = $1; xi_lex_searchspace_clear(); }
        ;

/* ---------------- *
 * Global Functions *
 * ---------------- */

global-function-decl
        :   KW_FUNC global-function-decl-rest                                                       { setloc($2, @$); }
        ;
global-function-decl-rest
        :   global-function-prototype-decl OP_SEMICOLON                                             { $$ = $1; }
        |   global-function-prototype-decl                                                          { builder.push_function_and_body($1); }
                OP_LBRACE
                    stmt-list-opt
                OP_RBRACE                                                                           { builder.pop_function_and_body(); $$ = $1; }
        ;
global-function-prototype-decl
        :  TOK_IDENTIFIER
        // TODO: handle template parameters
                OP_LPAREN
                    function-parameter-list-opt
                OP_RPAREN
                function-return-type-decl-opt                                                       { $$ = SETLOC(builder.define_global_function($1, $3, $5), @$); }
        ;

/* ------------- *
 * All Functions *
 * ------------- */

function-parameter-list-opt
        :   function-parameter-list                                                                 { $$ = $1; }
        |   %empty                                                                                  { $$ = new xcc::list<xcc::xi_parameter_decl>(); }
        ;
function-parameter-list
        :   function-parameter-list OP_COMA function-parameter                                      { $$ = new xcc::list<xcc::xi_parameter_decl>($1, $3); }
        |   function-parameter                                                                      { $$ = new xcc::list<xcc::xi_parameter_decl>($1); }
        ;
function-parameter
        :   TOK_IDENTIFIER OP_COLON type                                                            { $$ = SETLOC(builder.define_parameter($3, $1), @$); }
        |                           type                                                            { $$ = SETLOC(builder.define_parameter($1),     @$); }
        ;
function-return-type-decl-opt
        :   OP_ARROW type                                                                           { $$ = $2; }
        |   %empty                                                                                  { $$ = builder.get_void_type(); }
        ;

/* ---- *
 * Type *
 * ---- */

type
        :   prefix-type                                                                             { $$ = $1; }
        ;
prefix-type
        :   OP_MUL prefix-type                                                                      { $$ = builder.get_pointer_type($2); }
        |   OP_MOD prefix-type                                                                      { $$ = builder.get_reference_type($2); }
        |   KW_FUNC OP_LPAREN type-list-opt OP_RPAREN OP_ARROW type                                 { $$ = builder.get_function_type($6,                      $3); }
        |   KW_FUNC OP_LPAREN type-list-opt OP_RPAREN                                               { $$ = builder.get_function_type(builder.get_void_type(), $3); }
        |   postfix-type                                                                            { $$ = $1; }
        ;
postfix-type
        :   term-type                                                                               { $$ = $1; }
        ;
term-type
        :   q-type                                                                                  { $$ = $1; }
        ;
q-type-prefix
        :   q-namespace-prefix
                OP_DOUBLE_COLON
                TOK_TYPE                                                                            { $$ = $3; xi_lex_searchspace_set($3); }
        |   q-type                                                                                  {          xi_lex_searchspace_set($1); }
                OP_DOUBLE_COLON
                TOK_TYPE                                                                            { $$ = $4; xi_lex_searchspace_set($4); }
        |   TOK_TYPE                                                                                { $$ = $1; xi_lex_searchspace_set($1); }
        ;
q-type
        :   q-type-prefix                                                                           { $$ = $1; xi_lex_searchspace_clear(); }
                //TODO: optional template parameters
        ;
type-list-opt
        :   type-list                                                                               { $$ = $1; }
        |   %empty                                                                                  { $$ = new type_list_t(); }
        ;
type-list
        :   type-list OP_COMA type                                                                  { $$ = new type_list_t($1, $3); }
        |   type                                                                                    { $$ = new type_list_t($1); }
        ;
type-list-p1
        :   type-list OP_COMA type                                                                  { $$ = new type_list_t($1, $3); }
        ;

/* ---------- *
 * Statements *
 * ---------- */

stmt-list-opt
        :   stmt                                                                                    { builder.emit($1); }
                stmt-list-opt
        |   local-stmt                                                                              { builder.emit($1); }
        |   %empty
        ;
stmt
        :   block-stmt                                                                              { $$ = $1; }
        |   return-stmt         OP_SEMICOLON                                                        { $$ = $1; }
        |   continue-stmt       OP_SEMICOLON                                                        { $$ = $1; }
        |   break-stmt          OP_SEMICOLON                                                        { $$ = $1; }
        |   assign-stmt         OP_SEMICOLON                                                        { $$ = $1; }
        |   expr-stmt           OP_SEMICOLON                                                        { $$ = $1; }
        |   if-stmt                                                                                 { $$ = $1; }
        //|   while-stmt                                                                              { $$ = $1; }
        //|   for-stmt                                                                                { $$ = $1; }
        |   OP_SEMICOLON                                                                            { $$ = builder.make_nop_stmt(); }
        ;
local-stmt
        :   KW_LOCAL
                begin-block-stmt
                    local-stmt-cont
                OP_SEMICOLON
                stmt-list-opt                                                                       { $$ = SETLOC($2, @$); builder.pop(); }
        ;
local-stmt-cont
        :   unused-id                                       OP_ASSIGN   expr                        { builder.define_xi_local_inplace($1, nullptr, $3,      @$); }
        |   unused-id           OP_COLON    type                                                    { builder.define_xi_local_inplace($1, $3,      nullptr, @$); }
        |   unused-id           OP_COLON    type            OP_ASSIGN   expr                        { builder.define_xi_local_inplace($1, $3,      $5,      @$); }
        |   unused-id-list-p1                               OP_ASSIGN   expr-list-p1                { builder.define_xi_local_inplace($1, nullptr, $3,      @$); }
        |   unused-id-list-p1   OP_COLON    type-list-p1                                            { builder.define_xi_local_inplace($1, $3,      nullptr, @$); }
        |   unused-id-list-p1   OP_COLON    type-list-p1    OP_ASSIGN   expr-list-p1                { builder.define_xi_local_inplace($1, $3,      $5,      @$); }
        ;
noif-stmt
        :   return-stmt         OP_SEMICOLON                                                        { $$ = $1; }
        |   continue-stmt       OP_SEMICOLON                                                        { $$ = $1; }
        |   break-stmt          OP_SEMICOLON                                                        { $$ = $1; }
        |   assign-stmt         OP_SEMICOLON                                                        { $$ = $1; }
        |   expr-stmt           OP_SEMICOLON                                                        { $$ = $1; }
        //|   while-stmt                                                                              { $$ = $1; }
        //|   for-stmt                                                                                { $$ = $1; }
        |   OP_SEMICOLON                                                                            { $$ = builder.make_nop_stmt(); }
        ;
expr-stmt
        :   expr                                                                                    { $$ = SETLOC(builder.make_expr_stmt($1), @$); }
        ;
block-stmt
        :   OP_LBRACE
                begin-block-stmt                                                                    { builder.push_block($2->as<xcc::ast_block_stmt>()); }
                stmt-list-opt
            OP_RBRACE                                                                               { $$ = SETLOC($2, @$); builder.pop(); }
        ;
begin-block-stmt
        :   %empty                                                                                  { $$ = builder.make_block_stmt(); }
        ;
return-stmt
        :   KW_RETURN expr                                                                          { $$ = SETLOC(builder.make_return_stmt(builder.get_return_type(), $2), @$); }
        |   KW_RETURN                                                                               { $$ = SETLOC(builder.make_return_stmt(nullptr, nullptr), @$); }
        ;
continue-stmt
        :   KW_CONTINUE                                                                             { $$ = SETLOC(builder.make_continue_stmt(), @$); }
        ;
break-stmt
        :   KW_BREAK                                                                                { $$ = SETLOC(builder.make_break_stmt(), @$); }
        ;
assign-stmt
        :   expr assign-op expr                                                                     { $$ = SETLOC(builder.make_expr_stmt(
                                                                                                                builder.make_xi_op($2, $1, $3, @$)), @$); }
        ;
if-stmt
        :   KW_IF expr op-colon-noif-stmt                                                           { $$ = SETLOC(builder.make_if_stmt($2, $3, nullptr), @$); }
        |   KW_IF expr op-colon-noif-stmt else-stmt                                                 { $$ = SETLOC(builder.make_if_stmt($2, $3, $4),      @$); }
        |   KW_IF expr op-colon-noif-stmt elif-stmt                                                 { $$ = SETLOC(builder.make_if_stmt($2, $3, $4),      @$); }
        ;
else-stmt
        :   KW_ELSE op-colon-noif-stmt                                                              { $$ = $2; }
        ;
elif-stmt
        :   KW_ELIF expr op-colon-noif-stmt                                                         { $$ = SETLOC(builder.make_if_stmt($2, $3, nullptr), @$); }
        |   KW_ELIF expr op-colon-noif-stmt else-stmt                                               { $$ = SETLOC(builder.make_if_stmt($2, $3, $4),      @$); }
        |   KW_ELIF expr op-colon-noif-stmt elif-stmt                                               { $$ = SETLOC(builder.make_if_stmt($2, $3, $4),      @$); }
        ;
op-colon-noif-stmt
        :   OP_COLON noif-stmt                                                                      { $$ = $2; }
        |            block-stmt                                                                     { $$ = $1; }
        ;

/* ----------- *
 * Expressions *
 * ----------- */

expr                : land-expr                                                                     { $$ = $1; }
                    ;
land-expr           : land-expr     land-op lor-expr                                                { $$ = builder.make_xi_op($2, $1, $3, @$); }
                    | lor-expr                                                                      { $$ = $1; }
                    ;
lor-expr            : lor-expr      lor-op  loose-prefix-expr                                       { $$ = builder.make_xi_op($2, $1, $3, @$); }
                    | loose-prefix-expr                                                             { $$ = $1; }
                    ;
loose-prefix-expr   : loose-prefix-op loose-prefix-expr                                             { $$ = builder.make_xi_op($1, $2, @$); }
                    | cmp-expr                                                                      { $$ = $1; }
                    ;
cmp-expr            : band-expr     cmp-op  band-expr                                               { $$ = builder.make_xi_op($2, $1, $3, @$); }
                    | band-expr                                                                     { $$ = $1; }
                    ;
band-expr           : band-expr     band-op bor-expr                                                { $$ = builder.make_xi_op($2, $1, $3, @$); }
                    | bor-expr                                                                      { $$ = $1; }
                    ;
bor-expr            : bor-expr      bor-op  add-expr                                                { $$ = builder.make_xi_op($2, $1, $3, @$); }
                    | add-expr                                                                      { $$ = $1; }
                    ;
add-expr            : add-expr      add-op  shift-expr                                              { $$ = builder.make_xi_op($2, $1, $3, @$); }
                    | shift-expr                                                                    { $$ = $1; }
                    ;
shift-expr          : shift-expr    shift-op mul-expr                                               { $$ = builder.make_xi_op($2, $1, $3, @$); }
                    | mul-expr                                                                      { $$ = $1; }
                    ;
mul-expr            : mul-expr      mul-op  tight-prefix-expr                                       { $$ = builder.make_xi_op($2, $1, $3, @$); }
                    | tight-prefix-expr                                                             { $$ = $1; }
                    ;
tight-prefix-expr   : tight-prefix-op tight-prefix-expr                                             { $$ = builder.make_xi_op($1, $2, @$); }
                    | postfix-expr                                                                  { $$ = $1; }
                    ;
postfix-expr        : term-expr                                                                     { $$ = $1; }
                    | OP_LPAREN
                        expr
                        OP_RPAREN                                                                   { $$ = $2; }
                    ;
term-expr           : LITERAL_INTEGER                                                               { $$ = $1; }
                    | LITERAL_FLOAT                                                                 { $$ = $1; }
                    | LITERAL_STRING                                                                { $$ = $1; }
                    | TOK_EXPR                                                                      { $$ = $1; }
                    ;

expr-list-opt
                    : expr-list                                                                     { $$ = $1; }
                    | %empty                                                                        { $$ = new expr_list_t(); }
                    ;
expr-list
                    : expr-list OP_COMA expr                                                        { $$ = new expr_list_t($1, $3); }
                    | expr                                                                          { $$ = new expr_list_t($1); }
                    ;
expr-list-p1
                    : expr-list OP_COMA expr                                                        { $$ = new expr_list_t($1, $3); }
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

/* ------------------- *
 * Special Identifiers *
 * ------------------- */

unused-id
        :   /* set unused identifier */                                                             { xi_lex_state_push(XI_STATE_UNUSED_ID); }
            TOK_IDENTIFIER                                                                          { xi_lex_state_pop(XI_STATE_UNUSED_ID); $$ = $2; }
        ;
unused-id-list-opt
        :   unused-id-list                                                                          { $$ = $1; }
        |   %empty                                                                                  { $$ = new id_list_t(); }
        ;
unused-id-list
        :   unused-id-list OP_COMA unused-id                                                        { $$ = new id_list_t($1, $3); }
        |   unused-id                                                                               { $$ = new id_list_t($1); }
        ;
unused-id-list-p1
        :   unused-id-list OP_COMA unused-id                                                        { $$ = new id_list_t($1, $3); }
        ;

%%

void yyerror(XILTYPE* loc, xi_builder_t& builder, const char* msg) {
    printf("at %d:%d: %s\n", loc->first.line_number+1, loc->first.column_number+1, msg);
    assert(false);
}


