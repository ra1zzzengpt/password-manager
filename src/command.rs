use crate::errors::ErrorType;

pub enum Command {
    Generate,
    Help,
    Quit,
    Unknown(String),
}

impl From<&str> for Command {
    fn from(command: &str) -> Self {
        match command {
            "/h" => Command::Help,
            "/q" => Command::Quit,
            "/g" => Command::Generate,
            other => Command::Unknown(other.to_string()),
        }
    }
}

// fn parse_generate(base_string : String) -> Result<Command, ErrorType> {
//
// }