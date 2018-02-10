
extern func cos(f64)->f64;
extern func sin(f64)->f64;

extern func cos(f32)->f32;
extern func sin(f32)->f32;

func main(x: f32, y: f32)->f32 {
    return sin(x) + cos(2*y);
}

