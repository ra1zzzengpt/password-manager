# PWMD — Password Manager

A small CLI password manager written in Rust. Generate or store credentials,
keep them in a local vault file, and manage them from a simple command prompt.

> ⚠️ **Not secure yet.** The vault (`save.txt`) is stored in **plaintext**.
> Don't keep real secrets in it until encryption lands — see Roadmap v0.6.
> `/copy` also places the password on the system clipboard, where other apps can
> read it and it is not cleared automatically.


> ⚠️⚠️⚠️ **in next update will be required [ratatui](https://github.com/ratatui/ratatui)**

## Current version — v0.4

- Command-driven REPL with a small set of slash-commands
- Cryptographically-random password generation with a configurable length
- Persistent vault: entries are auto-loaded on startup and written to `save.txt`
- List every entry or filter by service name and login
- Passwords are masked (`****`) by default; reveal them with `--show`
- Copy a password straight to the system clipboard (`/copy`)
- Remove entries by service name (and optional login)
- Edit an entry in place — replace its service, login and password (`/edit`)
- Dump the whole vault to a timestamped backup file (`/dump`)

### Commands

| Command     | Aliases              | Arguments                       | Description                                           |
|-------------|----------------------|---------------------------------|-------------------------------------------------------|
| `/generate` | `/gen`, `/g`         | `<service> <login> <length>`    | Generate a random password of `<length>` and store it |
| `/add`      | `/a`                 | `<service> <login> <password>`  | Store an existing password                            |
| `/copy`     | `/cp`                | `<service> <login>`             | Copy a stored password to the system clipboard        |
| `/fastgen`  | `/fg`                | `<length>`                      | Generate a one-off password (not saved)               |
| `/list`     | `/l`                 | `[service] [login]` `[--show]`  | List all entries, or filter by `<service>` (and `<login>`) |
| `/remove`   | `/rm`, `/r`          | `<service> [login]`             | Remove entries for `<service>`, optionally narrowed by `<login>` |
| `/edit`     | `/e`                 | `<service> <login> <new-service> <new-login> <new-password>` | Replace a matching entry with new values |
| `/save`     | `/s`                 | —                               | Rewrite the vault file from memory                    |
| `/dump`     | `/d`                 | —                               | Export the whole vault to a timestamped backup file   |
| `/clear`    | `/c`                 | —                               | Clear the screen                                      |
| `/help`     | `/h`                 | —                               | Show available commands                               |
| `/quit`     | `/q`, `/exit`        | —                               | Exit                                                  |

### Flags

| Flag       | Aliases | Applies to | Effect                                            |
|------------|---------|------------|---------------------------------------------------|
| `--show`   | `--s`   | `/list`    | Reveal real passwords instead of masking as `****` |

### Storage format

Entries live in `save.txt` in the working directory, one per line, space-separated:

```
<service> <login> <password>
github.com ra1zzzengpt 1874974343jf1723nfj1jhsad1knc
```

Entries from `/add` and `/generate` are appended immediately. `/save`, `/remove`,
and `/edit` rewrite the whole file from the current in-memory state. `/dump` writes
a separate timestamped copy (`YYYY-MM-DD=HH:MM:SS-PWDM-DUMP.txt`) and leaves
`save.txt` untouched.

## Installation

Requires [Rust](https://www.rust-lang.org/tools/install) (stable, edition 2024).

```bash
git clone https://github.com/ra1zzzengpt/password-manager.git
cd password-manager
cargo run
```

## Roadmap

- [x] **v0.2 — File storage:** persistent local vault (`save.txt`)
- [x] **v0.2 — Remove:** delete entries by service name (`/remove`)
- [x] **v0.3 — Filtering & lookup:** filter by service and login, masked output with `--show`
- [x] **v0.3 — Clipboard:** copy a password without printing it (`/copy`)
- [x] **v0.4 — Edit:** modify existing entries in place (`/edit`)
- [x] **v0.4 — Dump:** export the vault to a timestamped backup file (`/dump`)
- [ ] **v0.5 — Encryption:** protect the vault with a master password

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md).

## License

MIT — see [LICENSE.md](LICENSE.md).
