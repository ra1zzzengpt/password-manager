use crate::core::container::Container;
use crate::models::command::Command;
use crate::models::errors::{AppError, ErrorType};
use crate::models::target::Target;
use crate::parser::command::parse_command;
use crate::utils::generate::generate_password;

pub enum CommandRunner {
    CommandSuccess(()),
    CommandSuccessWithOutput(String),
    CommandClear,
    CommandHelp,
    CommandExit,
    UnknownCommand(String),
}

pub struct Controller {
    container: Container,
}

impl Default for Controller {
    fn default() -> Self {
        Self::new()
    }
}

impl Controller {
    #[must_use]
    pub fn new() -> Controller {
        Controller {
            container: Container::default(),
        }
    }

    pub fn command_runner(&mut self, user_input: &str) -> Result<CommandRunner, AppError> {
        match parse_command(user_input) {
            Ok(command) => match command {
                Command::Generate(service) | Command::Add(service) => Ok(CommandRunner::CommandSuccess(self.container.add_service(service)?)),

                Command::FastGenerate(length) => Ok(CommandRunner::CommandSuccessWithOutput(generate_password(length))),

                Command::Remove(target) => Ok(CommandRunner::CommandSuccess(self.container.remove(&target)?)),

                Command::SaveWithRewrite => Ok(CommandRunner::CommandSuccess(self.container.save_with_rewrite()?)),

                Command::Copy(target) => Ok(CommandRunner::CommandSuccess(self.copy_to_clipboard(&target)?)) ,

                Command::Edit((target,service)) => Ok(CommandRunner::CommandSuccess(self.container.edit(&target, service)?)),

                Command::Dump => Ok(CommandRunner::CommandSuccess(self.container.dump()?)),

                Command::List((target,flags)) => Ok(CommandRunner::CommandSuccessWithOutput(self.container.list(&target, &flags))),

                Command::Clear => Ok(CommandRunner::CommandClear),

                Command::Help => Ok(CommandRunner::CommandHelp),

                Command::Quit => Ok(CommandRunner::CommandExit),

                Command::Unknown(unknown) => Ok(CommandRunner::UnknownCommand(unknown)),
            },
            Err(error) => Err(error),
        }
    }

    pub fn load(&mut self) -> Result<(), AppError> {
        self.container.load()
    }

    pub fn copy_to_clipboard(&self, target : &Target) -> Result<(), AppError> {
        match self.container.find(&target) {
            Some(result) => {
                let mut clipboard = match arboard::Clipboard::new() {
                    Ok(clipboard) => clipboard,
                    Err(error) =>
                        return Err(AppError::new(
                            ErrorType::ArboardError,
                            error.to_string(),
                        )),
                };
                match clipboard.set_text(result.password().to_string()) {
                    Ok(_) => Ok(()),
                    Err(error) => Err(AppError::new(
                        ErrorType::ArboardError,
                        error.to_string(),
                    )),
                }
            }
            None => Err(AppError::new(
                ErrorType::NotFound,
                format!("{:?} not found.", target),
            )),
        }
    }
}
