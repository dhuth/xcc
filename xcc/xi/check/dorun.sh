#!/bin/bash

rmext() {
    echo ${1%.*}
}


exit_with() {
    rm -f $temp_files
    exit $1
}

maybe_exit_with_error() {
    if [ $1 != 0 ]; then
        rm -f $temp_files
        exit 99
    fi
}

check_diff() {
    local lhs=$1
    local rhs=$2

}

## Inputs:
#       $1 - xi source

check_dir=$(dirname `realpath $0`)
xcc="$check_dir/../../xcc"

xi_base=$(rmext `basename $1`)
xi_source="$check_dir/$xi_base.xi"
xi_object="$check_dir/$xi_base.ll"
xi_result="$check_dir/$xi_base.stdout"

temp_files="$xi_object $xi_result"

$xcc -xxi $xi_source        || maybe_exit_error $?
lli $xi_object > $xi_result || maybe_exit_error $?

diffout=`diff -q $xi_result $check_dir/run-stdout/$xi_base.stdout`

if [ -n "$diffout" ]; then
    exit_with 1
else
    exit_with 0
fi


