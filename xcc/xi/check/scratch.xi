
extern cfunc sinf(x: f32) -> f32;
extern cfunc cosf(x: f32) -> f32;

func main(x: f32, y: f32)->f32 {
    return sinf(x) + cosf(2*y);
}

