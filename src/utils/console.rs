use std::io::Write;

#[must_use]
pub fn read<T>(prompt: &str) -> T
where
    T: std::str::FromStr,
{
    loop {
        let mut input = String::new();
        print!("{}", prompt);
        let _ = std::io::stdout().flush();
        if std::io::stdin().read_line(&mut input).is_err() {
            eprintln!("{}", term_ansi::red!("[ERROR]: reading input"));
            continue;
        }
        if input.trim().is_empty() {
            eprintln!("{}", term_ansi::red!("[ERROR]: input can't be empty"));
            continue;
        }
        match input.trim().parse::<T>() {
            Ok(v) => return v,
            Err(_) => eprintln!("{}", term_ansi::red!("Invalid input, try again")),
        }
    }
}

pub fn console_clear() {
    println!("\x1B[3J\x1B[2J\x1B[H")
}

pub fn wait_enter() {
    println!("Press enter to continue...");
    std::io::stdin()
        .read_line(&mut String::new())
        .expect("[ERROR]: reading input");
}
