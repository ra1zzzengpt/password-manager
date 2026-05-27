use crate::errors::{AppError,ErrorType};
use crate::models::Service;
use crate::generate::generate_password;

pub fn parse_generate(iter: &mut std::str::SplitWhitespace) -> Result<Service, AppError> {
    let mut result: Service = Service {
        service_name: String::new(),
        login: String::new(),
        password: String::new()
    };

    match iter.next() {
        Some(name) => { result.service_name = name.to_string(); },
        None => return Err(AppError::new(ErrorType::IncorrectNumberOfParameters, String::from("Can't find service name.")))
    }

    match iter.next() {
        Some(login) => { result.login = login.to_string(); },
        None => return Err(AppError::new(ErrorType::IncorrectNumberOfParameters, String::from("Can't find login.")))
    }
    match iter.next() {
        Some(password_length) => {
            match password_length.parse::<usize>() {
                Ok(password_length) => {
                    if password_length >= 8 {
                        result.password = generate_password(password_length);
                    } else { return Err(AppError::new(ErrorType::IncorrectNumberOfParameters, String::from("Can't generate password."))); }
                }
                Err(_) => return Err(AppError::new(ErrorType::ParseError, String::from("Can't parse password length in usize.")))
            }
        }
        None => return Err(AppError::new(ErrorType::IncorrectNumberOfParameters,String::from("Can't find password.")))
    }
    Ok(result)
}

pub fn parse_fast_generate(iter: &mut std::str::SplitWhitespace) -> Result<String, AppError> {
    let length = iter.next().ok_or_else(|| AppError::new(ErrorType::IncorrectNumberOfParameters, String::from("Can't find password length.")));
    match length?.parse::<usize>() {
        Ok(length) => {Ok(generate_password(length))},
        Err(_) => Err(AppError::new(ErrorType::ParseError,String::from("Can't parse password length in usize.")))
    }
}