m4_include(`util.m4')m4_divert(`-1')
m4_define_ifdef(`token',   `switch_token_def',      `TOKEN($1, $2)')
m4_define_ifdef(`context', `switch_context_def',    `CONTEXT(CONTEXT_NAME($1), CB(m4_foreach(i, $2, `TOKEN_NAME(i), ')))')
m4_divert(`0')

