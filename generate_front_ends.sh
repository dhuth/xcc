#!/bin/bash


tree_def_files=$top_srcdir/xcc/tree.def
lang_files=
lang_libs=

for lang_file in `find . -wholename *.lang | grep -e '\([^/]\+\)/\1.lang'`
do
    lang_dir=$(dirname $lang_file)
    lang_name=$(basename $lang_dir)
    
    lang_files="$lang_files $lang_file"
    if [ -e $lang_dir/tree.def ]; then
        tree_def_files="$tree_def_files $lang_dir/tree.def"
    fi

    if [ -e $lang_dir/Makefile.am ]; then
        lang_libs="$lang_libs $lang_name/lib$lang_name.a"
        lang_makefiles="$lang_makefiles xcc/$lang_name/Makefile"
    fi
done


m4_cmd="m4 -P -I$top_srcdir/m4"
$m4_cmd -Dswitch_frontends_header   $top_srcdir/m4/lang.def.m4 $lang_files       > $top_srcdir/xcc/all_frontends.def.hpp
$m4_cmd -Dswitch_tree_header        $top_srcdir/m4/lang.def.m4 $lang_files       > $top_srcdir/xcc/all_tree_headers.def.hpp
$m4_cmd -Dswitch_frontends_compiler $top_srcdir/m4/lang.def.m4 $lang_files       > $top_srcdir/xcc/all_frontend_compilers.def.hpp
$m4_cmd -Dswitch_tree_types_header  $top_srcdir/m4/tree.def.m4 $tree_def_files   > $top_srcdir/xcc/all_tree_types.def.hpp
$m4_cmd -Dswitch_tree_expand        $top_srcdir/m4/tree.def.m4 $tree_def_files   > $top_srcdir/xcc/all_tree_types_expand.def.hpp

echo "AC_SUBST([xcc_lang_libs], [\"$lang_libs\"])"
echo "m4_define([__xcc_lang_makefiles__], [$lang_makefiles])"

