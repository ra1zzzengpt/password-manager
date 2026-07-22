pub mod core;
pub mod menu;
pub mod models;
pub mod parser;
pub mod utils;
pub mod crypto;

fn main() -> std::io::Result<()> {
    menu::menu_run();
    Ok(())
}
