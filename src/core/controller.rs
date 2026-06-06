use crate::core::container::Container;
use crate::models::command::Command;
use crate::models::errors::{AppError, ErrorType};
use crate::parser::command::parse_command;
use crate::utils::generate::generate_password;

pub enum CommandRunner {
    CommandSuccess(String),
    CommandFailure(AppError),
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

    pub fn command_runner(&mut self, user_input: String) -> CommandRunner {
        match parse_command(user_input.as_str()) {
            Ok(command) => match command {
                Command::Generate(result) | Command::Add(result) => match result {
                    Ok(service) => match self.container.add_service(&service) {
                        Ok(_) => CommandRunner::CommandSuccess(service.to_string()),
                        Err(error) => CommandRunner::CommandFailure(error),
                    },
                    Err(error) => CommandRunner::CommandFailure(error),
                },

                Command::FastGenerate(res_fg) => match res_fg {
                    Ok(length) => CommandRunner::CommandSuccess(generate_password(length)),
                    Err(error) => CommandRunner::CommandFailure(error),
                },

                Command::Remove(res) => match res {
                    Ok(res) => match self.container.remove(&res) {
                        Ok(_) => CommandRunner::CommandSuccess("Deleted".to_string()),
                        Err(error) => CommandRunner::CommandFailure(error),
                    },
                    Err(error) => CommandRunner::CommandFailure(error),
                },

                Command::SaveWithRewrite => match self.container.save_with_rewrite() {
                    Ok(_) => CommandRunner::CommandSuccess("Saved to save.txt".to_string()),
                    Err(error) => CommandRunner::CommandFailure(error),
                },

                Command::Copy(res) => match res {
                    Ok(target) => match self.container.find(&target) {
                        Some(result) => {
                            let mut clipboard = match arboard::Clipboard::new() {
                                Ok(clipboard) => clipboard,
                                Err(error) => {
                                    return CommandRunner::CommandFailure(AppError::new(
                                        ErrorType::ArboardError,
                                        error.to_string(),
                                    ));
                                }
                            };
                            match clipboard.set_text(result.password().to_string()) {
                                Ok(_) => CommandRunner::CommandSuccess("Copied".to_string()),
                                Err(error) => CommandRunner::CommandFailure(AppError::new(
                                    ErrorType::ArboardError,
                                    error.to_string(),
                                )),
                            }
                        }
                        None => CommandRunner::CommandFailure(AppError::new(
                            ErrorType::NotFound,
                            format!("{:?} not found.", target),
                        )),
                    },
                    Err(error) => CommandRunner::CommandFailure(error),
                },

                Command::List(target, flags) => match target {
                    Ok(target) => {
                        CommandRunner::CommandSuccess(self.container.list(&target, flags))
                    }
                    Err(error) => CommandRunner::CommandFailure(error),
                },

                Command::Clear => CommandRunner::CommandClear,

                Command::Help => CommandRunner::CommandHelp,

                Command::Quit => CommandRunner::CommandExit,

                Command::Unknown(unknown) => CommandRunner::UnknownCommand(unknown),
            },
            Err(error) => CommandRunner::CommandFailure(error),
        }
    }

    pub fn load(&mut self) -> Result<(), AppError> {
        self.container.load()
    }
}
