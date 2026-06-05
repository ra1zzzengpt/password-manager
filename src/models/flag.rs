pub enum Flag {
    Show,
    Unknown(String),
}

impl From<&str> for Flag {
    fn from(flag: &str) -> Self {
        match flag{
            "--show" => Flag::Show,
            other => Flag::Unknown(other.to_string()),
        }
    }
}
