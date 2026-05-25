use crate::console;
use crate::generate;
use crate::models;

pub fn menu_print() {
    println!("_____ _ _ _ ____  _____");
    println!("|  _  | | | |    \\|     |");
    println!("|   __| | | |  |  | | | |");
    println!("|__|  |_____|____/|_|_|_|");
    loop {
        match console::read::<String>("> ").as_str() {
            "/gen" => {
                let service = models::Service {
                    service_name:console::read::<String>("Service name: "),
                    login:console::read::<String>("Login: "),
                    password:generate::generate_password(console::read::<usize>("Password len: "))
                };
                println!("{}",service);
            }
            "/help" => {println!("/gen - generate new password\n/help - display this help\n/exit - exit");}
            "/exit" => return,
            _=> {
                println!("Unknown command. (use /help to see available commands)");
                continue;
            }
        }
    }
}