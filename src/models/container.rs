use crate::models::errors::AppError;
use crate::models::service::Service;
use crate::utils::files::{add_to_file, read_from_file, rewrite_to_file};
pub struct Container {
    services: Vec<Service>,
}

impl Default for Container {
    fn default() -> Self {
        Self::new()
    }
}
impl Container {
    pub fn new() -> Container {
        Container {
            services: Vec::new(),
        }
    }

    pub fn add_service(&mut self, service: &Service) -> Result<(), AppError> {
        self.services.push(service.clone());
        Container::save_to_add(service.clone().to_string())
    }

    pub fn show(&self) {
        for service in &self.services {
            println!("{}", service);
        }
    }

    pub fn load(&mut self) -> Result<(), AppError> {
        match read_from_file(std::path::Path::new("save.txt")) {
            Ok(parsed_services) => {
                self.services = parsed_services;
            }
            Err(error) => return Err(error),
        }
        Ok(())
    }

    pub fn save_with_rewrite(&self) -> Result<(), AppError> {
        match rewrite_to_file(std::path::Path::new("save.txt"), &self.services) {
            Ok(_) => Ok(()),
            Err(error) => Err(error),
        }
    }

    fn save_to_add(content: String) -> Result<(), AppError> {
        match add_to_file(std::path::Path::new("save.txt"), content.as_str()) {
            Ok(_) => Ok(()),
            Err(error) => Err(error),
        }
    }
}

impl std::fmt::Display for Container {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        let mut result = String::new();
        for service in &self.services {
            result.push_str(&service.to_string());
            result.push('\n');
        }
        write!(f, "{}", result)
    }
}
