# Contributing

## Getting started

```bash
git clone https://github.com/ra1zzzengpt/password-manager.git
cd password-manager
cargo build
cargo run
```

Requires Rust stable. Check your version: `rustc --version`.

## Before submitting

- Run `cargo fmt` — code must be formatted
- Run `cargo clippy` — no warnings allowed
- Run `cargo build` — must compile cleanly

## Pull requests

- Branch off `main`, name it `feature/<what>` or `fix/<what>`
- Keep commits focused — one logical change per commit
- Describe what changed and why in the PR description

## Reporting issues

Open an issue with:
- What you did
- What you expected
- What actually happened
- Output / error message if any
