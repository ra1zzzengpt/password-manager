pub struct AppError {
    pub err_type: ErrorType,
    pub message: String,
}

pub enum ErrorType {
    IteratorError,
    ParseError,
}

impl ErrorType {
    pub fn to_string(&self) -> String {
        match &self {
            ErrorType::IteratorError => String::from("IteratorError"),
            ErrorType::ParseError => String::from("ParseError"),
        }
    }
}

impl std::fmt::Display for AppError {
    fn fmt(&self, formatter: &mut std::fmt::Formatter) -> std::fmt::Result {
        write!(formatter, "[{}]: {}", self.err_type.to_string(), self.message)
    }
}