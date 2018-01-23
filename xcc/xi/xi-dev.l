%option noyywrap bison-bridge
%option prefix="xi"
%option header-file="xi-lex.hpp"

%option stack

%{
#include "xi-parser.hpp"
#include "source.hpp"
#include <cstring>
#include <ctype.h>

#define     SHIFTLOC                                    yyloc->columns(strlen(yytext));
#define     NEWLINE                                     yyloc->lines(1);
                                                        
#define     IGNORE                                      SHIFTLOC

#define     BEGIN_LOC                                   yyloc->step();
#define     RETURN_VALUE(tp,name,valname,val)           { SHIFTLOC yylval->valname = val; return xi::parser::token::tp##_##name; }
#define     RETURN_LOCATED_VALUE(tp,name,valname,val)   { SHIFTLOC yylval->valname = xcc::setloc(val, *yyloc); return xi::parser::token::tp##_##name; }
#define     RETURN_VOID(tp,name)                        { SHIFTLOC return xi::parser::token::tp##_##name; }

#define     RETURN_LITERAL(tp,val)                      RETURN_VALUE(LITERAL,tp,expr,val)
#define     RETURN_LOCATED_LITERAL(tp,val)              RETURN_LOCATED_VALUE(LITERAL,tp,expr,val)
#define     RETURN_KW(name)                             RETURN_VOID(KW,name)
#define     RETURN_OP(name, val)                        RETURN_VALUE(OP, name, op, operator_t::__##val##__)
#define     RETURN_TOKEN(n,v,t)                         RETURN_VALUE(TOK,n,v,t)
#define     RETURN_LOCATED_TOKEN(n,v,t)                 RETURN_LOCATED_VALUE(TOK,n,v,t)
#define     RETURN_TYPE(t)                              RETURN_VALUE(TOK,TYPE,type,t)
#define     RETURN_DECL(t)                              RETURN_VALUE(TOK,DECL,decl,t)
#define     RETURN_EXPR(e)                              RETURN_VALUE(TOK,EXPR,expr,e)
#define     RETURN_LOCATED_EXPR(e)                      RETURN_LOCATED_VALUE(TOK,EXPR,expr,e)

static inline char* copy_cstring(const char* s) {
    char* d = (char*) malloc(strlen(s) + 1);
    strcpy(d, s);
    return d;
}

%}

IDENTIFIER                                              [a-zA-Z_][a-zA-Z0-9_]*
DIGIT                                                   [0-9]
DIGIT_HEX_LOWER                                         [0-9a-f]
DIGIT_HEX_UPPER                                         [0-9A-F]
HEX                                                     ({DIGIT_HEX_UPPER}+|{DIGIT_HEX_LOWER}+)

FP_FRONT                                                {DIGIT}+\.{DIGIT}*
FP_BACK                                                 [Ee][+-]?{DIGIT}+

INTEGER_10_LITERAL                                      {DIGIT}+
INTEGER_16_LITERAL                                      0x{HEX}
FLOATING_POINT_LITERAL                                  ({FP_FRONT}({FP_BACK})?)|({DIGIT}+{FP_BACK})

DBL_STRING_CHAR                                         [^\\"]
DBL_STRING_ESCAPE                                       \\.
DBL_STRING_LITERAL                                      ["]({DBL_STRING_CHAR}|{DBL_STRING_ESCAPE})*["]


%x                                                      XI_STATE_BLOCK_COMMENT

%%
%{
    BEGIN_LOC
%}
        /* white space */
\n                                                      NEWLINE
[ \t]+                                                  IGNORE


        /* comments */
\\"*"                                                   { SHIFTLOC; yy_push_state(XI_STATE_BLOCK_COMMENT); }
<XI_STATE_BLOCK_COMMENT>"*/"                            { SHIFTLOC; yy_pop_state(); }
<XI_STATE_BLOCK_COMMENT>\n                              NEWLINE
<XI_STATE_BLOCK_COMMENT>.                               IGNORE
"//"[^\n]*\n                                            NEWLINE


            /* Delimiters */

\{                                                      RETURN_OP(LBRACE,               none)
\}                                                      RETURN_OP(RBRACE,               none)
\[                                                      RETURN_OP(LBRACKET,             none)
\]                                                      RETURN_OP(RBRACKET,             none)
\(                                                      RETURN_OP(LPAREN,               none)
\)                                                      RETURN_OP(RPAREN,               none)

\-\>                                                    RETURN_OP(ARROW,                none)
\,                                                      RETURN_OP(COMA,                 none)
\:                                                      RETURN_OP(COLON,                none)
\:\:                                                    RETURN_OP(DOUBLE_COLON,         none)
\;                                                      RETURN_OP(SEMICOLON,            none)
\.                                                      RETURN_OP(DOT,                  none)

            /* operators */

and                                                     RETURN_OP(LOGICAL_AND,          and)
or                                                      RETURN_OP(LOGICAL_OR,           or)
xor                                                     RETURN_OP(LOGICAL_XOR,          xor)
not                                                     RETURN_OP(LOGICAL_NOT,          not)

\+                                                      RETURN_OP(ADD,                  add)
\-                                                      RETURN_OP(SUB,                  sub)
\*                                                      RETURN_OP(MUL,                  mul)
\/                                                      RETURN_OP(DIV,                  div)
\%                                                      RETURN_OP(MOD,                  mod)

sl                                                      RETURN_OP(SL,                   sl)
sr                                                      RETURN_OP(SR,                   sr)
\&                                                      RETURN_OP(BINARY_AND,           band)
\|                                                      RETURN_OP(BINARY_OR,            bor)
\^                                                      RETURN_OP(BINARY_XOR,           bxor)
\~                                                      RETURN_OP(BINARY_NOT,           bnot)

\<                                                      RETURN_OP(LT,                   lt)
\<\=                                                    RETURN_OP(LE,                   le)
\>                                                      RETURN_OP(GT,                   gt)
\>\=                                                    RETURN_OP(GE,                   ge)
\=\=                                                    RETURN_OP(EQ,                   eq)
\!\=                                                    RETURN_OP(NE,                   ne)

\=                                                      RETURN_OP(ASSIGN,               assign)
\+\=                                                    RETURN_OP(ASSIGN_ADD,           iadd)
\-\=                                                    RETURN_OP(ASSIGN_SUB,           isub)
\*\=                                                    RETURN_OP(ASSIGN_MUL,           imul)
\/\=                                                    RETURN_OP(ASSIGN_DIV,           idiv)
\%\=                                                    RETURN_OP(ASSIGN_MOD,           imod)
sl\=                                                    RETURN_OP(ASSIGN_SL,            isl)
sr\=                                                    RETURN_OP(ASSIGN_SR,            isr)
\&\=                                                    RETURN_OP(ASSIGN_BINARY_AND,    iband)
\|\=                                                    RETURN_OP(ASSIGN_BINARY_OR,     ibor)
\^\=                                                    RETURN_OP(ASSIGN_BINARY_XOR,    ibxor)
and\=                                                   RETURN_OP(ASSIGN_LOGICAL_AND,   iand)
or\=                                                    RETURN_OP(ASSIGN_LOGICAL_OR,    ior)
xor\=                                                   RETURN_OP(ASSIGN_LOGICAL_XOR,   ixor)


            /* top level keywords */

const                                                   RETURN_KW(CONST)
func                                                    RETURN_KW(FUNC)
namespace                                               RETURN_KW(NAMESPACE)
struct                                                  RETURN_KW(STRUCT)


            /* type keywords */
            
void                                                    RETURN_TYPE(builder.get_void_type())
bool                                                    RETURN_TYPE(builder.get_bool_type())
u8                                                      RETURN_TYPE(builder.get_integer_type(8,   true))
u16                                                     RETURN_TYPE(builder.get_integer_type(16,  true))
u32                                                     RETURN_TYPE(builder.get_integer_type(32,  true))
u64                                                     RETURN_TYPE(builder.get_integer_type(64,  true))
i8                                                      RETURN_TYPE(builder.get_integer_type(8,   false))
i16                                                     RETURN_TYPE(builder.get_integer_type(16,  false))
i32                                                     RETURN_TYPE(builder.get_integer_type(32,  false))
i64                                                     RETURN_TYPE(builder.get_integer_type(64,  false))
f32                                                     RETURN_TYPE(builder.get_real_type(32));
f64                                                     RETURN_TYPE(builder.get_real_type(64));

            /* statement keywords */

break                                                   RETURN_KW(BREAK)
continue                                                RETURN_KW(CONTINUE)
else                                                    RETURN_KW(ELSE)
elif                                                    RETURN_KW(ELIF)
for                                                     RETURN_KW(FOR)
if                                                      RETURN_KW(IF)
return                                                  RETURN_KW(RETURN)
while                                                   RETURN_KW(WHILE)
var                                                     RETURN_KW(VAR)

            /* expression keywords */
            /* literals */

{FLOATING_POINT_LITERAL}                                RETURN_LITERAL(REAL,    builder.make_real(yytext))
{INTEGER_10_LITERAL}                                    RETURN_LITERAL(INTEGER, builder.make_integer(yytext, 10))
{INTEGER_16_LITERAL}                                    RETURN_LITERAL(INTEGER, builder.make_integer(&yytext[2], 16))
true                                                    RETURN_LITERAL(BOOL,    builder.make_true())
false                                                   RETURN_LITERAL(BOOL,    builder.make_false())
{IDENTIFIER}                                            RETURN_TOKEN(IDENTIFIER, text, copy_cstring(yytext))

<<EOF>>                                                 return xi::parser::token::TOK_EOF;
%%

