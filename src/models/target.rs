#[derive(Debug)]
pub struct Target {
    service_name: Option<String>,
    service_login: Option<String>,
}

impl Target {
    #[must_use]
    pub fn new(service_name: Option<String>, service_login: Option<String>) -> Self {
        Target {
            service_name,
            service_login,
        }
    }

    pub fn name(&self) -> Option<String> {
        self.service_name.clone()
    }

    pub fn login(&self) -> Option<String> {
        self.service_login.clone()
    }
}
