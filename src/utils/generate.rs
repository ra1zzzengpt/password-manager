use crate::models::errors::{AppError, ErrorType};
use rand::Rng;

const ALLOWED_SYMBOLS: &[u8] = b"abcdefghijklmnopqrstuvwxyz\
                             ABCDEFGHIJKLMNOPQRSTUVWXYZ\
                             0123456789!@#$%^&*()";
const MAX_LENGTH: usize = 10000;

pub fn generate_password(length: usize) -> Result<String, AppError> {
    if length > MAX_LENGTH {
        return Err(AppError::new(
            ErrorType::AllocationError,
            format!("memory allocation error for {length}"),
        ));
    }
    let mut password: String = String::with_capacity(length);
    let mut rng = rand::thread_rng();
    for _ in 0..length {
        password.push(ALLOWED_SYMBOLS[rng.gen_range(0..ALLOWED_SYMBOLS.len())] as char)
    }
    Ok(password)
}
