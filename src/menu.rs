use crate::console;
use crate::command;
use crate::parser::parse_generate;

pub fn menu_print() {
    println!("_____ _ _ _ ____  _____");
    println!("|  _  | | | |    \\|     |");
    println!("|   __| | | |  |  | | | |");
    println!("|__|  |_____|____/|_|_|_|");
    loop {
        match command::Command::from(console::read::<String>("> ").as_str()) {
            command::Command::Generate(mut split) => {
                match parse_generate(&mut split) {
                    Ok(generate) => {println!("{}", generate)}
                    Err(err) => println!("{}", err),
                }
            }
            command::Command::Help => {print_help();}
            command::Command::Quit => return,
            command::Command::Unknown(command) => {
                eprintln!("Unknown command '{}'. (use /help to see available commands)", command);
                continue;
            }
        }
    }
}

fn print_help() {
    println!("/help | h - display this help");
    println!("/gen | generate | g - [SERVICE NAME] [LOGIN] [PASSWORD LENGTH] - generate random password");
    println!("/quit | q | exit - exit");
}