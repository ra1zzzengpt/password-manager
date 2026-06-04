use crate::models::errors::{AppError, ErrorType};

pub fn next_required<T: std::str::FromStr>(
    iter: &mut std::str::SplitWhitespace,
) -> Result<T, AppError> {
    match iter.next() {
        Some(raw) => raw.parse::<T>().map_err(|_| {
            AppError::new(
                ErrorType::ParseError,
                format!("can't parse parameter: {}", raw),
            )
        }),
        None => Err(AppError::new(
            ErrorType::IncorrectNumberOfParameters,
            "can't find <REQUIRED> parameter.".to_string(),
        )),
    }
}

pub fn next_optional<T: std::str::FromStr>(
    iter: &mut std::str::SplitWhitespace,
) -> Result<Option<T>, AppError> {
    match iter.next() {
        None => Ok(None),
        Some(raw) => raw.parse::<T>().map(Some).map_err(|_| {
            AppError::new(
                ErrorType::ParseError,
                format!("Can't parse [OPTIONAL] parameter: {}.", raw),
            )
        }),
    }
}
