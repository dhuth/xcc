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
        
        OP_INDEX                            "[]"
        OP_INVOKE                           "()"
        
        
        KW_CLASS                            "class"
        KW_CONST                            "const"
        KW_EXPORT_CFUNC                     "export cfunc"
        KW_EXTERN                           "extern"
        KW_FUNC                             "func"
        KW_IMPORT_CFUNC                     "import cfunc"
        KW_INTERNAL                         "internal"
        KW_NAMESPACE                        "namespace"
        KW_STATIC                           "static"
        KW_STRUCT                           "struct"
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
        KW_RANGE                            "range"
        KW_SIZEOF                           "sizeof"
        KW_TYPEOF                           "typeof"
        KW_ZIP                              "zip"
;

%union {
        xcc::ast_expr*                      expr;
        xcc::ast_decl*                      decl;
        xcc::ast_type*                      type;
        xcc::ast_stmt*                      stmt;
        xcc::ast_namespace_decl*            namespace_decl;
        xcc::list<xcc::ast_expr>*           expr_list;
        xcc::list<xcc::ast_decl>*           decl_list;
        xcc::list<xcc::ast_type>*           type_list;
        xcc::list<xcc::ast_stmt>*           stmt_list;
        
        xcc::xi_function_decl*              function;
        xcc::xi_parameter_decl*             parameter;
        xcc::xi_struct_decl*                struct_decl;
        xcc::ast_local_decl*                local;
        xcc::list<xcc::xi_parameter_decl>*  parameter_list;
        
        xcc::xi_operator                    op;
        const char*                         text;
}

%type   <text>                              TOK_IDENTIFIER
%type   <text>                              unused-id
%type   <decl>                              TOK_DECL
%type   <namespace_decl>                    TOK_NAMESPACE
%type   <type>                              TOK_TYPE
%type   <expr>                              TOK_EXPR

%type   <expr>                              LITERAL_INTEGER
%type   <expr>                              LITERAL_FLOAT
%type   <expr>                              LITERAL_STRING

%type   <type>                              type
%type   <type>                              postfix-type
%type   <type>                              prefix-type
%type   <type>                              term-type
%type   <type>                              name-type
%type   <type>                              type-explicit
%type   <expr>                              dim-expr


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

%type   <namespace_decl>                    namespace-name

%type   <function>                          global-function-proto-decl
%type   <function>                          global-function-decl-rest
%type   <struct_decl>                       global-struct-proto-decl

%type   <decl>                              struct-field-decl

%type   <parameter_list>                    function-parameter-list-opt
%type   <parameter_list>                    function-parameter-list
%type   <parameter>                         function-parameter

%type   <type_list>                         base-type-list-opt
%type   <type_list>                         base-type-list

%type   <stmt>                              stmt
%type   <stmt>                              no-if-stmt
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
%type   <stmt>                              expr-stmt
%type   <stmt>                              op-colon-stmt
%type   <stmt>                              op-colon-noif-stmt

%type   <stmt>                              begin-block-stmt

%type   <local>                             for-init-decl


%code {
extern YY_DECL;
void yyerror(XILTYPE* loc, xi_builder_t& builder, const char* msg);

extern void xi_push_stmt_state();
extern void xi_pop_stmt_state();

extern void xi_push_decl_state();
extern void xi_pop_decl_state();

extern void xi_push_struct_decl_state();
extern void xi_pop_struct_decl_state();

extern void xi_push_type_state();
extern void xi_pop_type_state();

extern void xi_push_expr_state();
extern void xi_pop_expr_state();

extern void xi_push_unused_id_state();
extern void xi_pop_unused_id_state();

extern void xi_ready_namespace_member(xcc::ast_namespace_decl*);
extern void xi_clear_namespace_member();

extern void xi_ready_static_type_member(xcc::ast_type*);
extern void xi_clear_static_type_member();

}
%param                              {xi_builder_t&          builder}

%start  translation-unit
%%

unused-id
        : { xi_push_unused_id_state(); }
            TOK_IDENTIFIER
          { xi_pop_unused_id_state(); $$ = $2; }
        ;

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
        | global-struct-decl
        | global-typedef-decl
        ;

global-namespace-decl
        : KW_NAMESPACE global-namespace-decl-cont
        ;
        
global-namespace-decl-cont
        : unused-id                         { builder.push_namespace(builder.define_namespace($1)); }
            OP_LBRACE
            global-decl-list
            OP_RBRACE                       { builder.pop(); }
        | unused-id OP_DOUBLE_COLON         { builder.push_namespace(builder.define_namespace($1)); }
            global-namespace-decl-cont      { builder.pop(); }
        ;
global-typedef-decl
        : KW_TYPEDEF type unused-id OP_SEMICOLON                                                    { builder.setloc(builder.define_named_type($3, $2), @$); }
        ;
global-variable-decl
        : KW_EXTERN type unused-id      OP_SEMICOLON                                                { auto var = builder.setloc(builder.define_global_variable($2, $3), @$); var->is_extern = true; }
        |           type unused-id      OP_SEMICOLON                                                {            builder.setloc(builder.define_global_variable($1, $2), @$);     }
        |           type unused-id      OP_EQ expr OP_SEMICOLON                                     {            builder.setloc(builder.define_global_variable($1, $2, $4), @$); }
        ;
global-function-decl
        : KW_EXTERN       KW_FUNC global-function-proto-decl OP_SEMICOLON                           { builder.setloc($3, @$); $3->is_extern = true; }
        |                 KW_FUNC global-function-proto-decl OP_SEMICOLON                           { builder.setloc($2, @$); }
        |                 KW_FUNC global-function-decl-rest                                         { /* do nothing */ }
        | KW_IMPORT_CFUNC     global-function-proto-decl OP_SEMICOLON                               { builder.setloc($2, @$); $2->is_extern = true; $2->is_c_extern = true; }
        | KW_EXPORT_CFUNC     global-function-decl-rest                                             { builder.setloc($2, @$); $2->is_c_extern = true; }
        | KW_INTERNAL     KW_FUNC global-function-proto-decl OP_SEMICOLON                           { builder.setloc($3, @$); $3->is_extern_visible = false; }
        | KW_INTERNAL     KW_FUNC global-function-decl-rest                                         { builder.setloc($3, @$); $3->is_extern_visible = false; }
        ;
global-function-decl-rest
        : global-function-proto-decl
            OP_LBRACE push-stmt-state { builder.push_function_and_body($1); }
              stmt-list-opt
            OP_RBRACE pop-stmt-state  { builder.pop_function_and_body(); builder.setloc($1, @$); $$ = $1; }
        ;
global-struct-decl
        : global-struct-proto-decl OP_SEMICOLON                                                     { builder.setloc($1, @$); }
        | global-struct-proto-decl
                OP_LBRACE push-struct-decl-state { builder.push_member($1); }
                    struct-member-list-opt
                OP_RBRACE pop-struct-decl-state  { builder.pop(); builder.setloc($1, @$); }
        ;
global-struct-proto-decl
        : KW_STRUCT unused-id      base-type-list-opt                                               { $$ = builder.setloc(builder.define_global_struct($2, $3), @$); }
        ;
global-function-proto-decl
        : type-explicit unused-id      OP_LPAREN function-parameter-list-opt OP_RPAREN              { $$ = builder.setloc(builder.define_global_function($1, $2, $4), @$); }
        ;

base-type-list-opt
        : OP_COLON base-type-list                                                                   { $$ = $2; }
        | %empty                                                                                    { $$ = new xcc::list<xcc::ast_type>(); }
        ;
base-type-list
        : base-type-list OP_COMA name-type                                                          { xi_clear_static_type_member(); $1->append($3); $$ = $1; }
        | name-type                                                                                 { xi_clear_static_type_member(); $$ = new xcc::list<xcc::ast_type>($1); }
        ;

push-struct-decl-state
        : %empty    { xi_push_struct_decl_state(); }
        ;
pop-struct-decl-state
        : %empty    { xi_pop_struct_decl_state(); }
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
        : type-explicit unused-id                                                                   { $$ = builder.setloc(builder.define_parameter($1, $2), @$); }
        | type-explicit                                                                             { $$ = builder.setloc(builder.define_parameter($1), @$);     }
        ;




struct-member-list-opt
        : struct-member-list
        | %empty
        ;
struct-member-list
        : struct-member-decl struct-member-list
        | struct-member-decl
        ;
struct-member-decl
        : struct-field-decl
        ;
struct-field-decl
        : KW_STATIC type unused-id      OP_ASSIGN expr OP_SEMICOLON                                 { $$ = builder.setloc( builder.define_field($2, $3, $5, true),  @$); }
        | KW_STATIC type unused-id      OP_SEMICOLON                                                { $$ = builder.setloc( builder.define_field($2, $3, true),      @$); }
        |           type unused-id      OP_ASSIGN expr OP_SEMICOLON                                 { $$ = builder.setloc( builder.define_field($1, $2, $4, false), @$); }
        |           type unused-id      OP_SEMICOLON                                                { $$ = builder.setloc( builder.define_field($1, $2, false),     @$); }
        ;

namespace-name
        : TOK_NAMESPACE                                                                             { xi_ready_namespace_member($1); $$ = $1; }
        | namespace-name OP_DOUBLE_COLON TOK_NAMESPACE                                              { xi_ready_namespace_member($3); $$ = $3; }
        ;


type
        : postfix-type                                                                              { $$ = $1; }
        ;
postfix-type
        : postfix-type OP_LBRACKET dim-expr-list OP_RBRACKET                                        { $$ = builder.get_array_type($1, $3); }
        | prefix-type  OP_BAND                                                                      { $$ = builder.get_ref_type($1); }
        | prefix-type                                                                               { $$ = $1; }
        ;
prefix-type
        : term-type                                                                                 { $$ = $1; }
        | KW_CONST prefix-type                                                                      { $$ = builder.get_const_type($2); }
        ;
term-type
        : name-type                                                                                 { xi_clear_static_type_member();  $$ = $1; }
        | OP_LPAREN type OP_RPAREN                                                                  { $$ = $2; }
        ;
name-type
        : TOK_TYPE                                                                                  { $$ = $1; }
        | name-type      OP_DOUBLE_COLON TOK_TYPE                                                   { xi_ready_static_type_member($3); $$ = $3; }
        | namespace-name OP_DOUBLE_COLON TOK_TYPE                                                   { xi_clear_namespace_member();     $$ = $3; }
        ;
type-explicit
        : { xi_push_type_state(); }
            type
          { xi_pop_type_state(); $$ = $2; }
        ;

dim-expr-list
        : dim-expr-list OP_COMA                                                                     { $1->append(nullptr); $$=$1;            }
        | dim-expr-list OP_COMA dim-expr                                                            { $1->append($3);      $$=$1;            }
        | dim-expr                                                                                  { $$ = new xcc::list<xcc::ast_expr>($1); }
        | %empty                                                                                    { $$ = new xcc::list<xcc::ast_expr>();   }
        ;
dim-expr
        : { xi_push_expr_state(); }
            expr
          { xi_pop_expr_state(); $$ = $2; }
        ;




stmt-list-opt
        : stmt                { builder.emit($1); } stmt-list-opt
        | local-stmt          { builder.emit($1); }
        | %empty
        ;

push-stmt-state
        : %empty      { xi_push_stmt_state(); }
        ;
pop-stmt-state
        : %empty      { xi_pop_stmt_state(); }
        ;
        
stmt
        : block-stmt                                                                                { $$ = $1; }
        | return-stmt OP_SEMICOLON                                                                  { $$ = $1; }
        | continue-stmt OP_SEMICOLON                                                                { $$ = $1; }
        | break-stmt OP_SEMICOLON                                                                   { $$ = $1; }
        | assign-stmt OP_SEMICOLON                                                                  { $$ = $1; }
        | expr-stmt OP_SEMICOLON                                                                    { $$ = $1; }
        | if-stmt                                                                                   { $$ = $1; }
        | while-stmt                                                                                { $$ = $1; }
        | for-stmt                                                                                  { $$ = $1; }
        | OP_SEMICOLON                                                                              { $$ = builder.make_nop_stmt(); }
        ;
no-if-stmt
        : block-stmt                                                                                { $$ = $1; }
        | return-stmt OP_SEMICOLON                                                                  { $$ = $1; }
        | continue-stmt OP_SEMICOLON                                                                { $$ = $1; }
        | break-stmt OP_SEMICOLON                                                                   { $$ = $1; }
        | assign-stmt OP_SEMICOLON                                                                  { $$ = $1; }
        | expr-stmt OP_SEMICOLON                                                                    { $$ = $1; }
        | while-stmt                                                                                { $$ = $1; }
        | for-stmt                                                                                  { $$ = $1; }
        | OP_SEMICOLON                                                                              { $$ = builder.make_nop_stmt(); }
        ;
op-colon-stmt
        : OP_COLON stmt                                                                             { $$ = $2; }
        |          block-stmt                                                                       { $$ = $1; }
        ;
op-colon-noif-stmt
        : OP_COLON no-if-stmt                                                                       { $$ = $2; }
        |          block-stmt                                                                       { $$ = $1; }
        ;
block-stmt
        : OP_LBRACE begin-block-stmt { builder.push_block($2->as<xcc::ast_block_stmt>()); }
            stmt-list-opt
          OP_RBRACE { $$ = builder.setloc($2, @$); builder.pop(); }
        ;
begin-block-stmt
        : %empty { $$ = builder.make_block_stmt(); }
        ;
return-stmt
        : KW_RETURN                                                                                 { $$ = builder.setloc(builder.make_return_stmt(nullptr, nullptr), @$);   }
        | KW_RETURN expr                                                                            { $$ = builder.setloc(builder.make_return_stmt(builder.get_return_type(), $2), @$); }
        ;
continue-stmt
        : KW_CONTINUE                                                                               { $$ = builder.make_continue_stmt(); }
        ;
break-stmt
        : KW_BREAK                                                                                  { $$ = builder.make_break_stmt(); }
        ;
assign-stmt
        : expr assign-op expr                                                                       { $$ = builder.setloc(builder.make_assign_stmt($2, $1, $3), @$); }
        ;
expr-stmt
        : expr                                                                                      { $$ = builder.setloc(builder.make_expr_stmt($1), @$); }
        ;
        
if-stmt
        : KW_IF expr op-colon-noif-stmt                                                             { $$ = builder.setloc(builder.make_if_stmt($2, $3, nullptr), @$); }
        | KW_IF expr op-colon-noif-stmt else-stmt                                                   { $$ = builder.setloc(builder.make_if_stmt($2, $3, $4), @$);      }
        | KW_IF expr op-colon-noif-stmt elif-stmt                                                   { $$ = builder.setloc(builder.make_if_stmt($2, $3, $4), @$);      }
        ;
else-stmt
        : KW_ELSE op-colon-noif-stmt                                                                { $$ = $2; }
        ;
elif-stmt
        : KW_ELIF expr op-colon-noif-stmt                                                           { $$ = builder.setloc(builder.make_if_stmt($2, $3, nullptr), @$); }
        | KW_ELIF expr op-colon-noif-stmt else-stmt                                                 { $$ = builder.setloc(builder.make_if_stmt($2, $3, $4), @$);      }
        | KW_ELIF expr op-colon-noif-stmt elif-stmt                                                 { $$ = builder.setloc(builder.make_if_stmt($2, $3, $4), @$);      }
        ;
while-stmt
        : KW_WHILE expr op-colon-stmt                                                               { $$ = builder.setloc(builder.make_while_stmt($2, $3), @$); }
        ;
local-stmt
        : type unused-id      OP_SEMICOLON begin-block-stmt
                {
                    builder.push_block($4->as<xcc::ast_block_stmt>());
                    auto vardecl = builder.setloc(builder.define_local_variable($1, $2), @1, @3);
                    builder.emit(
                            builder.setloc(
                                builder.make_assign_stmt(
                                    xcc::xi_operator::assign,
                                    builder.setloc(builder.make_declref_expr(vardecl), @2),
                                    builder.make_default_initializer($1)),
                                @1, @2));
                }
            stmt-list-opt
                { $$ = $4; builder.pop(); }
        | type unused-id      OP_ASSIGN expr OP_SEMICOLON begin-block-stmt
                {
                    builder.push_block($6->as<xcc::ast_block_stmt>());
                    auto vardecl = builder.setloc(builder.define_local_variable($1, $2), @1, @5);
                    builder.emit(
                            builder.setloc(
                                builder.make_assign_stmt(
                                    xcc::xi_operator::assign,
                                    builder.setloc(builder.make_declref_expr(vardecl), @2),
                                    $4),
                                @1, @3));
                }
            stmt-list-opt
                { $$ = builder.setloc($6, @$); builder.pop(); }
        ;
for-stmt
        : KW_FOR begin-block-stmt
                { builder.push_block($2->as<xcc::ast_block_stmt>()); }
            for-init-decl KW_IN expr
            op-colon-stmt
                {
                    builder.emit(builder.setloc(builder.make_for_stmt($4, $6, $7), @1, @7));
                    builder.pop();
                    $$ = builder.setloc($2, @$);
                }
        ;
for-init-decl
        : type unused-id      { $$ = builder.define_local_variable($1, $2); }
        ;




expr                : land-expr                                 { $$ = $1; }
                    ;
land-expr           : land-expr     land-op lor-expr            { $$ = builder.setloc(builder.make_op($2, $1, $3), @$); }
                    | lor-expr                                  { $$ = $1; }
                    ;
lor-expr            : lor-expr      lor-op  loose-prefix-expr   { $$ = builder.setloc(builder.make_op($2, $1, $3), @$); }
                    | loose-prefix-expr                         { $$ = $1; }
                    ;
loose-prefix-expr   : loose-prefix-op loose-prefix-expr         { $$ = builder.setloc(builder.make_op($1, $2), @$); }
                    | cmp-expr                                  { $$ = $1; }
                    ;
cmp-expr            : band-expr     cmp-op  band-expr           { $$ = builder.setloc(builder.make_op($2, $1, $3), @$); }
                    | band-expr                                 { $$ = $1; }
                    ;
band-expr           : band-expr     band-op bor-expr            { $$ = builder.setloc(builder.make_op($2, $1, $3), @$); }
                    | bor-expr                                  { $$ = $1; }
                    ;
bor-expr            : bor-expr      bor-op  add-expr            { $$ = builder.setloc(builder.make_op($2, $1, $3), @$); }
                    | add-expr                                  { $$ = $1; }
                    ;
add-expr            : add-expr      add-op  shift-expr          { $$ = builder.setloc(builder.make_op($2, $1, $3), @$); }
                    | shift-expr                                { $$ = $1; }
                    ;
shift-expr          : shift-expr    shift-op mul-expr           { $$ = builder.setloc(builder.make_op($2, $1, $3), @$); }
                    | mul-expr                                  { $$ = $1; }
                    ;
mul-expr            : mul-expr      mul-op  tight-prefix-expr   { $$ = builder.setloc(builder.make_op($2, $1, $3), @$); }
                    | tight-prefix-expr                         { $$ = $1; }
                    ;
tight-prefix-expr   : tight-prefix-op tight-prefix-expr         { $$ = builder.setloc(builder.make_op($1, $2), @$); }
                    | postfix-expr                              { $$ = $1; }
                    ;
postfix-expr        : postfix-expr OP_LPAREN expr-list-opt OP_RPAREN            { $$ = builder.setloc(builder.make_call_expr($1, $3), @$);      }
                    | name-type    OP_LPAREN expr-list-opt OP_RPAREN            { $$ = builder.setloc(builder.make_ctor_expr($1, $3), @$);      }
                    | postfix-expr OP_LBRACKET expr-list OP_RBRACKET            { $$ = builder.setloc(builder.make_index_expr($1, $3), @$);     }
                    | postfix-expr OP_DOT      unused-id                        { $$ = builder.setloc(builder.make_memberref_expr($1, $3), @$); }
                    | name-type    OP_DOUBLE_COLON TOK_EXPR                     { xi_clear_static_type_member(); $$ = $3; }
                    | term-expr                                                 { $$ = $1; }
                    ;
term-expr           : LITERAL_INTEGER                           { $$ = $1; }
                    | LITERAL_FLOAT                             { $$ = $1; }
                    | LITERAL_STRING                            { $$ = $1; }
                    | TOK_EXPR                                  { $$ = $1; }
                    ;

expr-list-opt
                    : expr-list                                 { $$ = $1; }
                    | %empty                                    { $$ = new xcc::list<xcc::ast_expr>(); }
                    ;
expr-list
                    : expr-list OP_COMA expr                    { $1->append($3); $$ = $1; }
                    | expr                                      { $$ = new xcc::list<xcc::ast_expr>($1); }
                    ;

//type-or-expr
//                    : type
//                    | expr
//                    | TOK_IDENTIFIER
//                    ;




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
    printf("at %d:%d: %s\n", loc->first.line_number+1, loc->first.column_number+1, msg);
    assert(false);
}


