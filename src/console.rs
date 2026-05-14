use std::io::Write;
pub fn read<T: std::str::FromStr>(prompt: &str) -> T {
    loop {
        let mut input = String::new();
        print!("{}", prompt);
        let _ = std::io::stdout().flush();
        if std::io::stdin().read_line(&mut input).is_err() {
            eprintln!("Read error");
            continue;
        }
        match input.trim().parse::<T>() {
            Ok(v) => return v,
            Err(_) => eprintln!("Invalid input, try again"),
        }
    }
}