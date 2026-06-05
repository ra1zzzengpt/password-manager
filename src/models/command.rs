use crate::models::errors::AppError;
use crate::models::service::Service;
use crate::models::target::Target;
use crate::parser::param::{next_optional, next_required};
use crate::parser::service::parse_service;

pub enum Command {
    Generate(Result<Service, AppError>),
    Add(Result<Service, AppError>),
    FastGenerate(Result<usize, AppError>),
    Remove(Result<Target, AppError>),
    SaveWithRewrite,
    Clear,
    List(Result<Target, AppError>),
    Help,
    Quit,
    Unknown(String),
}

impl From<&str> for Command {
    fn from(command: &str) -> Self {
        let mut split = command.split_whitespace();
        // todo: in Vec + parse flags
        match split.next().unwrap_or("?") {
            "/h" | "/help" => Command::Help,

            "/q" | "/quit" | "/exit" => Command::Quit,

            "/g" | "/gen" | "/generate" => Command::Generate(parse_service::<usize>(&mut split)),

            "/r" | "/rm" | "/remove" => Command::Remove({
                match (
                    next_required::<String>(&mut split),
                    next_optional::<String>(&mut split),
                ) {
                    (Ok(service_name), Ok(service_login)) => {
                        Ok(Target::new(Option::from(service_name), service_login))
                    }
                    (Err(error), _) | (_, Err(error)) => Err(error),
                }
            }),

            "/a" | "/add" => Command::Add(parse_service::<String>(&mut split)),

            "/fg" | "/fastgen" => Command::FastGenerate(next_required::<usize>(&mut split)),

            "/l" | "/list" => Command::List({
                match (
                    next_optional::<String>(&mut split),
                    next_optional::<String>(&mut split),
                ) {
                    (Ok(service_opt), Ok(service_login_opt)) => {
                        Ok(Target::new(service_opt, service_login_opt))
                    }
                    (Err(_), _) | (_, Err(_)) => Ok(Target::new(None, None)),
                }
            }),

            "/c" | "/clear" => Command::Clear,

            "/s" | "/save" => Command::SaveWithRewrite,

            other => Command::Unknown(other.to_string()),
        }
    }
}
