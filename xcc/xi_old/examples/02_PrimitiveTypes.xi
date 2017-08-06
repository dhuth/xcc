/*
    i8 to i64                           // signed integers
    u8 to u64                           // unsigned integers
    isize                               // signed size type
    usize                               // unsigned size type
    bool                                // boolean type
    f32, f64                            // floating point types
    char                                // character type (same as c char)
    str                                 // string
    
    *const T                            // pointer to const type T
    *T                                  // pointer to mutable type T
    
    &const T                            // reference to const type T
    &T                                  // reference to mutable type T
    
    T[N]                                // array of T, size N
    
    local a: [0, 1, 2, 3, 4];           // a: [i32; 5]      -- an array
    local b: a[1..3];                   // b: &[i32; 3]     -- a slice
    
    local x = (2, 3.0);                 // tuple x: i32 = 2; y: f32 = 3.0
    local x: (i32, &str) = (2, "str");  //
    local x: (i32,) = (2,)              // single value tuple
    
    fn foo(x: i32)-> i32 { ret x; }
    local f = foo;                      // f: fn(i32)->i32
    
*/
