use crate::models::command::Command;
use crate::models::container::Container;
use crate::models::errors::AppError;
use crate::utils::generate::generate_password;

pub enum CommandRunner {
    CommandSuccess(String),
    CommandFailure(AppError),
    CommandHelp,
    CommandExit,
    UnknownCommand(String),
}

pub struct AppController {
    container: Container,
}

impl Default for AppController {
    fn default() -> Self {
        Self::new()
    }
}

impl AppController {
    pub fn new() -> AppController {
        AppController {
            container: Container::default(),
        }
    }

    pub fn command_runner(&mut self, user_input: String) -> CommandRunner {
        match Command::from(user_input.as_str()) {
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

            Command::SaveWithRewrite => match self.container.save_with_rewrite() {
                Ok(_) => CommandRunner::CommandSuccess("Saved to save.txt".to_string()),
                Err(error) => CommandRunner::CommandFailure(error),
            },

            Command::List(subcommand) => {
                CommandRunner::CommandSuccess(self.container.list(&subcommand))
            }

            Command::Help => CommandRunner::CommandHelp,

            Command::Quit => CommandRunner::CommandExit,

            Command::Unknown(unknown) => CommandRunner::UnknownCommand(unknown),
        }
    }

    pub fn load(&mut self) -> Result<(), AppError> {
        self.container.load()
    }
}
