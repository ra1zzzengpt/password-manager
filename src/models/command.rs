use crate::models::flag::Flags;
use crate::models::service::Service;
use crate::models::target::Target;

pub enum Command {
    Generate(Service),
    Add(Service),
    FastGenerate(usize),
    Remove(Target),
    Edit((Target, Service)),
    SaveWithRewrite,
    Copy(Target),
    Clear,
    List((Target, Flags)),
    Dump,
    Help,
    Quit,
    Unknown(String),
}
