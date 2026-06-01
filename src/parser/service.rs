use crate::models::errors::AppError;
use crate::models::service::Service;
use crate::parser::param::next_required;
use crate::utils::generate::generate_password;

pub trait PasswordSource {
    fn get_password(self) -> String;
}

impl PasswordSource for usize {
    fn get_password(self) -> String {
        generate_password(self)
    }
}
impl PasswordSource for String {
    fn get_password(self) -> String {
        self
    }
}

pub fn parse_service<T>(iter: &mut std::str::SplitWhitespace) -> Result<Service, AppError>
where
    T: std::str::FromStr + PasswordSource,
{
    let service_name = next_required::<String>(iter)?;

    let login = next_required::<String>(iter)?;

    let password = next_required::<T>(iter)?.get_password();

    Ok(Service::new(service_name, login, password))
}
