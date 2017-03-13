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
#define     RETURN_LOCATED_VALUE(tp,name,valname,val)   { SHIFTLOC yylval->valname = builder.setloc(val, *yyloc); return tp##_##name; }
#define     RETURN_VOID(tp,name)                        { SHIFTLOC return tp##_##name; }

#define     RETURN_LITERAL(tp,val)                      RETURN_VALUE(LITERAL,tp,expr,val)
#define     RETURN_LOCATED_LITERAL(tp,val)              RETURN_LOCATED_VALUE(LITERAL,tp,expr,val)
#define     RETURN_KW(name)                             RETURN_VOID(KW,name)
#define     RETURN_OP(name, val)                        RETURN_VALUE(OP, name, op, xcc::xi_operator::val)
#define     RETURN_TOKEN(n,v,t)                         RETURN_VALUE(TOK,n,v,t)
#define     RETURN_LOCATED_TOKEN(n,v,t)                 RETURN_LOCATED_VALUE(TOK,n,v,t)
#define     RETURN_TYPE(t)                              RETURN_VALUE(TOK,TYPE,type,t)
#define     RETURN_DECL(t)                              RETURN_VALUE(TOK,DECL,decl,t)
#define     RETURN_EXPR(e)                              RETURN_VALUE(TOK,EXPR,expr,e)
#define     RETURN_LOCATED_EXPR(e)                      RETURN_LOCATED_VALUE(TOK,EXPR,expr,e)

#define     RESET_IDENTIFIER_CHECK                  \
                                                    {\
                                                        xi_parse_raw_identifier = false;\
                                                    }
void xi_push_stmt_state();
void xi_pop_stmt_state();

void xi_push_decl_state();
void xi_pop_decl_state();

void xi_push_struct_decl_state();
void xi_pop_struct_decl_state();

void xi_push_unused_id_state();
void xi_pop_unused_id_state();

void xi_push_type_state();
void xi_pop_type_state();

void xi_push_expr_state();
void xi_pop_expr_state();

void xi_ready_namespace_member(xcc::ast_namespace_decl* decl);
void xi_clear_namespace_member();

void xi_ready_static_type_member(xcc::ast_type*);
void xi_clear_static_type_member();

static inline void on_state_push(const char* name, int state);
static inline void on_state_pop(const char*  name, int state);

xcc::ast_namespace_decl*                            xi_scope_namespace = nullptr;
xcc::xi_type_decl*                                  xi_scope_type      = nullptr;

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
%x                                                  XI_STATE_NAMESPACE_MEMBER_ID

%s                                                  XI_STATE_STMT
%s                                                  XI_STATE_DECL
%s                                                  XI_STATE_EXPR
%s                                                  XI_STATE_TYPE
%s                                                  XI_STATE_STRUCT
%%
%{
    BEGIN_LOC
%}
        /* white space */
\n                                                  NEWLINE
[ \t]+                                              IGNORE


        /* comments */
\\"*"                                               { SHIFTLOC; yy_push_state(XI_STATE_BLOCK_COMMENT); }
<XI_STATE_BLOCK_COMMENT>"*/"                        { SHIFTLOC; yy_pop_state(); }
<XI_STATE_BLOCK_COMMENT>\n                          NEWLINE
<XI_STATE_BLOCK_COMMENT>.                           IGNORE
"//"[^\n]*\n                                        NEWLINE


        /* preprocessor lines */
^#[ \t]+{INTEGER_10_LITERAL}[ \t]+{DBL_STRING_LITERAL}.*\n {
                                                        char* cpp_filename = (char*)malloc(1024*sizeof(char));
                                                        int   cpp_line_number;
                                                        sscanf(yytext, "# %d \"%s\"", &cpp_line_number, cpp_filename);
                                                        
                                                        
                                                        xcc::set_source_span_from_cpp(*yyloc, cpp_filename, cpp_line_number);
                                                        free(cpp_filename);
                                                    }


            /* Delimiters */

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



        /* operators */

<XI_STATE_OP>\n                                     NEWLINE
<XI_STATE_OP>[ \t]                                  IGNORE

<XI_STATE_OP,XI_STATE_STMT>\+                       RETURN_OP(ADD,          add)
<XI_STATE_OP,XI_STATE_STMT>\-                       RETURN_OP(SUB,          sub)
<XI_STATE_OP,XI_STATE_STMT>\*                       RETURN_OP(MUL,          mul)
<XI_STATE_OP,XI_STATE_STMT>\/                       RETURN_OP(DIV,          div)
<XI_STATE_OP,XI_STATE_STMT>\%                       RETURN_OP(MOD,          mod)
<XI_STATE_OP,XI_STATE_STMT>\*\>                     RETURN_OP(SHR,          shr)
<XI_STATE_OP,XI_STATE_STMT>\<\*                     RETURN_OP(SHL,          shl)

<XI_STATE_OP,XI_STATE_STMT>\=\=                     RETURN_OP(EQ,           eq)
<XI_STATE_OP,XI_STATE_STMT>\!\=                     RETURN_OP(NE,           ne)
<XI_STATE_OP,XI_STATE_STMT>\<                       RETURN_OP(LT,           lt)
<XI_STATE_OP,XI_STATE_STMT>\>                       RETURN_OP(GT,           gt)
<XI_STATE_OP,XI_STATE_STMT>\<\=                     RETURN_OP(LE,           le)
<XI_STATE_OP,XI_STATE_STMT>\>\=                     RETURN_OP(GE,           ge)

\&                                                  RETURN_OP(BAND,         band)
\|                                                  RETURN_OP(BOR,          bor)
\^                                                  RETURN_OP(BXOR,         bxor)
\~                                                  RETURN_OP(BNOT,         bnot)
\&&                                                 RETURN_OP(LAND,         land)
\|\|                                                RETURN_OP(LOR,          lor)
\!                                                  RETURN_OP(LNOT,         lnot)

<XI_STATE_OP,XI_STATE_STMT>\=                       RETURN_OP(ASSIGN,       assign)
<XI_STATE_OP,XI_STATE_STMT>\+\=                     RETURN_OP(ASSIGN_ADD,   assign_add)
<XI_STATE_OP,XI_STATE_STMT>\-\=                     RETURN_OP(ASSIGN_SUB,   assign_sub)
<XI_STATE_OP,XI_STATE_STMT>\*\=                     RETURN_OP(ASSIGN_MUL,   assign_mul)
<XI_STATE_OP,XI_STATE_STMT>\/\=                     RETURN_OP(ASSIGN_DIV,   assign_div)
<XI_STATE_OP,XI_STATE_STMT>\%\=                     RETURN_OP(ASSIGN_MOD,   assign_mod)
<XI_STATE_OP,XI_STATE_STMT>\-\>\=                   RETURN_OP(ASSIGN_SHR,   assign_shr)
<XI_STATE_OP,XI_STATE_STMT>\<\-\=                   RETURN_OP(ASSIGN_SHL,   assign_shl)
<XI_STATE_OP,XI_STATE_STMT>\&\=                     RETURN_OP(ASSIGN_BAND,  assign_band)
<XI_STATE_OP,XI_STATE_STMT>\|\=                     RETURN_OP(ASSIGN_BOR,   assign_bor)
<XI_STATE_OP,XI_STATE_STMT>\&\&\=                   RETURN_OP(ASSIGN_LAND,  assign_land)
<XI_STATE_OP,XI_STATE_STMT>\|\|\=                   RETURN_OP(ASSIGN_LOR,   assign_lor)

<XI_STATE_OP>\[[ \t\n]*\]                           RETURN_OP(INDEX,        index)
<XI_STATE_OP>\([ \t\n]*\)                           RETURN_OP(INVOKE,       invoke)


        /* builtin types */
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


        /* declaration scope keywords */

const                                               RETURN_KW(CONST)
extern                                              RETURN_KW(EXTERN)
export[ \t]+cfunc                                   RETURN_KW(EXPORT_CFUNC)
func                                                RETURN_KW(FUNC)
import[ \t]+cfunc                                   RETURN_KW(IMPORT_CFUNC)
internal                                            RETURN_KW(INTERNAL)
namespace                                           RETURN_KW(NAMESPACE)
static                                              RETURN_KW(STATIC)
struct                                              RETURN_KW(STRUCT)
typedef                                             RETURN_KW(TYPEDEF)


        /* statement scope keywords*/
        
<XI_STATE_STMT>elif                                 RETURN_KW(ELIF)
<XI_STATE_STMT>else                                 RETURN_KW(ELSE)
<XI_STATE_STMT>for                                  RETURN_KW(FOR)
<XI_STATE_STMT>if                                   RETURN_KW(IF)
<XI_STATE_STMT>in                                   RETURN_KW(IN)
<XI_STATE_STMT>local                                RETURN_KW(LOCAL)
<XI_STATE_STMT>return                               RETURN_KW(RETURN)
<XI_STATE_STMT>while                                RETURN_KW(WHILE)


    /* expression scope keywords */

<XI_STATE_STMT>iter                                 RETURN_KW(ITER)
<XI_STATE_STMT>map                                  RETURN_KW(MAP)
<XI_STATE_STMT>range                                RETURN_KW(RANGE)
<XI_STATE_STMT>sizeof                               RETURN_KW(SIZEOF)
<XI_STATE_STMT>typeof                               RETURN_KW(TYPEOF)
<XI_STATE_STMT>zip                                  RETURN_KW(ZIP)


    /* constant literals */

true                                                RETURN_LITERAL(INTEGER, builder.make_true())
false                                               RETURN_LITERAL(INTEGER, builder.make_false())

{INTEGER_10_LITERAL}                                RETURN_LOCATED_LITERAL(INTEGER, builder.make_integer(yytext, 10))
{INTEGER_16_LITERAL}                                RETURN_LOCATED_LITERAL(INTEGER, builder.make_integer(&yytext[2], 16))
{FLOATING_POINT_LITERAL}                            RETURN_LOCATED_LITERAL(FLOAT,   builder.make_real(yytext))
{DBL_STRING_LITERAL}                                RETURN_LOCATED_LITERAL(STRING,  builder.make_string(yytext, 1, strlen(yytext)-2))


    /* identifiers */

<XI_STATE_UNUSED_ID>\n                              NEWLINE
<XI_STATE_UNUSED_ID>[ \t\n]                         IGNORE
<XI_STATE_UNUSED_ID>{IDENTIFIER}                    {
                                                        RETURN_TOKEN(IDENTIFIER, text, strdup(yytext))
                                                    }

{IDENTIFIER}                                        {
                                                        if(xi_scope_namespace) {
                                                            auto t = builder.find_member(xi_scope_namespace, yytext);
                                                            if(t == nullptr) {
                                                                //TODO: this is an error
                                                                xi_clear_namespace_member();
                                                            }
                                                            else if(t->is<xcc::xi_type_decl>() ||
                                                                    t->is<xcc::ast_typedef_decl>()) {
                                                                RETURN_TYPE(builder.get_declaration_type(t));
                                                            }
                                                            else {
                                                                RETURN_LOCATED_EXPR(builder.make_declref_expr(t));
                                                            }
                                                        }
                                                        else if(xi_scope_type) {
                                                            auto t = builder.find_member(xi_scope_type, yytext);
                                                            if(t == nullptr) {
                                                                //TODO: this is an error
                                                                xi_clear_static_type_member();
                                                            }
                                                            else if(t->is<xcc::xi_type_decl>() ||
                                                                    t->is<xcc::ast_typedef_decl>()) {
                                                                RETURN_TYPE(builder.get_declaration_type(t));
                                                            }
                                                            else {
                                                                //RETURN_LOCATED_EXPR(builder.make_memberref(builder.get_declaration_type(xi_scope_type), t));
                                                                RETURN_LOCATED_EXPR(builder.make_memberref_expr(builder.get_declaration_type(xi_scope_type), yytext))
                                                            }
                                                        }
                                                        else {
                                                            auto t = builder.find_declaration(yytext);
                                                            if(t == nullptr) {
                                                                RETURN_TOKEN(IDENTIFIER, text, strdup(yytext))
                                                            }
                                                            else if(t->is<xcc::xi_type_decl>()  ||
                                                                    t->is<xcc::ast_typedef_decl>()) {
                                                                RETURN_TYPE(builder.get_declaration_type(t));
                                                            }
                                                            else if(t->is<xcc::ast_namespace_decl>()) {
                                                                RETURN_TOKEN(NAMESPACE, namespace_decl, t->as<xcc::ast_namespace_decl>());
                                                            }
                                                            else {
                                                                if(YY_START == XI_STATE_TYPE) {
                                                                    RETURN_TOKEN(IDENTIFIER, text, strdup(yytext));
                                                                }
                                                                RETURN_LOCATED_EXPR(builder.make_declref_expr(t));
                                                            }
                                                        }
                                                    }

<<EOF>>                                             return TOK_EOF;
%%

static inline void on_state_push(const char* name, int state) {
    //printf("[%d] pushing state: %d: %s\n", YY_START, state, name);                 //TODO: for debugging only
}

static inline void on_state_pop(const char* name, int state) {
    //printf("[%d] poping state:  %d: %s\n", YY_START, state, name);                 //TODO: for debugging only
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

void xi_push_struct_decl_state() {
    xi_push_decl_state();
    on_state_push("struct", XI_STATE_STRUCT);
    yy_push_state(XI_STATE_STRUCT);
}

void xi_pop_struct_decl_state() {
    on_state_pop("struct", XI_STATE_STRUCT);
    yy_pop_state();
    xi_pop_decl_state();
}

void xi_push_type_state() {
    on_state_push("type", XI_STATE_TYPE);
    yy_push_state(XI_STATE_TYPE);
}

void xi_pop_type_state() {
    on_state_pop("type", XI_STATE_TYPE);
    yy_pop_state();
}

void xi_push_expr_state() {
    on_state_push("expr", XI_STATE_EXPR);
    yy_push_state(XI_STATE_EXPR);
}

void xi_pop_expr_state() {
    on_state_pop("expr", XI_STATE_EXPR);
    yy_pop_state();
}

void xi_ready_namespace_member(xcc::ast_namespace_decl* decl) {
    xi_scope_namespace = decl;
}

void xi_clear_namespace_member() {
    xi_scope_namespace = nullptr;
}

void xi_ready_static_type_member(xcc::ast_type* tp) {
    xi_scope_type = tp->as<xcc::xi_object_type>()->declaration;
}

void xi_clear_static_type_member() {
    xi_scope_type = nullptr;
}



