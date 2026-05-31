use crate::models::errors::{AppError, ErrorType};

pub fn parse_param<T: std::str::FromStr>(
    iter: &mut std::str::SplitWhitespace,
) -> Result<T, AppError> {
    let parameter = iter.next().ok_or_else(|| {
        AppError::new(
            ErrorType::IncorrectNumberOfParameters,
            String::from("Can't find password length."),
        )
    })?;
    parameter.parse::<T>().map_err(|_| {
        AppError::new(
            ErrorType::ParseError,
            String::from("Can't parse parameter."),
        )
    })
}
