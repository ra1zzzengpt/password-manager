use crate::command::Command;
use crate::console::read;
use crate::container::Container;
use crate::models::Service;
use crate::parser::parse_fast_generate;

pub fn menu_print() {
    println!("_____ _ _ _ ____  _____");
    println!("|  _  | | | |    \\|     |");
    println!("|   __| | | |  |  | | | |");
    println!("|__|  |_____|____/|_|_|_|");
    let mut container = Container::default();
    match container.load() {
        Ok(_) => {}
        Err(error) => {
            eprintln!("{}", error);
        }
    }
    loop {
        match Command::from(read::<String>("> ").as_str()) {
            Command::Generate(mut split) => match Service::parse::<usize>(&mut split) {
                Ok(generate) => {
                    container.add_service(generate);
                    match container.save() {
                        Ok(_) => continue,
                        Err(error) => eprintln!("{}", error),
                    }
                }
                Err(err) => println!("{}", term_ansi::red!("{}", err)),
            },
            Command::Help => {
                print_help();
            }
            Command::FastGenerate(mut split) => match parse_fast_generate(&mut split) {
                Ok(generate) => {
                    println!("{}", generate)
                }
                Err(err) => println!("{}", term_ansi::red!("{}", err)),
            },
            Command::List => {
                container.show();
            }
            Command::Quit => return,

            Command::Unknown(command) => {
                eprintln!(
                    "Unknown command '{}'. (use /help to see available commands)",
                    command
                );
            }
        }
    }
}

fn print_help() {
    println!("/help | h - display this help");
    println!(
        "/gen | generate | g - [SERVICE NAME] [LOGIN] [PASSWORD LENGTH] - generate random password for service"
    );
    println!("/fg | fastgen - [PASSWORD LENGTH] - generate random password (DON'T SAVE)");
    println!("/list | l - list all available services");
    println!("/quit | q | exit - exit");
}
