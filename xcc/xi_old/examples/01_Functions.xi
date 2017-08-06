
// ------------------------------ //
// Main Function (Entry Function) //
// ------------------------------ //

fn main() {
    println!("Hello world");
}

// ------------------------ //
// Funciton with parameters //
// ------------------------ //

fn print_sum(x: i32, y: i32) {
    println!("{0} + {1} = {2}", x, y, x + y);
}

fn main() {
    print_sum(2,2);
    print_sum(3,3);
}

// -------------------------- //
// Function with return value //
// -------------------------- //

fn pow(x: f32, e: u32) -> f32 {
    if e == 0 { ret 1; }
    else      { ret x * pow(x, e - 1); }
}

fn print_pow(x: f32, e: u32) {
    println!("{0} ^ {1} = {2}", x, e, pow(x,e));
}

fn main() {
    print_pow(4,0);
    print_pow(4,1);
    print_pow(4,2);
    print_pow(4,3);
    print_pow(4,4);
}


