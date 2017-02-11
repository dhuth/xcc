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


%code {
extern YY_DECL;
extern void yyerror(xi_builder_t& builder, const char* msg);
}
%param                              {xi_builder_t&          builder}

%start  translation-unit
%%

translation-unit
    : %empty
    ;

%%

void yyerror(xi_builder_t& builder, const char* msg) {
    printf("%s\n", msg);
}


