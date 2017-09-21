%option noyywrap bison-bridge
%option prefix="xi"
%option header-file="xi-lex.hpp"

%option stack

%{
#include "xi-parser.hpp"
#include "source.hpp"

#define     SHIFTLOC                                    xcc::advance_column(yyloc->last, strlen(yytext));
#define     NEWLINE                                     xcc::advance_line(yyloc->last);
                                                        
#define     IGNORE                                      SHIFTLOC

#define     BEGIN_LOC                                   xcc::init_source_span(*yyloc, yyloc->last);
#define     RETURN_VALUE(tp,name,valname,val)           { SHIFTLOC yylval->valname = val; return tp##_##name; }
#define     RETURN_LOCATED_VALUE(tp,name,valname,val)   { SHIFTLOC yylval->valname = xcc::setloc(val, *yyloc); return tp##_##name; }
#define     RETURN_VOID(tp,name)                        { SHIFTLOC return tp##_##name; }

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
%s                                                      XI_STATE_UNUSED_ID

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

\{                                                      RETURN_OP(LBRACE,       none)
\}                                                      RETURN_OP(RBRACE,       none)
\[                                                      RETURN_OP(LBRACKET,     none)
\]                                                      RETURN_OP(RBRACKET,     none)
\(                                                      RETURN_OP(LPAREN,       none)
\)                                                      RETURN_OP(RPAREN,       none)

\-\>                                                    RETURN_OP(ARROW,        none)
\,                                                      RETURN_OP(COMA,         none)
\:                                                      RETURN_OP(COLON,        none)
\:\:                                                    RETURN_OP(DOUBLE_COLON, none)
\;                                                      RETURN_OP(SEMICOLON,    none)
\.                                                      RETURN_OP(DOT,          none)



        /* operators */

\+                                                      RETURN_OP(ADD,          add)
\-                                                      RETURN_OP(SUB,          sub)
\*                                                      RETURN_OP(MUL,          mul)
\/                                                      RETURN_OP(DIV,          div)
\%                                                      RETURN_OP(MOD,          mod)
\*\>                                                    RETURN_OP(SHR,          sr)
\<\*                                                    RETURN_OP(SHL,          sl)

\=\=                                                    RETURN_OP(EQ,           eq)
\!\=                                                    RETURN_OP(NE,           ne)
\<                                                      RETURN_OP(LT,           lt)
\>                                                      RETURN_OP(GT,           gt)
\<\=                                                    RETURN_OP(LE,           le)
\>\=                                                    RETURN_OP(GE,           ge)

\&                                                      RETURN_OP(BAND,         band)
\|                                                      RETURN_OP(BOR,          bor)
\^                                                      RETURN_OP(BXOR,         bxor)
\~                                                      RETURN_OP(BNOT,         bnot)
and                                                     RETURN_OP(LAND,         and)
or                                                      RETURN_OP(LOR,          or)
xor                                                     RETURN_OP(LXOR,         xor)
not                                                     RETURN_OP(LNOT,         not)

\=                                                      RETURN_OP(ASSIGN,       move)
\+\=                                                    RETURN_OP(ASSIGN_ADD,   iadd)
\-\=                                                    RETURN_OP(ASSIGN_SUB,   isub)
\*\=                                                    RETURN_OP(ASSIGN_MUL,   imul)
\/\=                                                    RETURN_OP(ASSIGN_DIV,   idiv)
\%\=                                                    RETURN_OP(ASSIGN_MOD,   imod)
\-\>\=                                                  RETURN_OP(ASSIGN_SHR,   isr)
\<\-\=                                                  RETURN_OP(ASSIGN_SHL,   isl)
\&\=                                                    RETURN_OP(ASSIGN_BAND,  iband)
\|\=                                                    RETURN_OP(ASSIGN_BOR,   ibor)
and\=                                                   RETURN_OP(ASSIGN_LAND,  iand)
or\=                                                    RETURN_OP(ASSIGN_LOR,   ior)

\[[ \t\n]*\]                                            RETURN_OP(INDEX,        index)
\([ \t\n]*\)                                            RETURN_OP(INVOKE,       invoke)


        /* builtin types */
void                                                    RETURN_TYPE(builder.get_void_type())
bool                                                    RETURN_TYPE(builder.get_bool_type())
i8                                                      RETURN_TYPE(builder.get_integer_type(8,  false))
i16                                                     RETURN_TYPE(builder.get_integer_type(16, false))
i32                                                     RETURN_TYPE(builder.get_integer_type(32, false))
i64                                                     RETURN_TYPE(builder.get_integer_type(64, false))
u8                                                      RETURN_TYPE(builder.get_integer_type(8,  true))
u16                                                     RETURN_TYPE(builder.get_integer_type(16, true))
u32                                                     RETURN_TYPE(builder.get_integer_type(32, true))
u64                                                     RETURN_TYPE(builder.get_integer_type(64, true))
f16                                                     RETURN_TYPE(builder.get_real_type(16))
f32                                                     RETURN_TYPE(builder.get_real_type(32))
f64                                                     RETURN_TYPE(builder.get_real_type(64))


        /* declaration scope keywords */

const                                                   RETURN_KW(CONST)
extern                                                  RETURN_KW(EXTERN)
export[ \t]+cfunc                                       RETURN_KW(EXPORT_CFUNC)
func                                                    RETURN_KW(FUNC)
import[ \t]+cfunc                                       RETURN_KW(IMPORT_CFUNC)
internal                                                RETURN_KW(INTERNAL)
namespace                                               RETURN_KW(NAMESPACE)
static                                                  RETURN_KW(STATIC)
struct                                                  RETURN_KW(STRUCT)
typedef                                                 RETURN_KW(TYPEDEF)


        /* statement scope keywords*/
        
elif                                                    RETURN_KW(ELIF)
else                                                    RETURN_KW(ELSE)
for                                                     RETURN_KW(FOR)
if                                                      RETURN_KW(IF)
in                                                      RETURN_KW(IN)
local                                                   RETURN_KW(LOCAL)
return                                                  RETURN_KW(RETURN)
while                                                   RETURN_KW(WHILE)


    /* expression scope keywords */

iter                                                    RETURN_KW(ITER)
range                                                   RETURN_KW(RANGE)
sizeof                                                  RETURN_KW(SIZEOF)


    /* constant literals */

true                                                    RETURN_LITERAL(INTEGER, builder.make_true())
false                                                   RETURN_LITERAL(INTEGER, builder.make_false())

{INTEGER_10_LITERAL}                                    RETURN_LOCATED_LITERAL(INTEGER, builder.make_integer(yytext, 10))
{INTEGER_16_LITERAL}                                    RETURN_LOCATED_LITERAL(INTEGER, builder.make_integer(&yytext[2], 16))
{FLOATING_POINT_LITERAL}                                RETURN_LOCATED_LITERAL(FLOAT,   builder.make_real(yytext))
{DBL_STRING_LITERAL}                                    RETURN_LOCATED_LITERAL(STRING,  builder.make_string(yytext, 1, strlen(yytext)-2))


    /* identifiers */


<XI_STATE_UNUSED_ID>{IDENTIFIER}                        RETURN_TOKEN(IDENTIFIER, text, yytext)

{IDENTIFIER}                                            {
                                                        }

<<EOF>>                                                 return TOK_EOF;
%%

void xi_lex_state_push(int state) {
    yy_push_state(state);
}

void xi_lex_state_pop(int state) {
    assert(state == YY_START);
    yy_pop_state();
}
