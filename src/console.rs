pub fn read_i32(prompt : &str) -> i32 {
    loop {
        let mut input = String::new();
        println!("{}", prompt);
        if std::io::stdin().read_line(&mut input).is_err() {
            println!("Error reading input");
            continue;
        }
        match input.trim().parse::<i32>() {
            Ok(num) => return num,
            Err(_) => eprintln!("Error reading input"),
        }
    }
}
