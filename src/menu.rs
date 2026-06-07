use crate::core::controller::{CommandRunner, Controller};
use crate::utils::console::{console_clear, read};

pub fn menu_run() {
    console_clear();
    print_menu();
    let mut appcontroller = Controller::default();
    if let Err(e) = appcontroller.load() {
        eprintln!("{e}");
    }
    loop {
        match appcontroller.command_runner(read::<String>("> ").as_str()) {
            Ok(cmd_runner) => match cmd_runner {
                CommandRunner::CommandSuccess(_) => continue,

                CommandRunner::CommandSuccessWithOutput(result) => println!("{result}"),

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
            Err(error) => eprintln!("{}", term_ansi::red!("{error}")),
        }
    }
}

fn print_help() {
    println!("COMMANDS");
    println!("command <REQUIRED>  [OPTIONAL]  --flag");
    println!("/help /h");
    println!(" └─ display this help");
    println!("/add /a  <SERVICE> <LOGIN> <PASSWORD>");
    println!(" └─ add a service with your own password");
    println!("/gen /generate /g  <SERVICE> <LOGIN> <LENGTH>");
    println!(" └─ add a service with a generated password");
    println!("/copy /cp  <SERVICE> <LOGIN>");
    println!(" └─ copy a service's password to the clipboard");
    println!("/rm /remove /r  <SERVICE> [LOGIN]");
    println!(" └─ remove a service, optionally narrowed by login");
    println!("/edit /e  <SERVICE> <LOGIN> <NEW_SERVICE> <NEW_LOGIN> <NEW_PASSWORD>");
    println!(" └─ replace a matching entry with a new service, login and password");
    println!("/fastgen /fg  <LENGTH>");
    println!(" └─ generate a one-off password (not saved)");
    println!("/list /l  [SERVICE] [LOGIN]  [--show]");
    println!(" └─ list every service, or filter by name (and login)");
    println!("/save /s");
    println!(" └─ save passwords to file (overwrite)");
    println!("/dump /d");
    println!(" └─ export the whole vault to a timestamped backup file");
    println!("/clear /c");
    println!(" └─ clear the screen");
    println!("/quit /q /exit");
    println!(" └─ exit the manager");
    println!();
    println!("FLAGS");
    println!("--show --s");
    println!(" └─ with /list: reveal real passwords instead of ****");
}

fn print_menu() {
    println!("_____ _ _ _ ____  _____");
    println!("|  _  | | | |    \\|     |");
    println!("|   __| | | |  |  | | | |");
    println!("|__|  |_____|____/|_|_|_|");
}
