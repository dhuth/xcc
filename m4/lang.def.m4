m4_include(`util.m4')m4_divert(`-1')
m4_define_ifdef(`header',       `switch_frontends_header',      `#include "$1"')
m4_define_ifdef(`compiler',     `switch_frontends_compiler',    `{"$1", "$2", {m4_foreach(i, $3, `"i", ')}, &$4},')
m4_divert(`0')
