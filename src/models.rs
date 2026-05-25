pub struct Service {
    pub(crate) service_name: String,
    pub(crate) login: String,
    pub(crate) password: String,
}

impl std::fmt::Display for Service {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        write!(f, "https://{} - {} : {}", self.service_name, self.login, self.password)
    }
}