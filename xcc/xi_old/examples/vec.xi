
namespace xi {

struct fvec2 { f32[2]       _el; }
struct fvec3 { f32[3]       _el; }
struct fvec4 { f32[4]       _el; }

struct fmat2 { f32[2,2]     _el; }
struct fmat3 { f32[3,3]     _el; }
struct fmat4 { f32[4,4]     _el; }

func void __init__(fvec2& v, f32 x, f32 y) {
    v._el[0] = x;
    v._el[1] = y;
}

func void __init__(fvec3& v, f32 x, f32 y, f32 z) {
    v._el[0] = x;
    v._el[1] = y;
    v._el[2] = z;
}

func void __init__(fvec4& v, f32 x, f32 y, f32 z, f32 w) {
    v._el[0] = x;
    v._el[1] = y;
    v._el[2] = z;
    v._el[3] = w;
}

func f32 dot(const fvec3&, const fvec3&);

export cfunc f32 check_dot() {
    fvec3 a = fvec3(1,2,3);
    fvec3 b = fvec3(1,2,3);
    
    fvec2 c = fvec2(1,2);
    
    return dot(a,b);
}

func f32 dot(const fvec3& a, const fvec3& b) {
    f32 sum = 0;
    u32 i = 0;
    while i < 3 {
        sum += a._el[i] * b._el[i];
        i += 1;
    }
    return sum;
}

}

//ctor vec2(.x, .y)      { }
//ctor vec3(.x, .y, .z)  { }
