m4_divert(`-1')
m4_define(`__m4_list_base',                                     `0')
m4_define(`__m4_strip_back',                                    `m4_patsubst(`$*',`\s*$',`')')
m4_define(`__m4_strip_front',                                   `m4_patsubst(`$*',`^\s*',`')')
m4_define(`__m4_pass',                                          `$*')
m4_define(`__m4_nargs',                                         `m4_ifelse((m4_strip($*)), `()', `0', `$#')')
m4_define(`__m4_argn',                                          `m4_ifelse(m4_eval($1 >= 0), m4_true,
                                                                    `m4_ifelse(`$1', __m4_list_base,
                                                                        `$2',
                                                                        `__m4_argn(m4_eval($1 - 1), m4_shift(m4_shift($*)))')',
                                                                    `<err>')')
m4_define(`__m4_for',                                           `m4_pushdef(`$1', `$2')$4`'m4_ifelse(`$2', `$3',
                                                                    `',
                                                                    `$0(`$1', m4_incr(`$2'), `$3', `$4')')m4_popdef(`$1')')
m4_define(`__m4_foreach',                                       `m4_ifelse(`$2', `()', `',
                                                                    `m4_define(`$1', m4_first$2)$3`'$0(`$1', (m4_rest$2), `$3')')')
m4_define(`__m4_list_map_do_first',                             `m4_with(`$1',`m4_list_first($2)', `$3')')
m4_define(`__m4_list_map_do_rest',                              `__m4_list_map(`$1', m4_list_rest($2), `$3')')
m4_define(`__m4_list_map',                                      `m4_with(`__list_len',`m4_list_len($2)',
                                                                    `m4_ifelse(__list_len, `0', `',
                                                                               __list_len, `1', `__m4_list_map_do_first($@)',
                                                                                                `m4_concat(
                                                                                                    __m4_list_map_do_first($@),
                                                                                                    `,',
                                                                                                    `__m4_list_map_do_rest($@)')')')')
m4_define(`__m4_member_name',                                   ``__m4_storage__$1.$2'')
m4_define(`__m4_array_index_name',                              ``__m4_storage__$1[$2]'')
m4_define(`__m4_map_index_name',                                ``__m4_storage__$1[$2]'')

m4_define(`m4_true',                                            `1')
m4_define(`m4_false',                                           `0')
m4_define(`m4_or',                                              `m4_ifelse($1, m4_true, m4_true, `m4_ifelse($2, m4_true, m4_true, m4_false)')')
m4_define(`m4_and',                                             `m4_ifelse($1, m4_true, `m4_ifelse($2, m4_true, m4_true, m4_false)', m4_false)')
m4_define(`m4_not',                                             `m4_ifelse($1, m4_true, m4_false, m4_true)')

m4_define(`m4_strip',                                           `__m4_strip_back(__m4_strip_front($*))')
m4_define(`m4_concat',                                          `m4_ifelse(`$#', `0', `',
                                                                           `$#', `1', `$1',
                                                                                      `$1'`m4_concat(m4_shift($@))')')
m4_define(`m4_quote',                                           ``$*'')
m4_define(`m4_first',                                           `$1')
m4_define(`m4_rest',                                            m4_defn(`m4_shift'))
m4_define(`m4_argn',                                            m4_defn(`__m4_argn'))
m4_define(`m4_nargs',                                           m4_defn(`__m4_nargs'))
m4_define(`m4_shiftn',                                          `m4_ifelse($1, `0', `m4_shift($@)', `m4_shiftn(m4_eval(`($1) - 1'), m4_shift(m4_shift($@)))')')

m4_define(`m4_list',                                            `($@)')
m4_define(`m4_list_expand',                                     `__m4_pass$1')
m4_define(`m4_list_len',                                        `__m4_nargs(m4_list_expand($1))')
m4_define(`m4_list_item',                                       `__m4_argn($2,m4_list_expand($1))')
m4_define(`m4_list_first',                                      `m4_first(m4_list_expand($1))')
m4_define(`m4_list_rest',                                       `m4_list(m4_rest(m4_list_expand($1)))')
m4_define(`m4_list_empty',                                      `m4_ifelse(m4_list_len($1), `0', m4_true, m4_false)')
m4_define(`m4_list_map',                                        `m4_list(__$0($@))')

m4_define(`m4_with',                                            `m4_pushdef(`$1', `$2')$3`'m4_popdef(`$1')')
m4_define(`m4_for',                                             `m4_ifelse(m4_eval(`($2) < ($3)'), m4_true,
                                                                    `m4_pushdef(`$1')__$0(`$1', m4_eval(`$2'), m4_eval(`($3)-1'), `$4')m4_popdef(`$1')')')
m4_define(`m4_foreach',                                         `m4_pushdef(`$1')__$0($@)m4_popdef(`$1')')

m4_define(`m4_define_ifdef',                                    `m4_ifelse($#,              `3', `m4_ifdef(`$2', `m4_define(`$1', `$3')', `m4_define(`$1', `')')',
                                                                           m4_eval($# > 3), `1', `m4_define_ifdef(`$1', m4_shiftn(3,$@))')')
m4_define(`m4_list_enumerate',                                  `m4_strip(
                                                                    m4_pushdef(`i', 0)
                                                                    m4_list_map(`li', $1, `(i, li)m4_define(`i', m4_incr(i))')
                                                                    m4_popdef(`i'))')


m4_define(`__m4_get_indir',                                     `m4_ifelse(`$#', `1', `m4_indir(`$1')',
                                                                           `$#', `2', `m4_indir(__m4_member_name(`$1',`$2'))')')
m4_define(`__m4_set_indir',                                     `m4_ifelse(`$#', `2', `m4_define(`$1', `$2')',
                                                                           `$#', `3', `m4_define(__m4_member_name(`$1',`$2'), `$3')')')
m4_define(`m4_get',                                             m4_defn(`__m4_get_indir'))
m4_define(`m4_set',                                             m4_defn(`__m4_set_indir'))

m4_divert(`0')m4_dnl
