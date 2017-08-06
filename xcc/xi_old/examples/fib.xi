
func i32 fib_rec(i32 x) {
    if x <  0: return 0;
    if x == 0: return 1;
    if x == 1: return 1;
    return fib_rec(x-1) + fib_rec(x);
}

func i32 fib_iter(i32 x) {
    if x <  0: return 0;
    if x <= 1: return 1;
    local i32 i = 2;
    local i32 prev_m_1 = 1;
    local i32 prev_m_2 = 1;
    while true {
        local i32 next = prev_m_1 + prev_m_2;
        prev_m_1 = prev_m_2;
        prev_m_2 = next;
        if i == x:
            return next;
        i += 1;
    }
    return 0;
}

export cfunc i32 check_fib_rec(i32 x) {
    return fib_rec(x);
}

export cfunc i32 check_fib_iter(i32 x) {
    return fib_iter(x);
}

