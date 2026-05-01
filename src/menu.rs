use crate::console;
use crate::generate;

pub fn menu_print() {
    loop {
        println!(" _  _  _ _   _  _ _|   _ _  _  _  _  _  _  _");
        println!(r"|_)(_|_\_\VV(_)| (_|  | | |(_|| |(_|(_|(/_| ");
        println!("|                                    _|    ");
        println!("[1] Generate new password");
        println!("[2] Exit");
        match console::read_i32("Enter your choice: ") {
            1=> {
                println!("{}", generate::generate_password(
                    console::read_i32("Enter a length: ") as usize));
            }
            2=> return,
            _=> {
                println!("Invalid choice");
                continue;
            }
        }
    }
}