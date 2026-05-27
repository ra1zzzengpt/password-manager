pub mod menu;
pub mod console;
pub mod generate;
pub mod models;
pub mod command;
pub mod errors;
pub mod parser;

fn main() -> std::io::Result<()> {
    menu::menu_print();
    Ok(())
}