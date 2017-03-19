
namespace xi {

struct vec<type T, u32 N>           { T[N]      _el; }
struct mat<type T, u32 N, u32 M>    { T[N,M]    _el; }

typedef fvec<u32 N> = vec<f32, N>;

#for N in 1 .. 9 // 1 to 9 (inclusive)

typedef uvec##N         = vec<u64, N>;
typedef ivec##N         = vec<i64, N>;
typedef fvec##N         = vec<f64, N>;

typedef umat##N         = mat<u64, N, N>;
typedef imat##N         = mat<i64, N, N>;
typedef fmat##N         = mat<f64, N, N>;

#for M in 1 .. 9
#if N != M

typedef umat##N##x##M   = mat<u64, N, M>;
typedef imat##N##x##M   = mat<i64, N, M>;
typedef fmat##N##x##M   = mat<f64, N, M>;

#endif
#enfor
#endfor

func void operator= <type T, u32 N> (T[N]& a, iterable<T>& i) inline {
    for idx, v in zip(range(N), i): 
        a[idx] = v;
}

func void operator= <type T, u32 N, u32 M>(T[N,M]& a, iterable<iterable<T>&>& i) inline {
    for idx_i, idx_j, v in zip(xrange(N, M), i):
        a[idx_i, idx_j] = v;
}

func void __init__<type T, u32 N>(vec<T,N>& self, iterable<T>& el) {
    self._el = el;
}

func void __init__<type T, u32 M, u32 N>(mat<T,N,M>& self, iterable<iterable<T>&>& el) {
    self._el = el;
}

}

//ctor vec2(.x, .y)      { }
//ctor vec3(.x, .y, .z)  { }
