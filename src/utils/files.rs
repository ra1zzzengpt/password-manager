use crate::models::errors::{AppError, ErrorType};
use crate::models::service::Service;
use crate::parser::service::parse_service;
use std::fs::{File, OpenOptions};
use std::io::{BufRead, BufReader, Write};

pub fn rewrite_to_file(path: &std::path::Path, content: &Vec<Service>) -> Result<(), AppError> {
    let mut file = File::create(path)
        .map_err(|error| AppError::new(ErrorType::FileCreateError, error.to_string()))?;
    for service in content {
        writeln!(file, "{}", service)
            .map_err(|error| AppError::new(ErrorType::FileWriteError, error.to_string()))?;
    }
    Ok(())
}

pub fn add_to_file(path: &std::path::Path, content: &str) -> Result<(), AppError> {
    let mut file = OpenOptions::new()
        .append(true)
        .create(true)
        .open(path)
        .map_err(|error| AppError::new(ErrorType::FileOpenError, error.to_string()))?;
    writeln!(file, "{}", content)
        .map_err(|error| AppError::new(ErrorType::FileWriteError, error.to_string()))
}

pub fn read_from_file(path: &std::path::Path) -> Result<Vec<Service>, AppError> {
    let mut vector: Vec<Service> = Vec::new();
    let file = OpenOptions::new()
        .read(true)
        .open(path)
        .map_err(|error| AppError::new(ErrorType::FileOpenError, error.to_string()))?;
    let reader = BufReader::new(file);
    for line in reader.lines() {
        match line {
            Ok(line) => {
                let parsed_service = match parse_service::<String>(&mut line.split_whitespace()) {
                    Ok(service) => service,
                    Err(_) => {
                        println!(
                            "{}",
                            term_ansi::yellow!("{} {:?}", "[WARNING]: can't parse line:", line)
                        );
                        continue;
                    }
                };
                vector.push(parsed_service);
            }
            Err(_) => {
                println!(
                    "{}",
                    term_ansi::yellow!("{} {:?}", "[WARNING]: can't read line:", line)
                );
                continue;
            }
        };
    }
    Ok(vector)
}
