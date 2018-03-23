#!/bin/bash


all_tree_def_files=$top_srcdir/xcc/tree.def
all_lang_files=
all_lang_libs=
all_lang_dirs=

for lang_file in `find . -wholename *.lang | grep -e '\([^/]\+\)/\1.lang'`
do
    lang_dir=$(dirname $lang_file)
    lang_name=$(basename $lang_dir)
    all_lang_files="$all_lang_files $lang_file"

##  Add tree generation info
    if [ -e $lang_dir/tree.def ]; then
        all_tree_def_files="$all_tree_def_files $lang_dir/tree.def"
    fi

##  Add primary makefile
    if [ -e $lang_dir/Makefile.am ]; then
        all_lang_libs="$all_lang_libs $lang_name/lib$lang_name.a"
        all_lang_dirs="$all_lang_dirs $lang_name"
        all_lang_makefiles="$all_lang_makefiles xcc/$lang_name/Makefile"
    fi

## Add language testcases
    if [ -e $lang_dir/check/make_tests.sh ]; then
        lang_tests=`$lang_dir/check/make_tests.sh`
        echo "AC_SUBST([${lang_name}_lang_tests], [\"$lang_tests\"])"
    fi
done


m4_cmd="m4 -P -I$m4lib_dir -I$top_srcdir/m4"
$m4_cmd -Dswitch_frontends_header   $top_srcdir/m4/lang.def.m4 $all_lang_files       > $top_srcdir/xcc/all_frontends.def.hpp
$m4_cmd -Dswitch_tree_header        $top_srcdir/m4/lang.def.m4 $all_lang_files       > $top_srcdir/xcc/all_tree_headers.def.hpp
$m4_cmd -Dswitch_frontends_compiler $top_srcdir/m4/lang.def.m4 $all_lang_files       > $top_srcdir/xcc/all_frontend_compilers.def.hpp
$m4_cmd -Dswitch_frontends_preprc   $top_srcdir/m4/lang.def.m4 $all_lang_files       > $top_srcdir/xcc/all_preprocessors.def.hpp
$m4_cmd -Dswitch_tree_types_header  $top_srcdir/m4/tree.def.m4 $all_tree_def_files   > $top_srcdir/xcc/all_tree_types.def.hpp
$m4_cmd -Dswitch_tree_expand        $top_srcdir/m4/tree.def.m4 $all_tree_def_files   > $top_srcdir/xcc/all_tree_types_expand.def.hpp

echo "AC_SUBST([xcc_lang_dirs],  [\"$all_lang_dirs\"])"
echo "AC_SUBST([xcc_lang_libs],  [\"$all_lang_libs\"])"
echo "AC_SUBST([xcc_tests],      [])"
echo "m4_define([__xcc_lang_makefiles__], [$all_lang_makefiles])"

