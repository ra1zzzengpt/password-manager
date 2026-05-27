# PWMD — Password Manager

A CLI password manager written in Rust.

## Current Version — v0.1

Basic password generation with a simple command-driven interface.

**Available commands:**

| Command | Arguments | Description |
|---|---|---|
| `/gen`, `/generate`, `/g` | `<service> <login> <length>` | Generate a password and bind it to a service/login |
| `/fg`, `/fastgen` | `<length>` | Generate a standalone password |
| `/help`, `/h` | — | Show available commands |
| `/quit`, `/exit`, `/q` | — | Exit |

## Installation

Requires [Rust](https://www.rust-lang.org/tools/install) (latest stable).

```bash
git clone https://github.com/ra1zzzengpt/password-manager.git
cd password-manager
cargo run
```

## Roadmap

- **v0.2** — File storage: save generated entries to a local vault file
- **v0.3** — Filtering & output: list by service, by login, quick lookup by name
- **v0.4** — Sub-commands: additional utility functions
- **v0.5** — Dump & export: portable export format for vault migration
- **v0.6** — Encryption: protect the vault with a master password or machine binding

## License

MIT — see [LICENSE.md](LICENSE.md).
