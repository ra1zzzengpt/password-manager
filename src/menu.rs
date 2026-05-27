use crate::console::{read};
use crate::command::{Command};
use crate::parser::{parse_fast_generate, parse_generate};

pub fn menu_print() {
    println!("_____ _ _ _ ____  _____");
    println!("|  _  | | | |    \\|     |");
    println!("|   __| | | |  |  | | | |");
    println!("|__|  |_____|____/|_|_|_|");
    loop {
        match Command::from(read::<String>("> ").as_str()) {
            Command::Generate(mut split) => {
                match parse_generate(&mut split) {
                    Ok(generate) => {
                        println!("{}", generate)
                    }
                    Err(err) => println!("{}", term_ansi::red!("{}", err)),
                }
            }
            Command::Help => {
                print_help();
            }
            Command::FastGenerate(mut split) => {
                match parse_fast_generate(&mut split) {
                    Ok(generate) => {
                        println!("{}", generate)
                    }
                    Err(err) => println!("{}", term_ansi::red!("{}", err)),
                }
            }
            Command::Quit => return,

            Command::Unknown(command) => {
                eprintln!("Unknown command '{}'. (use /help to see available commands)", command);
            }
        }
    }
}

fn print_help() {
    println!("/help | h - display this help");
    println!("/gen | generate | g - [SERVICE NAME] [LOGIN] [PASSWORD LENGTH] - generate random password for service");
    println!("/fg | fastgen - [PASSWORD LENGTH] - generate random password");
    println!("/quit | q | exit - exit");
}