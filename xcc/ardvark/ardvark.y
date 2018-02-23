%glr-parser
%skeleton                                           "glr.cc"
%defines                                            "ardvark-parser.hpp"
%define     api.prefix                              {avr}
%define     parse.trace
%define     parse.error                             verbose

%locations

%code requires {
#include "avr_parsedefs.hpp"

#define YYSTYPE                         AVRSTYPE
#define YYLTYPE                         AVRLTYPE

#define YYLEX                           int yylex(YYSTYPE* yylval_param, XILTYPE* yyloc, avr_builder_t& builder)
}

%token
        TOK_EOF                         0   "end of file"

;
