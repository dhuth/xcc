#!/bin/bash

xi_check_dir=$(dirname `realpath $0`)
xi_tests=

for test_file in `find $xi_check_dir/test_*.xi`
do
    test_name=`basename ${test_file%.xi}`
    if [ -e $xi_check_dir/run-stdout/$test_name.stdout ]; then
        test_exec_name=$test_name.dorun.sh
        test_exec_path=$xi_check_dir/$test_exec_name
        
        echo "#!/bin/sh"                            >  $test_exec_path
        echo "$xi_check_dir/dorun.sh $test_file"    >> $test_exec_path
        echo "exit \$?"                             >> $test_exec_path
        
        xi_tests="$xi_tests check/$test_exec_name"
    fi
done

echo "$xi_tests"

