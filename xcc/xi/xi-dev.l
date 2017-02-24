%option noyywrap bison-bridge
%option prefix="xi"
%option header-file="xi-lex.hpp"

%option stack

%{
#include "xi-parser.hpp"
#include "source.hpp"

#define     SHIFTLOC                                xcc::advance_column(yyloc->last, strlen(yytext));
#define     IGNORE                                  SHIFTLOC
#define     NEWLINE                                 xcc::advance_line(yyloc->last);
#define     BEGIN_LOC                               xcc::init_source_span(*yyloc, yyloc->last);
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
void xi_push_expr_state();
void xi_pop_expr_state();

%}

IDENTIFIER                                          [a-zA-Z_][a-zA-Z0-9_]*
DIGIT                                               [0-9]
DIGIT_HEX_LOWER                                     [0-9a-f]
DIGIT_HEX_UPPER                                     [0-9A-F]
HEX                                                 ({DIGIT_HEX_UPPER}+|{DIGIT_HEX_LOWER}+)

FP_FRONT                                            {DIGIT}+\.{DIGIT}*
FP_BACK                                             [Ee][+-]?{DIGIT}+

INTEGER_10_LITERAL                                  {DIGIT}+
INTEGER_16_LITERAL                                  0x{HEX}
FLOATING_POINT_LITERAL                              ({FP_FRONT}({FP_BACK})?)|({DIGIT}+{FP_BACK})



%x                                                  XI_STATE_BLOCK_COMMENT
%s                                                  XI_STATE_EXPR
%s                                                  XI_STATE_STMT
%%
%{
    BEGIN_LOC
%}
\n                                                  NEWLINE
[ \t]+                                              IGNORE

"\*"                                                { SHIFTLOC; yy_push_state(XI_STATE_BLOCK_COMMENT); }
<XI_STATE_BLOCK_COMMENT>"*/"                        { SHIFTLOC; yy_pop_state(); }
<XI_STATE_BLOCK_COMMENT>\n                          NEWLINE
<XI_STATE_BLOCK_COMMENT>.                           IGNORE

"//"[^\n]*\n                                        NEWLINE

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
\>\>\=                                              RETURN_OP(ASSIGN_SHR,   assign_shr)
\<\<\=                                              RETURN_OP(ASSIGN_SHL,   assign_shl)
\&\=                                                RETURN_OP(ASSIGN_BAND,  assign_band)
\|\=                                                RETURN_OP(ASSIGN_BOR,   assign_bor)
\&\&\=                                              RETURN_OP(ASSIGN_LAND,  assign_land)
\|\|\=                                              RETURN_OP(ASSIGN_LOR,   assign_lor)

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

abstract                                            RETURN_KW(ABSTRACT)
auto                                                RETURN_KW(AUTO)
byref                                               RETURN_KW(BYREF)
byval                                               RETURN_KW(BYVAL)
class                                               RETURN_KW(CLASS)
const                                               RETURN_KW(CONST)
extern                                              RETURN_KW(EXTERN)
func                                                RETURN_KW(FUNC)
generic                                             RETURN_KW(GENERIC)
inline                                              RETURN_KW(INLINE)
interface                                           RETURN_KW(INTERFACE)
namespace                                           RETURN_KW(NAMESPACE)
out                                                 RETURN_KW(OUT)
private                                             RETURN_KW(PRIVATE)
protected                                           RETURN_KW(PROTECTED)
public                                              RETURN_KW(PUBLIC)
struct                                              RETURN_KW(STRUCT)
typedef                                             RETURN_KW(TYPEDEF)
virtual                                             RETURN_KW(VIRTUAL)

elif                                                RETURN_KW(ELIF)
else                                                RETURN_KW(ELSE)
for                                                 RETURN_KW(FOR)
if                                                  RETURN_KW(IF)
in                                                  RETURN_KW(IN)
return                                              RETURN_KW(RETURN)
while                                               RETURN_KW(WHILE)
yield                                               RETURN_KW(YIELD)

<XI_STATE_EXPR>iter                                 RETURN_KW(ITER)
<XI_STATE_EXPR>map                                  RETURN_KW(MAP)
<XI_STATE_EXPR>range                                RETURN_KW(RANGE)
<XI_STATE_EXPR>sizeof                               RETURN_KW(SIZEOF)
<XI_STATE_EXPR>typeof                               RETURN_KW(TYPEOF)
<XI_STATE_EXPR>zip                                  RETURN_KW(ZIP)

<XI_STATE_EXPR>true                                 RETURN_LITERAL(INTEGER, builder.make_true())
<XI_STATE_EXPR>false                                RETURN_LITERAL(INTEGER, builder.make_false())

<XI_STATE_EXPR>{INTEGER_10_LITERAL}                 RETURN_LITERAL(INTEGER, builder.make_integer(yytext, 10))
<XI_STATE_EXPR>{INTEGER_16_LITERAL}                 RETURN_LITERAL(INTEGER, builder.make_integer(&yytext[2], 16))
<XI_STATE_EXPR>{FLOATING_POINT_LITERAL}             RETURN_LITERAL(FLOAT,   builder.make_real(yytext))

{IDENTIFIER}                                        RETURN_TOKEN(IDENTIFIER, text, strdup(yytext))

<<EOF>>                                             return TOK_EOF;
%%

void xi_push_expr_state() {
    yy_push_state(XI_STATE_EXPR);
}

void xi_pop_expr_state() {
    yy_pop_state();
}





