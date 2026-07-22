use serde;
#[derive(Clone, serde::Serialize, serde::Deserialize)]
pub struct Service {
    service_name: String,
    login: String,
    password: String,
}

impl Service {
    pub fn new(service_name: String, login: String, password: String) -> Service {
        Service {
            service_name,
            login,
            password,
        }
    }

    pub fn name(&self) -> &str {
        &self.service_name
    }

    pub fn login(&self) -> &str {
        &self.login
    }

    pub fn password(&self) -> &str {
        &self.password
    }

    pub fn to_string_hidden_password(&self) -> String {
        format!(
            "{} {} {}",
            self.service_name,
            self.login,
            "*".repeat(self.password.chars().count())
        )
    }
}

impl std::fmt::Display for Service {
    fn fmt(&self, formatter: &mut std::fmt::Formatter) -> std::fmt::Result {
        write!(
            formatter,
            "{} {} {}",
            self.service_name, self.login, self.password
        )
    }
}
