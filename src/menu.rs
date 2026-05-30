use crate::models::controller::{AppController, CommandRunner};
use crate::utils::console::read;

pub fn menu_print() {
    println!("_____ _ _ _ ____  _____");
    println!("|  _  | | | |    \\|     |");
    println!("|   __| | | |  |  | | | |");
    println!("|__|  |_____|____/|_|_|_|");
    let mut appcontroller = AppController::default();
    match appcontroller.load() {
        Ok(_) => (),
        Err(e) => {
            eprintln!("{}", e);
        }
    }
    loop {
        match appcontroller.command_runner(read::<String>("> ")) {
            CommandRunner::CommandSuccess(success) => {
                println!("{}", success);
            }
            CommandRunner::CommandFailure(failure) => {
                eprintln!("{}", failure);
            }
            CommandRunner::CommandHelp => {
                print_help();
            }
            CommandRunner::UnknownCommand(command) => {
                eprintln!("Unknown command: {}, use /help for know commands.", command);
            }
            CommandRunner::CommandExit => {
                break;
            }
        }
    }
}

fn print_help() {
    println!("/help | h - display this help");
    println!("/add | a - [SERVICE NAME] [LOGIN] [PASSWORD] - add your service to manager");
    println!(
        "/gen | generate | g - [SERVICE NAME] [LOGIN] [PASSWORD LENGTH] - generate random password for service"
    );
    println!("/fastgen | fg - [PASSWORD LENGTH] - generate random password (DON'T SAVE)");
    println!("/list | l - list all available services");
    println!("/save | s - save passwords to file with rewrite");
    println!("/quit | q | exit - exit");
}
