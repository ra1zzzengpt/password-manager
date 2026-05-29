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
    let service_name = match iter.next() {
        Some(name) => name.to_string(),
        None => {
            return Err(AppError::new(
                ErrorType::IncorrectNumberOfParameters,
                String::from("Can't find service name."),
            ));
        }
    };

    let login = match iter.next() {
        Some(login) => login.to_string(),
        None => {
            return Err(AppError::new(
                ErrorType::IncorrectNumberOfParameters,
                String::from("Can't find login."),
            ));
        }
    };
    let password = match iter.next() {
        Some(password_length) => match password_length.parse::<T>() {
            Ok(password_info) => password_info.get_password(),
            Err(_) => {
                return Err(AppError::new(
                    ErrorType::ParseError,
                    String::from("Can't parse password length in usize."),
                ));
            }
        },
        None => {
            return Err(AppError::new(
                ErrorType::IncorrectNumberOfParameters,
                String::from("Can't find password."),
            ));
        }
    };
    Ok(Service::new(service_name, login, password))
}
