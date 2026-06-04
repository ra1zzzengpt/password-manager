use crate::models::controller::{CommandRunner, Controller};
use crate::utils::console::{console_clear, read};

pub fn menu_run() {
    console_clear();
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
    println!("COMMANDS");
    println!(
        "{}",
        term_ansi::bold!("{}", "command <REQUIRED>  [OPTIONAL]")
    );
    println!("/help /h");
    println!(" └─ display this help");
    println!("/add /a  <SERVICE> <LOGIN> <PASSWORD>");
    println!(" └─ add a service with your own password");
    println!("/gen /generate /g  <SERVICE> <LOGIN> <LENGTH>");
    println!(" └─ add a service with a generated password");
    println!("/rm /remove /r  <SERVICE> [LOGIN]");
    println!(" └─ remove a service, optionally narrowed by login");
    println!("/fastgen /fg  <LENGTH>");
    println!(" └─ generate a one-off password (not saved)");
    println!("/list /l  [SERVICE] [LOGIN]");
    println!(" └─ list every service, or filter by name (and login)");
    println!("/save /s");
    println!(" └─ save passwords to file (overwrite)");
    println!("/clear /c");
    println!(" └─ clear the screen");
    println!("/quit /q /exit");
    println!(" └─ exit the manager");
}

fn print_menu() {
    println!("_____ _ _ _ ____  _____");
    println!("|  _  | | | |    \\|     |");
    println!("|   __| | | |  |  | | | |");
    println!("|__|  |_____|____/|_|_|_|");
}
