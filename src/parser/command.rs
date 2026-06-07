use crate::models::command::Command;
use crate::models::errors::AppError;
use crate::models::target::Target;
use crate::parser::flag::parse_flags;
use crate::parser::param::{next_optional, next_required};
use crate::parser::service::parse_service;

pub fn parse_command(command: &str) -> Result<Command, AppError> {
    let (mut args, raw_flags): (Vec<&str>, Vec<&str>) = command
        .split_whitespace()
        .partition(|substr| !substr.starts_with("--"));
    if args.is_empty() {
        return Ok(Command::Unknown(format!("{command}")));
    }
    let flags = parse_flags(&raw_flags)?;
    let command = args.remove(0);
    match command {
        "/h" | "/help" => Ok(Command::Help),

        "/q" | "/quit" | "/exit" => Ok(Command::Quit),

        "/g" | "/gen" | "/generate" => Ok(Command::Generate(parse_service::<usize>(&args)?)),

        "/r" | "/rm" | "/remove" => {
            let target = Target::new(
                Option::from(next_required::<String>(&args, 0)?),
                next_optional::<String>(&args, 1)?,
            );
            Ok(Command::Remove(target))
        }

        "/e" | "/edit" => {
            let target = Target::new(
                Option::from(next_required::<String>(&args, 0)?),
                Option::from(next_required::<String>(&args, 1)?),
            );
            let service = parse_service::<String>(&args[2..])?;
            Ok(Command::Edit((target, service)))
        }

        "/cp" | "/copy" => {
            let target = Target::new(
                Option::from(next_required::<String>(&args, 0)?),
                Option::from(next_required::<String>(&args, 1)?),
            );
            Ok(Command::Copy(target))
        }

        "/a" | "/add" => Ok(Command::Add(parse_service::<String>(&args)?)),

        "/fg" | "/fastgen" => Ok(Command::FastGenerate(next_required::<usize>(&args, 0)?)),

        "/l" | "/list" => {
            let target = Target::new(
                next_optional::<String>(&args, 0)?,
                next_optional::<String>(&args, 1)?,
            );
            Ok(Command::List((target, flags)))
        }

        "/c" | "/clear" => Ok(Command::Clear),

        "/s" | "/save" => Ok(Command::SaveWithRewrite),

        "/d" | "/dump" => Ok(Command::Dump),

        other => Ok(Command::Unknown(other.to_string())),
    }
}
