pub struct AppError {
    pub err_type: ErrorType,
    pub message: String,
}

pub enum ErrorType {
    ParseError,
    IncorrectNumberOfParameters,
    IncorrectPasswordLength,
}
impl std::fmt::Display for ErrorType {
    fn fmt(&self, formatter: &mut std::fmt::Formatter) -> std::fmt::Result {
        let message = match &self {
            ErrorType::IncorrectNumberOfParameters => String::from("ParametersError"),
            ErrorType::ParseError => String::from("ParseError"),
            ErrorType::IncorrectPasswordLength => String::from("IncorrectPasswordLengthError"),
        };
        write!(formatter, "{}",message)
    }
}

impl std::fmt::Display for AppError {
    fn fmt(&self, formatter: &mut std::fmt::Formatter) -> std::fmt::Result {
        write!(formatter, "[{}]: {}", self.err_type, self.message)
    }
}

impl AppError {
    pub fn new(err_type: ErrorType, message: String) -> AppError {
        AppError { err_type, message }
    }
}