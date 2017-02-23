func f64 pow(f64 f, u32 i) {
    f64 res = 1;
    while(i > 0) {
        res *= f;
        i   -= 1;
    }
    return res;
}

