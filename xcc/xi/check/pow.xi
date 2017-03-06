
define f64 xipow(f64 f, u32 i) {
    local f64 res = 1;
    while(i > 0) {
        res *= f;
        i   -= 1;
    }
    return res;
}

