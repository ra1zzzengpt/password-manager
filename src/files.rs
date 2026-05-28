use crate::errors::{AppError, ErrorType};
use crate::models::Service;
use std::fs::OpenOptions;
use std::io::{BufRead, BufReader, Write};

pub fn write_to_file(path: &std::path::Path, content: &str) -> Result<(), AppError> {
    let mut file = match OpenOptions::new().append(true).create(true).open(path) {
        Ok(file) => file,
        Err(_) => {
            return Err(AppError::new(
                ErrorType::FileOpenError,
                String::from("Cannot open file with for writing."),
            ));
        }
    };
    match writeln!(file, "{}", content) {
        Ok(_) => Ok(()),
        Err(_) => Err(AppError::new(
            ErrorType::FileWriteError,
            String::from("Cannot write content."),
        )),
    }
}

pub fn read_from_file(path: &std::path::Path) -> Result<Vec<Service>, AppError> {
    let mut vector: Vec<Service> = Vec::new();
    let file = match OpenOptions::new().read(true).open(path) {
        Ok(file) => file,
        Err(_) => {
            return Err(AppError::new(
                ErrorType::FileOpenError,
                String::from("Cannot open file with for reading."),
            ));
        }
    };
    let reader = BufReader::new(file);
    for line in reader.lines() {
        match line {
            Ok(line) => {
                let parsed_service =
                    match Service::parse::<String>(&mut line.trim().split_whitespace()) {
                        Ok(service) => service,
                        Err(_) => {
                            return Err(AppError::new(
                                ErrorType::ParseError,
                                String::from("Cannot parse service."),
                            ));
                        }
                    };
                vector.push(parsed_service);
            }
            Err(_) => {
                println!(
                    "{}",
                    term_ansi::yellow!("{} {:?}", "[WARNING]: can't read line: ", line)
                );
                continue;
            }
        };
    }
    Ok(vector)
}
