

import cfunc f64 sin(f64);
import cfunc f32 sinf(f32);
import cfunc f64 cos(f64);
import cfunc f32 cosf(f32);

func f32 sin(f32 x) { return sinf(x); }
func f32 cos(f32 x) { return cosf(x); }

func void check_sin() {
    f32 p32 = 3.14159;
    f64 p64 = 3.14159;
    
    sin(p32);
    sin(p64);
}


