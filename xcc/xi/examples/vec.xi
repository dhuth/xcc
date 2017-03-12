
namespace xi {

struct fvec2 { f32[2]       _el; }
struct fvec3 { f32[3]       _el; }
struct fvec4 { f32[4]       _el; }

struct fmat2 { f32[2,2]     _el; }
struct fmat3 { f32[3,3]     _el; }
struct fmat4 { f32[4,4]     _el; }

func void __init__(fvec3& v, f32 x, f32 y, f32 z) {
    v._el[0] = x;
    v._el[1] = y;
    v._el[2] = z;
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

export cfunc void main() {
    fvec4 v;
    fmat3 m;
}

}

//ctor vec2(.x, .y)      { }
//ctor vec3(.x, .y, .z)  { }
