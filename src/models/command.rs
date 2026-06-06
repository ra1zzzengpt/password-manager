use crate::models::errors::AppError;
use crate::models::flag::Flags;
use crate::models::service::Service;
use crate::models::target::Target;

pub enum Command {
    Generate(Result<Service, AppError>),
    Add(Result<Service, AppError>),
    FastGenerate(Result<usize, AppError>),
    Remove(Result<Target, AppError>),
    SaveWithRewrite,
    Copy(Result<Target, AppError>),
    Clear,
    List(Result<Target, AppError>, Flags),
    Help,
    Quit,
    Unknown(String),
}
