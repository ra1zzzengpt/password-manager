use crate::models::errors::{AppError, ErrorType};
use crate::models::flag::Flags;

pub fn parse_flags(tokens: &Vec<&str>) -> Result<Flags, AppError> {
    let mut flags: Flags = Default::default();
    for token in tokens {
        let (flag, value) = match token.split_once("=") {
            Some((f, v)) => (f, Some(v)),
            None => (*token, None),
        };
        match flag {
            "--show" | "--s" => flags.show = true,
            "--generate" | "--gen" | "--g" => match value {
                Some(length) => {
                    flags.generate = length.parse::<usize>().map_err(|_| {
                        AppError::new(
                            ErrorType::ParseError,
                            format!("{token} is not a valid flag length"),
                        )
                    })?
                }
                None => {
                    return Err(AppError::new(
                        ErrorType::IncorrectFlags,
                        format!("{token} is not a valid flag"),
                    ));
                }
            },
            other => {
                return Err(AppError::new(
                    ErrorType::IncorrectFlags,
                    format!("{other} is not a valid flag"),
                ));
            }
        }
    }
    Ok(flags)
}
