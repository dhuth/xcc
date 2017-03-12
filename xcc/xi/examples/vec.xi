
namespace xi {

struct fvec2 { f32[2]       _el; }
struct fvec3 { f32[3]       _el; }
struct fvec4 { f32[4]       _el; }

struct fmat2 { f32[2,2]     _el; }
struct fmat3 { f32[3,3]     _el; }
struct fmat4 { f32[4,4]     _el; }

func f32 dot(fvec3 a, fvec3 b) {
    f32 sum = 0;
    u32 i = 0;
    while i < 3 {
        sum += a[i] * b[i];
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
