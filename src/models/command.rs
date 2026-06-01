use crate::models::errors::AppError;
use crate::models::service::Service;
use crate::parser::param::{next_optional, next_required};
use crate::parser::service::parse_service;

pub enum Command {
    Generate(Result<Service, AppError>),
    Add(Result<Service, AppError>),
    FastGenerate(Result<usize, AppError>),
    Remove(Result<String, AppError>),
    SaveWithRewrite,
    Clear,
    List(SubCommand),
    Help,
    Quit,
    Unknown(String),
}

pub enum SubCommand {
    Default,
    ByService(String),
}

impl From<&str> for Command {
    fn from(command: &str) -> Self {
        let mut split = command.split_whitespace();
        match split.next().unwrap_or("?") {
            "/h" | "/help" => Command::Help,

            "/q" | "/quit" | "/exit" => Command::Quit,

            "/g" | "/gen" | "/generate" => Command::Generate(parse_service::<usize>(&mut split)),

            "/r" | "/rm" | "/remove" => Command::Remove(next_required::<String>(&mut split)),

            "/a" | "/add" => Command::Add(parse_service::<String>(&mut split)),

            "/fg" | "/fastgen" => Command::FastGenerate(next_required::<usize>(&mut split)),

            "/l" | "/list" => Command::List(match next_optional::<String>(&mut split) {
                Ok(Some(service_name)) => SubCommand::ByService(service_name),
                _ => SubCommand::Default,
            }),

            "/c" | "/clear" => Command::Clear,

            "/s" | "/save" => Command::SaveWithRewrite,

            other => Command::Unknown(other.to_string()),
        }
    }
}
