use crate::models::errors::{AppError, ErrorType};
use crate::models::service::Service;
use crate::utils::generate::generate_password;

pub trait PasswordSource {
    fn get_password(self) -> String;
}

impl PasswordSource for usize {
    fn get_password(self) -> String {
        generate_password(self)
    }
}
impl PasswordSource for String {
    fn get_password(self) -> String {
        self
    }
}

pub fn parse_service<T>(iter: &mut std::str::SplitWhitespace) -> Result<Service, AppError>
where
    T: std::str::FromStr + PasswordSource,
{
    let service_name = iter
        .next()
        .ok_or_else(|| {
            AppError::new(
                ErrorType::IncorrectNumberOfParameters,
                String::from("Can't find service name."),
            )
        })?
        .to_string();

    let login = iter
        .next()
        .ok_or_else(|| {
            AppError::new(
                ErrorType::IncorrectNumberOfParameters,
                String::from("Can't find login."),
            )
        })?
        .to_string();

    let password = iter
        .next()
        .ok_or_else(|| {
            AppError::new(
                ErrorType::IncorrectNumberOfParameters,
                String::from("Can't find password."),
            )
        })?
        .parse::<T>()
        .map_err(|_| {
            AppError::new(
                ErrorType::ParseError,
                String::from("Can't parse password length in usize."),
            )
        })?
        .get_password();

    Ok(Service::new(service_name, login, password))
}
