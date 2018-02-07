
namespace c {

    extern cfunc abs(i32)                                       -> i32;
    extern cfunc labs(i64)                                      -> i64;
    extern cfunc div(i32, i32)                                  -> div_t;
    extern cfunc ldiv(i64, i64)                                 -> ldiv_t;
    
    extern cfunc fabsf(f32)                                     -> f32;
    extern cfunc fabs(f64)                                      -> f64;
    
    extern cfunc fmodf(f32)                                     -> f32;
    extern cfunc fmod(f64)                                      -> f64;
    
    //...

}
