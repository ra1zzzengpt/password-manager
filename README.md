# PWMD — Password Manager

A small CLI password manager written in Rust. Generate or store credentials,
keep them in a local vault file, and list them back from a simple command prompt.

> ⚠️ **Not secure yet.** The vault (`save.txt`) is stored in **plaintext**.
> Don't keep real secrets in it until encryption lands — see Roadmap v0.6.

## Current version — v0.2

- Command-driven REPL with a small set of slash-commands
- Cryptographically-random password generation with a configurable length
- Persistent vault: entries are auto-loaded on startup and written to `save.txt`
- List every entry or filter by service name

### Commands

| Command     | Aliases       | Arguments                       | Description                                            |
|-------------|---------------|---------------------------------|--------------------------------------------------------|
| `/generate` | `/gen`, `/g`  | `<service> <login> <length>`    | Generate a random password of `<length>` and store it  |
| `/add`      | `/a`          | `<service> <login> <password>`  | Store an existing password                             |
| `/fastgen`  | `/fg`         | `<length>`                      | Generate a one-off password (not saved)                |
| `/list`     | `/l`          | `[service]`                     | List all entries, or only those matching `<service>`   |
| `/save`     | `/s`          | —                               | Rewrite the vault file from memory                     |
| `/help`     | `/h`          | —                               | Show available commands                                |
| `/quit`     | `/q`, `/exit` | —                               | Exit                                                   |

### Storage format

Entries live in `save.txt` in the working directory, one per line, space-separated:

```
<service> <login> <password>
github.com ra1zzzengpt 1874974343jf1723nfj1jhsad1knc
```

Entries from `/add` and `/generate` are appended immediately, while `/save`
rewrites the whole file from the current in-memory state.

## Installation

Requires [Rust](https://www.rust-lang.org/tools/install) (stable, edition 2024).

```bash
git clone https://github.com/ra1zzzengpt/password-manager.git
cd password-manager
cargo run
```

## Roadmap

- [x] **v0.2 — File storage:** persistent local vault (`save.txt`)
- [ ] **v0.3 — Filtering & lookup:** filter by login and quick lookup *(filter by service already works)*
- [ ] **v0.4 — Sub-commands:** edit / remove entries and other utilities
- [ ] **v0.5 — Dump & export:** portable export format for vault migration
- [ ] **v0.6 — Encryption:** protect the vault with a master password

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md).

## License

MIT — see [LICENSE.md](LICENSE.md).
