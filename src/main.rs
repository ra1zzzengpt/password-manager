pub mod command;
pub mod console;
pub mod container;
pub mod errors;
pub mod files;
pub mod generate;
pub mod menu;
pub mod models;
pub mod parser;

fn main() -> std::io::Result<()> {
    menu::menu_print();
    Ok(())
}
