use crate::models::controller::{Controller, CommandRunner};
use crate::utils::console::{read,console_clear};

pub fn menu_run() {
    print_menu();
    let mut appcontroller = Controller::default();
    if let Err(e) = appcontroller.load() {
        eprintln!("{e}");
    }
    loop {
        match appcontroller.command_runner(read::<String>("> ")) {
            CommandRunner::CommandSuccess(success) => {
                println!("{success}");
            }
            CommandRunner::CommandFailure(failure) => {
                eprintln!("{failure}");
            }
            CommandRunner::CommandClear => {
                console_clear();
                print_menu();
            }
            CommandRunner::CommandHelp => {
                print_help();
            }
            CommandRunner::UnknownCommand(command) => {
                eprintln!("Unknown command: {command}, use /help for know commands.");
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
    println!("/rm | r - [SERVICE NAME] remove service");
    println!("/fastgen | fg - [PASSWORD LENGTH] - generate random password (DON'T SAVE)");
    println!("/list | l - list all available services");
    println!("/save | s - save passwords to file with rewrite");
    println!("/clear | c - clear screen");
    println!("/quit | q | exit - exit");
}

fn print_menu() {
    println!("_____ _ _ _ ____  _____");
    println!("|  _  | | | |    \\|     |");
    println!("|   __| | | |  |  | | | |");
    println!("|__|  |_____|____/|_|_|_|");
}