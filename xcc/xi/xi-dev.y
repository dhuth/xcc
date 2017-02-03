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
        
        OP_COMA                             ","
        OP_DOT                              "."
        
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


%code {
extern YY_DECL;
extern void yyerror(xcc::translation_unit& tu, xi_builder_t& builder, const char* msg);
}
%param                              {xcc::translation_unit& tu}
%param                              {xi_builder_t&          builder}

%start      translation-unit
%%
type
                :   TOK_TYPE
                ;

translation-unit
                :   global-decl-list-opt
                ;
global-decl-list-opt
                :   global-decl-list
                |   %empty
                ;
global-decl-list
                :   global-decl-list global-decl
                |   global-decl
                ;
global-decl
                :   global-function-decl
                ;
global-function-decl
                :   TOK_IDENTIFIER
                ;
%%

void yyerror(xcc::translation_unit& tu, xi_builder_t& builder, const char* msg) {
}


