use crate::errors::{AppError, ErrorType};
use crate::generate::generate_password;

pub fn parse_fast_generate(iter: &mut std::str::SplitWhitespace) -> Result<String, AppError> {
    let length = iter.next().ok_or_else(|| {
        AppError::new(
            ErrorType::IncorrectNumberOfParameters,
            String::from("Can't find password length."),
        )
    });
    match length?.parse::<usize>() {
        Ok(length) => Ok(generate_password(length)),
        Err(_) => Err(AppError::new(
            ErrorType::ParseError,
            String::from("Can't parse password length in usize."),
        )),
    }
}
