


fn print_ilist(arr: i32[], len: usize) {
    local i: usize = 0;
    
    while i < usize {
        println!("arr[{0}] = {1}", i, arr[i]);
        i += 1;
    }
}

fn print_ilist(arr: i32[], len: usize) {
    for(i: usize = 0; i < usize; i += 1) {
        println!("arr[{0}] = {1}", i, arr[i]);
    }
}

fn print_ilist(arr: i32[], len: usize) {
    for i in 0 .. usize {
        println!("arr[{0}] = {1}", i, arr[i]);
    }
}

fn print_ilist(arr: i32[10]) {
    for i, v in enumerate(arr) {
        println!("{0}: {1}", i, v);
    }
}

