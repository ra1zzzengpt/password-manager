use crate::errors;
use crate::models;
use crate::generate;

pub fn parse_generate(iter: &mut std::str::SplitWhitespace) -> Result<models::Service, errors::AppError> {
    let mut result: models::Service = models::Service { service_name: String::new(), login: String::new(), password: String::new() };
    match iter.next() {
        Some(name) => { result.service_name = name.to_string(); },
        None => return Err(errors::AppError { err_type: errors::ErrorType::IncorrectNumberOfParameters, message: String::from("Can't find service name.") })
    }
    match iter.next() {
        Some(login) => { result.login = login.to_string(); },
        None => return Err(errors::AppError { err_type: errors::ErrorType::IncorrectNumberOfParameters, message: String::from("Can't find login.") })
    }
    match iter.next() {
        Some(password_length) => {
            match password_length.parse::<usize>() {
                Ok(password_length) => {
                    if password_length >= 8 {
                        result.password = generate::generate_password(password_length);
                    } else { return Err(errors::AppError { err_type: errors::ErrorType::IncorrectPasswordLength, message: String::from("Password is too short.") }) }
                }
                Err(_) => return Err(errors::AppError { err_type: errors::ErrorType::ParseError, message: String::from("Can't parse password in usize.") })
            }
        }
        None => return Err(errors::AppError { err_type: errors::ErrorType::IncorrectNumberOfParameters, message: String::from("Can't find password.") })
    }
    Ok(result)
}