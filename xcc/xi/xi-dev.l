%option noyywrap bison-bridge
%option prefix="xi"
%option header-file="xi-lex.hpp"

%option stack

%{
#include "xi-parser.hpp"
#include "source.hpp"

#define     SHIFTLOC                                xcc::advance_column(yyloc->last, strlen(yytext));
#define     NEWLINE                                 xcc::advance_line(yyloc->last);
                                                        
#define     IGNORE                                  SHIFTLOC

#define     BEGIN_LOC                               xcc::init_source_span(*yyloc, yyloc->last);
#define     RETURN_VALUE(tp,name,valname,val)       { SHIFTLOC yylval->valname = val; return tp##_##name; }
#define     RETURN_VOID(tp,name)                    { SHIFTLOC return tp##_##name; }

#define     RETURN_LITERAL(tp,val)                  RETURN_VALUE(LITERAL,tp,expr,val)
#define     RETURN_KW(name)                         RETURN_VOID(KW,name)
#define     RETURN_OP(name, val)                    RETURN_VALUE(OP, name, op, xcc::xi_operator::val)
#define     RETURN_TOKEN(n,v,t)                     RETURN_VALUE(TOK,n,v,t)
#define     RETURN_TYPE(t)                          RETURN_VALUE(TOK,TYPE,type,t)
#define     RETURN_DECL(t)                          RETURN_VALUE(TOK,DECL,decl,t)
#define     RETURN_EXPR(e)                          RETURN_VALUE(TOK,EXPR,expr,e)

#define     RESET_IDENTIFIER_CHECK                  \
                                                    {\
                                                        xi_parse_raw_identifier = false;\
                                                    }
void xi_push_stmt_state();
void xi_pop_stmt_state();

void xi_push_decl_state();
void xi_pop_decl_state();

void xi_push_unused_id_state();
void xi_pop_unused_id_state();

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

DBL_STRING_CHAR                                     [^\\"]
DBL_STRING_ESCAPE                                   \\.
DBL_STRING_LITERAL                                  ["]({DBL_STRING_CHAR}|{DBL_STRING_ESCAPE})*["]


%x                                                  XI_STATE_BLOCK_COMMENT
%x                                                  XI_STATE_LINE_COMMENT
%s                                                  XI_STATE_BEGIN_LINE

%x                                                  XI_STATE_OP
%x                                                  XI_STATE_UNUSED_ID

%s                                                  XI_STATE_STMT
%s                                                  XI_STATE_DECL
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

^#[ \t]+{INTEGER_10_LITERAL}[ \t]+{DBL_STRING_LITERAL}.*\n {
                                                        char* cpp_filename = (char*)malloc(1024*sizeof(char));
                                                        int   cpp_line_number;
                                                        sscanf(yytext, "# %d \"%s\"", &cpp_line_number, cpp_filename);
                                                        
                                                        
                                                        xcc::set_source_span_from_cpp(*yyloc, cpp_filename, cpp_line_number);
                                                        free(cpp_filename);
                                                    }

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

<XI_STATE_OP>\n                                     NEWLINE
<XI_STATE_OP>[ \t]                                  IGNORE

<XI_STATE_OP,XI_STATE_STMT>\+                       RETURN_OP(ADD,          add)
<XI_STATE_OP,XI_STATE_STMT>\-                       RETURN_OP(SUB,          sub)
<XI_STATE_OP,XI_STATE_STMT>\*                       RETURN_OP(MUL,          mul)
<XI_STATE_OP,XI_STATE_STMT>\/                       RETURN_OP(DIV,          div)
<XI_STATE_OP,XI_STATE_STMT>\%                       RETURN_OP(MOD,          mod)
<XI_STATE_OP,XI_STATE_STMT>\>\>                     RETURN_OP(SHR,          shr)
<XI_STATE_OP,XI_STATE_STMT>\<\<                     RETURN_OP(SHL,          shl)

<XI_STATE_OP,XI_STATE_STMT>\=\=                     RETURN_OP(EQ,           eq)
<XI_STATE_OP,XI_STATE_STMT>\!\=                     RETURN_OP(NE,           ne)
<XI_STATE_OP,XI_STATE_STMT>\<                       RETURN_OP(LT,           lt)
<XI_STATE_OP,XI_STATE_STMT>\>                       RETURN_OP(GT,           gt)
<XI_STATE_OP,XI_STATE_STMT>\<\=                     RETURN_OP(LE,           le)
<XI_STATE_OP,XI_STATE_STMT>\>\=                     RETURN_OP(GE,           ge)

<XI_STATE_OP,XI_STATE_STMT>\=                       RETURN_OP(ASSIGN,       assign)
<XI_STATE_OP,XI_STATE_STMT>\+\=                     RETURN_OP(ASSIGN_ADD,   assign_add)
<XI_STATE_OP,XI_STATE_STMT>\-\=                     RETURN_OP(ASSIGN_SUB,   assign_sub)
<XI_STATE_OP,XI_STATE_STMT>\*\=                     RETURN_OP(ASSIGN_MUL,   assign_mul)
<XI_STATE_OP,XI_STATE_STMT>\/\=                     RETURN_OP(ASSIGN_DIV,   assign_div)
<XI_STATE_OP,XI_STATE_STMT>\%\=                     RETURN_OP(ASSIGN_MOD,   assign_mod)
<XI_STATE_OP,XI_STATE_STMT>\>\>\=                   RETURN_OP(ASSIGN_SHR,   assign_shr)
<XI_STATE_OP,XI_STATE_STMT>\<\<\=                   RETURN_OP(ASSIGN_SHL,   assign_shl)
<XI_STATE_OP,XI_STATE_STMT>\&\=                     RETURN_OP(ASSIGN_BAND,  assign_band)
<XI_STATE_OP,XI_STATE_STMT>\|\=                     RETURN_OP(ASSIGN_BOR,   assign_bor)
<XI_STATE_OP,XI_STATE_STMT>\&\&\=                   RETURN_OP(ASSIGN_LAND,  assign_land)
<XI_STATE_OP,XI_STATE_STMT>\|\|\=                   RETURN_OP(ASSIGN_LOR,   assign_lor)

<XI_STATE_OP>\[[ \t\n]*\]                           RETURN_OP(INDEX,        index)
<XI_STATE_OP>\([ \t\n]*\)                           RETURN_OP(INVOKE,       invoke)

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
auto                                                RETURN_TYPE(builder.get_infered_type())

abstract                                            RETURN_KW(ABSTRACT)
byref                                               RETURN_KW(BYREF)
byval                                               RETURN_KW(BYVAL)
class                                               RETURN_KW(CLASS)
const                                               RETURN_KW(CONST)
extern                                              RETURN_KW(EXTERN)
export[ \t]+cfunc                                   RETURN_KW(EXPORT_CFUNC)
func                                                RETURN_KW(FUNC)
import[ \t]+cfunc                                   RETURN_KW(IMPORT_CFUNC)
inline                                              RETURN_KW(INLINE)
interface                                           RETURN_KW(INTERFACE)
internal                                            RETURN_KW(INTERNAL)
namespace                                           RETURN_KW(NAMESPACE)
out                                                 RETURN_KW(OUT)
private                                             RETURN_KW(PRIVATE)
protected                                           RETURN_KW(PROTECTED)
public                                              RETURN_KW(PUBLIC)
struct                                              RETURN_KW(STRUCT)
typedef                                             RETURN_KW(TYPEDEF)
virtual                                             RETURN_KW(VIRTUAL)

<XI_STATE_STMT>elif                                 RETURN_KW(ELIF)
<XI_STATE_STMT>else                                 RETURN_KW(ELSE)
<XI_STATE_STMT>for                                  RETURN_KW(FOR)
<XI_STATE_STMT>if                                   RETURN_KW(IF)
<XI_STATE_STMT>in                                   RETURN_KW(IN)
<XI_STATE_STMT>local                                RETURN_KW(LOCAL)
<XI_STATE_STMT>return                               RETURN_KW(RETURN)
<XI_STATE_STMT>while                                RETURN_KW(WHILE)
<XI_STATE_STMT>yield                                RETURN_KW(YIELD)

<XI_STATE_STMT>iter                                 RETURN_KW(ITER)
<XI_STATE_STMT>map                                  RETURN_KW(MAP)
<XI_STATE_STMT>range                                RETURN_KW(RANGE)
<XI_STATE_STMT>sizeof                               RETURN_KW(SIZEOF)
<XI_STATE_STMT>typeof                               RETURN_KW(TYPEOF)
<XI_STATE_STMT>zip                                  RETURN_KW(ZIP)

<XI_STATE_STMT>true                                 RETURN_LITERAL(INTEGER, builder.make_true())
<XI_STATE_STMT>false                                RETURN_LITERAL(INTEGER, builder.make_false())

{INTEGER_10_LITERAL}                                RETURN_LITERAL(INTEGER, builder.make_integer(yytext, 10))
{INTEGER_16_LITERAL}                                RETURN_LITERAL(INTEGER, builder.make_integer(&yytext[2], 16))
{FLOATING_POINT_LITERAL}                            RETURN_LITERAL(FLOAT,   builder.make_real(yytext))

{IDENTIFIER}                                        {
                                                        auto t = builder.find_declaration(yytext);
                                                        if(t == nullptr) {
                                                            // TODO: is this an error?
                                                            //RETURN_EXPR(builder.make_name_expr(yytext));
                                                            RETURN_TOKEN(IDENTIFIER, text, strdup(yytext))
                                                        }
                                                        else if(t->is<xcc::xi_type_decl>()    ||
                                                                t->is<xcc::ast_typedef_decl>()) {
                                                            RETURN_TYPE(builder.get_declaration_type(t));
                                                        }
                                                        else {
                                                            RETURN_EXPR(builder.make_declref_expr(t->as<xcc::ast_decl>()));
                                                        }
                                                    }

<XI_STATE_UNUSED_ID>\n                              NEWLINE
<XI_STATE_UNUSED_ID>[ \t\n]                         IGNORE
<XI_STATE_UNUSED_ID>{IDENTIFIER}                    RETURN_TOKEN(IDENTIFIER, text, strdup(yytext))

<<EOF>>                                             return TOK_EOF;
%%

static inline void on_state_push(const char* name, int state) {
    //printf("pushing state: %d: %s\n", state, name);                 //TODO: for debugging only
}

static inline void on_state_pop(const char* name, int state) {
    //printf("poping state:  %d: %s\n", state, name);                 //TODO: for debugging only
    assert(state == YY_START);
}

void xi_push_stmt_state() {
    on_state_push("stmt", XI_STATE_STMT);
    yy_push_state(XI_STATE_STMT);
}

void xi_pop_stmt_state() {
    on_state_pop("stmt", XI_STATE_STMT);
    yy_pop_state();
}


void xi_push_unused_id_state() {
    on_state_push("unused id", XI_STATE_UNUSED_ID);
    yy_push_state(XI_STATE_UNUSED_ID);
}

void xi_pop_unused_id_state() {
    on_state_pop("unused id", XI_STATE_UNUSED_ID);
    yy_pop_state();
}


void xi_push_decl_state() {
    on_state_push("decl", XI_STATE_DECL);
    yy_push_state(XI_STATE_DECL);
}

void xi_pop_decl_state() {
    on_state_pop("decl", XI_STATE_DECL);
    yy_pop_state();
}




