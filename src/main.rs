pub mod menu;
pub mod console;
pub mod generate;
pub mod files;
pub mod codec;
pub mod models;
pub mod command;
pub mod errors;

fn main() -> std::io::Result<()> {
    menu::menu_print();
    Ok(())
}