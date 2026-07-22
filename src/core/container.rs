use crate::models::errors::{AppError, ErrorType};
use crate::models::flag::Flags;
use crate::models::service::Service;
use crate::models::target::Target;
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
    #[must_use]
    pub fn new() -> Container {
        Container {
            services: Vec::new(),
        }
    }

    pub fn add_service(&mut self, service: Service) -> Result<(), AppError> {
        if self
            .services
            .iter()
            .any(|s| s.name() == service.name() && s.login() == service.login())
        {
            return Err(AppError::new(
                ErrorType::ServiceIsAvailable,
                format!(
                    "service: {} with login: {} already exists",
                    service.name(),
                    service.login()
                ),
            ));
        };
        self.services.push(service.clone());
        Container::save_to_add(&service)
    }

    pub fn load(&mut self) -> Result<(), AppError> {
        self.services = read_from_file(std::path::Path::new("save.json"))?;
        Ok(())
    }

    pub fn dump(&self) -> Result<(), AppError> {
        let mut filename = chrono::Local::now().format("%Y-%m-%d=%H:%M:%S").to_string();
        filename.push_str("-PWDM-DUMP.txt");
        rewrite_to_file(std::path::Path::new(&filename), &self.services)
    }

    pub fn save_with_rewrite(&self) -> Result<(), AppError> {
        rewrite_to_file(std::path::Path::new("save.json"), &self.services)
    }

    fn save_to_add(service: &Service) -> Result<(), AppError> {
        add_to_file(std::path::Path::new("save.json"), service)
    }

    pub fn list(&self, target: &Target, flags: &Flags) -> String {
        let mut services = match (&target.name(), &target.login()) {
            (Some(name), Some(login)) => self
                .services
                .iter()
                .filter(|service| service.name() == *name && service.login() == *login)
                .collect::<Vec<&Service>>(),
            (Some(name), None) => self
                .services
                .iter()
                .filter(|service| service.name() == *name)
                .collect::<Vec<&Service>>(),
            (None, _) => self.services.iter().collect::<Vec<&Service>>(),
        };
        services.sort_by(|s1, s2| s1.name().cmp(s2.name()));
        if !services.is_empty() {
            if flags.show {
                services
                    .iter()
                    .map(|service| service.to_string())
                    .collect::<Vec<String>>()
                    .join("\n")
            } else {
                services
                    .iter()
                    .map(|service| service.to_string_hidden_password())
                    .collect::<Vec<String>>()
                    .join("\n")
            }
        } else {
            String::from("No services found")
        }
    }

    pub fn remove(&mut self, target_service: &Target) -> Result<(), AppError> {
        let length = self.services.len();
        match (
            target_service.name().as_ref(),
            target_service.login().as_ref(),
        ) {
            (Some(name), Some(login)) => {
                self.services
                    .retain(|service| service.name() != *name || service.login() != *login);
            }
            (Some(name), None) => {
                self.services.retain(|service| service.name() != *name);
            }
            _ => (), // <- This place can't be used (target_service MUST have service_name in parsers)
        }
        if length == self.services.len() {
            return Err(AppError::new(
                ErrorType::NotFound,
                format!(
                    "Service {} not found",
                    target_service.name().as_ref().unwrap()
                ), // unwrap here can be used because we have service_name
            ));
        };
        self.save_with_rewrite()
    }

    pub fn edit(&mut self, target_service: &Target, new_service: Service) -> Result<(), AppError> {
        self.remove(target_service)?;
        self.add_service(new_service)?;
        Ok(())
    }

    pub fn find(&self, target: &Target) -> Option<&Service> {
        self.services.iter().find(|service| {
            service.name() == target.name().unwrap() && service.login() == target.login().unwrap()
        })
    }
}
