m4_include(`util.m4')m4_divert(`-1')
m4_define_ifdef(`tree_type', `switch_tree_types_header', `TREE_TYPE($1, $2)')
m4_divert(`0')
