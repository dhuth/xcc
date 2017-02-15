%option noyywrap bison-bridge
%option prefix="xi"
%option header-file="xi-lex.hpp"

%{
#include "xi-parser.hpp"

#define     SHIFTLOC                                 /* ignore */
#define     IGNORE                                  SHIFTLOC
#define     NEWLINE                                 /* ignore */
#define     RETURN_VALUE(tp,name,valname,val)       { SHIFTLOC yylval->valname = val; return tp##_##name; }
#define     RETURN_VOID(tp,name)                    { SHIFTLOC return tp##_##name; }

#define     RETURN_LITERAL(tp,val)                  RETURN_VALUE(LITERAL,tp,expr,val)
#define     RETURN_KW(name)                         RETURN_VOID(KW,name)
#define     RETURN_OP(name, val)                    RETURN_VALUE(OP, name, op, xcc::xi_operator::val)
#define     RETURN_TOKEN(n,v,t)                     RETURN_VALUE(TOK,n,v,t)
#define     RETURN_TYPE(t)                          RETURN_VALUE(TOK,TYPE,type,t)
#define     RETURN_DECL(t)                          RETURN_VALUE(TOK,DECL,decl,t)

#define     RESET_IDENTIFIER_CHECK                  \
                                                    {\
                                                        xi_parse_raw_identifier = false;\
                                                    }

%}

IDENTIFIER                                          [a-zA-Z_][a-zA-Z0-9_]*
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

\{                                                  RETURN_OP(LBRACE,       none)
\}                                                  RETURN_OP(RBRACE,       none)
\[                                                  RETURN_OP(LBRACKET,     none)
\]                                                  RETURN_OP(RBRACKET,     none)
\(                                                  RETURN_OP(LPAREN,       none)
\)                                                  RETURN_OP(RPAREN,       none)

\,                                                  RETURN_OP(COMA,         none)
\:                                                  RETURN_OP(COLON,        none)
\:\:                                                RETURN_OP(DOUBLE_COLON, none)
\;                                                  RETURN_OP(SEMICOLON,    none)
\.                                                  RETURN_OP(DOT,          dot)

\+                                                  RETURN_OP(ADD,          add)
\-                                                  RETURN_OP(SUB,          sub)
\*                                                  RETURN_OP(MUL,          mul)
\/                                                  RETURN_OP(DIV,          div)
\%                                                  RETURN_OP(MOD,          mod)
\>\>                                                RETURN_OP(SHR,          shr)
\<\<                                                RETURN_OP(SHL,          shl)

\=\=                                                RETURN_OP(EQ,           eq)
\!\=                                                RETURN_OP(NE,           ne)
\<                                                  RETURN_OP(LT,           lt)
\>                                                  RETURN_OP(GT,           gt)
\<\=                                                RETURN_OP(LE,           le)
\>\=                                                RETURN_OP(GE,           ge)

\=                                                  RETURN_OP(ASSIGN,       assign)
\+\=                                                RETURN_OP(ASSIGN_ADD,   assign_add)
\-\=                                                RETURN_OP(ASSIGN_SUB,   assign_sub)
\*\=                                                RETURN_OP(ASSIGN_MUL,   assign_mul)
\/\=                                                RETURN_OP(ASSIGN_DIV,   assign_div)
\%\=                                                RETURN_OP(ASSIGN_MOD,   assign_mod)

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

auto                                                RETURN_KW(AUTO)
byref                                               RETURN_KW(BYREF)
const                                               RETURN_KW(CONST)
extern                                              RETURN_KW(EXTERN)
for                                                 RETURN_KW(FOR)
func                                                RETURN_KW(FUNC)
generic                                             RETURN_KW(GENERIC)
if                                                  RETURN_KW(IF)
in                                                  RETURN_KW(IN)
interface                                           RETURN_KW(INTERFACE)
namespace                                           RETURN_KW(NAMESPACE)
out                                                 RETURN_KW(OUT)
private                                             RETURN_KW(PRIVATE)
protected                                           RETURN_KW(PROTECTED)
public                                              RETURN_KW(PUBLIC)
return                                              RETURN_KW(RETURN)
struct                                              RETURN_KW(STRUCT)
typedef                                             RETURN_KW(TYPEDEF)
while                                               RETURN_KW(WHILE)

{INTEGER_10_LITERAL}                                RETURN_LITERAL(INTEGER, builder.make_integer(yytext, 10))
{INTEGER_16_LITERAL}                                RETURN_LITERAL(INTEGER, builder.make_integer(&yytext[2], 16))
{FLOATING_POINT_LITERAL}                            RETURN_LITERAL(FLOAT,   builder.make_real(yytext))

{IDENTIFIER}                                        RETURN_TOKEN(IDENTIFIER, text, strdup(yytext))

<<EOF>>                                             return TOK_EOF;
%%



