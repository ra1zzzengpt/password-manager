use crate::models::errors::{AppError, ErrorType};

pub fn next_required<T: std::str::FromStr>(
    iter: &mut std::str::SplitWhitespace,
) -> Result<T, AppError> {
    iter.next()
        .ok_or_else(|| {
            AppError::new(
                ErrorType::IncorrectNumberOfParameters,
                String::from("Can't find argument."),
            )
        })?
        .parse::<T>()
        .map_err(|_| {
            AppError::new(
                ErrorType::ParseError,
                String::from("Can't parse parameter."),
            )
        })
}

pub fn next_optional<T: std::str::FromStr>(
    iter: &mut std::str::SplitWhitespace,
) -> Result<Option<T>, AppError> {
    match iter.next() {
        None => Ok(None),
        Some(raw) => raw.parse::<T>().map(Some).map_err(|_| {
            AppError::new(ErrorType::ParseError, String::from("Can't parse argument."))
        }),
    }
}
