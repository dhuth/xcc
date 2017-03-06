define inline f32 dot(byref notnull f32[64] a, byref notnull f32[64] b) {
    return sum(lambda f32(f32 x, f32 y): x*y, zip(iter(a),iter(b)))
}

define void mm(
            byref     notnull f32[64, 64] a,
            byref     notnull f32[64, 64] b,
            byref out         f32[64, 64] c) {
    for(auto i in range(64)) {
        for(auto j in range(64)) {
            c[i,j] = dot(a[i,:], b[:,j]);
        }
    }
}

