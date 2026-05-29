use crate::models::errors::AppError;
use crate::models::service::Service;
use crate::parser::param::parse_param;
use crate::parser::service::parse_service;

pub enum Command {
    Generate(Result<Service, AppError>),
    FastGenerate(Result<usize, AppError>),
    List,
    Help,
    Quit,
    Unknown(String),
}

impl From<&str> for Command {
    fn from(command: &str) -> Self {
        let mut split = command.split_whitespace();
        match split.next().unwrap_or("?") {
            "/h" | "/help" => Command::Help,
            "/q" | "/quit" | "/exit" => Command::Quit,
            "/g" | "/gen" | "/generate" => Command::Generate(parse_service::<usize>(&mut split)),
            "/fg" | "/fastgen" => Command::FastGenerate(parse_param::<usize>(&mut split)),
            "/l" | "/list" => Command::List,
            other => Command::Unknown(other.to_string()),
        }
    }
}
