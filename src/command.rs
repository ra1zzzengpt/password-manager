pub enum Command<'a> {
    Generate(std::str::SplitWhitespace<'a>),
    Help,
    Quit,
    Unknown(String),
}

impl<'a> From<&'a str> for Command<'a> {
    fn from(command: &'a str) -> Self {
        let mut split = command.split_whitespace();
        match split.next().unwrap() {
            "/h" | "/help" => Command::Help,
            "/q" | "/quit" | "/exit" => Command::Quit,
            "/g" | "/gen" | "/generate" => Command::Generate(split),
            other => Command::Unknown(other.to_string()),
        }
    }
}
