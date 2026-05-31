#[derive(Debug)]
pub struct AppError {
    pub err_type: ErrorType,
    pub message: String,
}

#[derive(Debug)]
pub enum ErrorType {
    ParseError,
    IncorrectNumberOfParameters,
    IncorrectPasswordLength,
    FileOpenError,
    FileWriteError,
    FileCreateError,
}
impl std::fmt::Display for ErrorType {
    fn fmt(&self, formatter: &mut std::fmt::Formatter) -> std::fmt::Result {
        let message = match &self {
            ErrorType::IncorrectNumberOfParameters => String::from("ParametersError"),
            ErrorType::ParseError => String::from("ParseError"),
            ErrorType::IncorrectPasswordLength => String::from("IncorrectPasswordLengthError"),
            ErrorType::FileOpenError => String::from("FileOpenError"),
            ErrorType::FileWriteError => String::from("FileWriteError"),
            ErrorType::FileCreateError => String::from("FileCreateError"),
        };
        write!(formatter, "{}", message)
    }
}

impl std::fmt::Display for AppError {
    fn fmt(&self, formatter: &mut std::fmt::Formatter) -> std::fmt::Result {
        write!(formatter, "[{}]: {}", self.err_type, self.message)
    }
}

impl AppError {
    #[must_use]
    pub fn new(err_type: ErrorType, message: String) -> AppError {
        AppError { err_type, message }
    }
}
