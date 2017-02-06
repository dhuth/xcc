%defines                                            "xi-parser.hpp"
%define     api.prefix                              {xi}
%define     api.pure

%code requires {
#include    "xi-internal.hpp"
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
        
        OP_PLUS                             "+"
        OP_MINUS                            "-"
        OP_STAR                             "*"
        
        KW_NAMESPACE                        "namespace"
        KW_TYPEDEF                          "typedef"
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
        const char*                         text;
}

%type   <text>                              TOK_IDENTIFIER
%type   <type>                              TOK_TYPE
%type   <expr>                              LITERAL_INTEGER
%type   <expr>                              LITERAL_FLOAT

%type   <type>                              type
%type   <type>                              postfix-type
%type   <type>                              prefix-type
%type   <type>                              basic-type


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
        ;
prefix-type
        :   basic-type                                                                              { $$ = $1; }
        ;
basic-type
        :   TOK_IDENTIFIER                                                                          { $$ = builder.get_named_type($1); }
        |   TOK_TYPE                                                                                { $$ = $1; }
        |   OP_LPAREN type OP_RPAREN                                                                { $$ = $2; }
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
        ;


global-variable
        :   type TOK_IDENTIFIER OP_SEMICOLON                                                       { tu.append(builder.define_variable($1, $2)); }
        ;


translation-unit
        :   global-decl-list-opt TOK_EOF
        ;
%%

void yyerror(xcc::translation_unit& tu, xi_builder_t& builder, const char* msg) {
    printf("%s\n", msg);
}


