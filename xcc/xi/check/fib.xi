
define i32 fib(i32 x) {
    if(x < 0) {
        return 0;
    }
    elif(x == 0) {
        return 1;
    }
    elif(x == 1) return 1;
    else {
        return fib(x-2) + fib(x-1);
    }
}

