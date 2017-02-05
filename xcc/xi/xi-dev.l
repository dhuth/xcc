%option noyywrap bison-bridge
%option prefix="xi"
%option header-file="xi-lex.hpp"

%{
#include "xi-parser.hpp"

#define     SHIFTLOC                                 /* ignore */
#define     IGNORE                                  SHIFTLOC;
#define     NEWLINE                                 /* ignore */
#define     RETURN_VALUE(tp,name,valname,val)       { SHIFTLOC; yylval->valname = val; return tp##_##name; }
#define     RETURN_VOID(tp,name)                    { SHIFTLOC; return tp##_##name; }

#define     RETURN_LITERAL(tp,val)                  RETURN_VALUE(LITERAL,tp,expr,val)
#define     RETURN_KW(name)                         RETURN_VOID(KW,name)
#define     RETURN_OP(name)                         RETURN_VOID(OP,name)
#define     RETURN_TOKEN(n,v,t)                     RETURN_VALUE(TOK,n,v,t)
#define     RETURN_TYPE(t)                          RETURN_VALUE(TOK,TYPE,type,t)


%}

IDENTIFIER                                          [a-zA-Z_][a-zA-Z0-9_]
DIGIT                                               [0-9]
DIGIT_HEX_LOWER                                     [0-9a-f]
DIGIT_HEX_UPPER                                     [0-9A-F]
HEX                                                 ({DIGIT_HEX_UPPER}+|{DIGIT_HEX_LOWER}+)

INTEGER_10_LITERAL                                  {DIGIT}+
INTEGER_16_LITERAL                                  0x{HEX}
FLOATING_POINT_LITERAL                              {DIGIT}+\.{DIGIT}*

%%
\n                                                  NEWLINE
[ \t]+                                              IGNORE

namespace                                           RETURN_KW(NAMESPACE)
typedef                                             RETURN_KW(TYPEDEF)

\{                                                  RETURN_OP(LBRACE)
\}                                                  RETURN_OP(RBRACE)
\[                                                  RETURN_OP(RBRACKET)
\]                                                  RETURN_OP(LBRACKET)
\(                                                  RETURN_OP(LPAREN)
\)                                                  RETURN_OP(RPAREN)

\,                                                  RETURN_OP(COMA)
\:                                                  RETURN_OP(COLON)
\:\:                                                RETURN_OP(DOUBLE_COLON)
\;                                                  RETURN_OP(SEMICOLON)
\.                                                  RETURN_OP(DOT)

\+                                                  RETURN_OP(PLUS)
\-                                                  RETURN_OP(MINUS)
\*                                                  RETURN_OP(STAR)

void                                                RETURN_TYPE(builder.get_void_type())
bool                                                RETURN_TYPE(builder.get_bool_type())
i8                                                  RETURN_TYPE(builder.get_integer_type(8,  false))
i16                                                 RETURN_TYPE(builder.get_integer_type(16, false))
i32                                                 RETURN_TYPE(builder.get_integer_type(32, false))
i64                                                 RETURN_TYPE(builder.get_integer_type(64, false))
u8                                                  RETURN_TYPE(builder.get_integer_type(8,  true))
u16                                                 RETURN_TYPE(builder.get_integer_type(16, true))
u32                                                 RETURN_TYPE(builder.get_integer_type(32, true))
u64                                                 RETURN_TYPE(builder.get_integer_type(64, true))
f16                                                 RETURN_TYPE(builder.get_real_type(16))
f32                                                 RETURN_TYPE(builder.get_real_type(32))
f64                                                 RETURN_TYPE(builder.get_real_type(64))

{INTEGER_10_LITERAL}                                RETURN_LITERAL(INTEGER, builder.make_integer(yytext, 10))
{INTEGER_16_LITERAL}                                RETURN_LITERAL(INTEGER, builder.make_integer(&yytext[2], 16))
{FLOATING_POINT_LITERAL}                            RETURN_LITERAL(FLOAT,   builder.make_real(yytext))

{IDENTIFIER}                                        RETURN_TOKEN(IDENTIFIER, text, strdup(yytext))

<<EOF>>                                             return TOK_EOF;
%%



