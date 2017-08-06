



fn process_value(value: i32) {
    match value {
        _ < 0   :   println!("value is small");
        0       :   println!("value = zero");
        1       :   println!("value = one");
        2       :   println!("value = two");
        3       :   println!("value = three");
        4       :   println!("value = four");
        _ > 4   :   println!("value is big");
    }
}


