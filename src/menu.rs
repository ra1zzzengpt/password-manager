use crate::models::controller::{CommandRunner, Controller};
use crate::utils::console::{console_clear, read};

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
    println!();
    println!("  {}", term_ansi::bold!("{}", "COMMANDS"));
    println!(
        "  {}",
        term_ansi::italic!("{}", "command <REQUIRED>  [OPTIONAL]")
    );
    println!();

    println!("  {}", term_ansi::cyan!("{}", "/help /h"));
    println!(
        "   {} display this help",
        term_ansi::rgb!(110, 110, 110, "{}", "└─")
    );

    println!(
        "  {}  {}",
        term_ansi::cyan!("{}", "/add /a"),
        term_ansi::yellow!("{}", "<SERVICE> <LOGIN> <PASSWORD>")
    );
    println!(
        "   {} add a service with your own password",
        term_ansi::rgb!(110, 110, 110, "{}", "└─")
    );

    println!(
        "  {}  {}",
        term_ansi::cyan!("{}", "/gen /generate /g"),
        term_ansi::yellow!("{}", "<SERVICE> <LOGIN> <LENGTH>")
    );
    println!(
        "   {} add a service with a generated password",
        term_ansi::rgb!(110, 110, 110, "{}", "└─")
    );

    println!(
        "  {}  {}",
        term_ansi::cyan!("{}", "/rm /remove /r"),
        term_ansi::yellow!("{}", "<SERVICE>")
    );
    println!(
        "   {} remove a service",
        term_ansi::rgb!(110, 110, 110, "{}", "└─")
    );

    println!(
        "  {}  {}",
        term_ansi::cyan!("{}", "/fastgen /fg"),
        term_ansi::yellow!("{}", "<LENGTH>")
    );
    println!(
        "   {} generate a one-off password (not saved)",
        term_ansi::rgb!(110, 110, 110, "{}", "└─")
    );

    println!(
        "  {}  {}",
        term_ansi::cyan!("{}", "/list /l"),
        term_ansi::yellow!("{}", "[SERVICE]")
    );
    println!(
        "   {} list every service, or one by name",
        term_ansi::rgb!(110, 110, 110, "{}", "└─")
    );

    println!("  {}", term_ansi::cyan!("{}", "/save /s"));
    println!(
        "   {} save passwords to file (overwrite)",
        term_ansi::rgb!(110, 110, 110, "{}", "└─")
    );

    println!("  {}", term_ansi::cyan!("{}", "/clear /c"));
    println!(
        "   {} clear the screen",
        term_ansi::rgb!(110, 110, 110, "{}", "└─")
    );

    println!("  {}", term_ansi::cyan!("{}", "/quit /q /exit"));
    println!(
        "   {} exit the manager",
        term_ansi::rgb!(110, 110, 110, "{}", "└─")
    );
    println!();
}

fn print_menu() {
    println!("_____ _ _ _ ____  _____");
    println!("|  _  | | | |    \\|     |");
    println!("|   __| | | |  |  | | | |");
    println!("|__|  |_____|____/|_|_|_|");
}
