
struct fvec2 {
    x:  f32;
    y:  f32;
}

// ----------- //
// Inheritance //
// ----------- //
struct fvec3: fvec2 {
    z:  f32;
}


fn main() {
    // default constructor
    local i2: fvec2 = fvec2 {1, 0};
    local j2: fvec2 = fvec2 {0, 1};
    
    local i3: fvec3 = fvec3 {1, 0, 0};
    local j3: fvec3 = fvec3 {0, 1, 0};
    local k3: fvec3 = fvec3 {0, 0, 1};
}

// -------------------- //
// constructor function //
// -------------------- //
fn operator __ctor__(self: &fvec3, x: f32, y: f32, z: f32) {
    self.x = x;
    self.y = y;
    self.z = z;
}

// ------------------- //
// Destructor function //
// ------------------- //
fn operator __dtor__(self: &fvec3) {
    /* do nothing */
}


