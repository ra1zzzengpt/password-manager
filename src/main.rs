pub mod menu;
pub mod console;
pub mod generate;
pub mod files;
pub mod codec;

fn main() -> std::io::Result<()> {
    menu::menu_print();
    Ok(())
}