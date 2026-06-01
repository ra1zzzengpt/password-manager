use crate::models::command::SubCommand;
use crate::models::errors::{AppError, ErrorType};
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
        Container::save_to_add(service.to_string())
    }

    pub fn load(&mut self) -> Result<(), AppError> {
        self.services = read_from_file(std::path::Path::new("save.txt"))?;
        Ok(())
    }

    pub fn save_with_rewrite(&self) -> Result<(), AppError> {
        rewrite_to_file(std::path::Path::new("save.txt"), &self.services)
    }

    fn save_to_add(content: String) -> Result<(), AppError> {
        add_to_file(std::path::Path::new("save.txt"), content.as_str())
    }

    pub fn list(&self, sub: &SubCommand) -> String {
        let services = match sub {
            SubCommand::ByService(name) => &self
                .services
                .iter()
                .filter(|service| service.name() == name)
                .collect::<Vec<&Service>>(),
            SubCommand::Default => &self.services.iter().collect::<Vec<&Service>>(),
        };
        if !services.is_empty() {
            services
                .iter()
                .map(|service| service.to_string())
                .collect::<Vec<String>>()
                .join("\n")
        } else {
            String::from("No services found")
        }
    }

    pub fn remove(&mut self, service_name : &String) -> Result<(), AppError> {
        let length = self.services.len();
        self.services.retain(|service| service.name() != service_name);
        if length == self.services.len() {
            return Err(AppError::new(ErrorType::NotFound,format!("Service {} not found", service_name)))
        };
        self.save_with_rewrite()
    }
}
