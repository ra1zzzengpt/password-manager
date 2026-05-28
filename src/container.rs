use crate::errors::AppError;
use crate::files::{read_from_file, write_to_file};
use crate::models::Service;
pub struct Container {
    pub services: Vec<Service>,
}

impl Container {
    pub fn new() -> Container {
        Container {
            services: Vec::new(),
        }
    }

    pub fn add_service(&mut self, service: Service) {
        self.services.push(service);
    }

    pub fn show(&self) {
        for service in &self.services {
            println!("{}", service);
        }
    }

    pub fn load(&mut self) -> Result<(), AppError> {
        match read_from_file(&std::path::Path::new("save.txt")) {
            Ok(parsed_services) => {
                self.services = parsed_services;
            }
            Err(error) => return Err(error),
        }
        Ok(())
    }

    pub fn save(&self) -> Result<(), AppError> {
        for service in &self.services {
            match write_to_file(&std::path::Path::new("save.txt"), &*service.to_string()) {
                Ok(_) => continue,
                Err(error) => return Err(error),
            }
        }
        Ok(())
    }
}
