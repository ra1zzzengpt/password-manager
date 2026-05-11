# Password Manager

A simple CLI-based password manager written in Rust.

## Current State

Currently, this tool is a basic password generator that allows you to:
- Generate random passwords of a specified length.
- Use a simple CLI menu for interaction.

## Roadmap

The project is evolving into a full-featured password manager. Future features include:
- **Vault Storage:** Securely store passwords for different websites/services.
- **Clipboard Integration:** Automatically copy selected passwords to the clipboard.
- **Encryption:** Keep your stored passwords safe with industry-standard encryption.
- **Improved UI:** Enhanced console interface and potential GUI.

## Prerequisites

- [Rust](https://www.rust-lang.org/tools/install) (latest stable version)
- [Cargo](https://doc.rust-lang.org/cargo/getting-started/installation.html)

## Installation & Running

1.  Clone the repository:
    ```bash
    git clone https://github.com/yourusername/password-manager.git
    cd password-manager
    ```
2.  Run the application:
    ```bash
    cargo run
    ```

## Usage

1.  Launch the application using `cargo run`.
2.  Select `[1] Generate new password` from the menu.
3.  Enter the desired length for your password.
4.  The generated password will be displayed in the console.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.
