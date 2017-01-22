#!/bin/bash

tree_def_files=$top_srcdir/xcc/tree.def

for lang_file in `find . -wholename *.lang | grep -e '\([^/]\+\)/\1.lang'`
do
    lang_dir=$(dirname $lang_file)
    lang_name=$(basename $lang_dir)
    
    if [ -e $lang_dir/tree.def ]; then
        tree_def_files="$tree_def_files $lang_dir/tree.def"
    fi
done


m4_cmd="m4 -P -I$top_srcdir/m4"
$m4_cmd -Dswitch_tree_types_header $top_srcdir/m4/tree.def.m4 $tree_def_files > $top_srcdir/xcc/all_tree_types.def.hpp

