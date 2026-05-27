pub struct AppError {
    pub err_type: ErrorType,
    pub message: String,
}

pub enum ErrorType {
    ParseError,
    IncorrectNumberOfParameters,
    IncorrectPasswordLength,
}

impl ErrorType {
    pub fn to_string(&self) -> String {
        match &self {
            ErrorType::IncorrectNumberOfParameters => String::from("ParametersError"),
            ErrorType::ParseError => String::from("ParseError"),
            ErrorType::IncorrectPasswordLength => String::from("IncorrectPasswordLengthError"),
        }
    }
}

impl std::fmt::Display for AppError {
    fn fmt(&self, formatter: &mut std::fmt::Formatter) -> std::fmt::Result {
        write!(formatter, "[{}]: {}", self.err_type.to_string(), self.message)
    }
}