use crate::models::errors::AppError;
use crate::models::service::Service;
use crate::parser::param::next_required;
use crate::utils::generate::generate_password;

pub trait PasswordRequirement {
    fn get_password(self) -> Result<String, AppError>;
}

impl PasswordRequirement for usize {
    fn get_password(self) -> Result<String, AppError> {
        generate_password(self)
    }
}
impl PasswordRequirement for String {
    fn get_password(self) -> Result<String, AppError> {
        Ok(self)
    }
}

pub fn parse_service<T>(tokens: &[&str]) -> Result<Service, AppError>
where
    T: std::str::FromStr + PasswordRequirement,
{
    let service_name = next_required::<String>(tokens, 0)?;

    let login = next_required::<String>(tokens, 1)?;

    let password = next_required::<T>(tokens, 2)?.get_password()?;

    Ok(Service::new(service_name, login, password))
}
