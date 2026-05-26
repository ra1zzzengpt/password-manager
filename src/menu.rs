use crate::console;
use crate::generate;
use crate::models;
use crate::command;

pub fn menu_print() {
    println!("_____ _ _ _ ____  _____");
    println!("|  _  | | | |    \\|     |");
    println!("|   __| | | |  |  | | | |");
    println!("|__|  |_____|____/|_|_|_|");
    loop {
        match command::Command::from(console::read::<String>("> ").as_str()) {
            command::Command::Generate => {
                let service = models::Service {
                    service_name:console::read::<String>("Service name: "),
                    login:console::read::<String>("Login: "),
                    password:generate::generate_password(console::read::<usize>("Password len: "))
                };
                println!("{}",service);
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
    println!("/quit | q - exit");
}