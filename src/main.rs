pub mod menu;
pub mod models;
pub mod parser;
pub mod utils;

fn main() -> std::io::Result<()> {
    menu::menu_run();
    Ok(())
}
