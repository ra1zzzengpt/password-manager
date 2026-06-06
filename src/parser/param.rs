use crate::models::errors::{AppError, ErrorType};

pub fn next_required<T: std::str::FromStr>(tokens: &[&str], index: usize) -> Result<T, AppError> {
    match tokens.get(index) {
        Some(raw) => raw.parse::<T>().map_err(|_| {
            AppError::new(
                ErrorType::ParseError,
                format!("can't parse parameter: {raw}"),
            )
        }),
        None => Err(AppError::new(
            ErrorType::IncorrectNumberOfParameters,
            "can't find <REQUIRED> parameter.".to_string(),
        )),
    }
}

pub fn next_optional<T: std::str::FromStr>(
    tokens: &[&str],
    index: usize,
) -> Result<Option<T>, AppError> {
    match tokens.get(index) {
        None => Ok(None),
        Some(raw) => raw.parse::<T>().map(Some).map_err(|_| {
            AppError::new(
                ErrorType::ParseError,
                format!("Can't parse [OPTIONAL] parameter: {raw}."),
            )
        }),
    }
}
