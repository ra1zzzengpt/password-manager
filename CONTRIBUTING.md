# Contributing to Password Manager

Thank you for taking the time to improve Password Manager. This guide covers the development environment, architecture, security constraints, testing, and pull-request workflow.

For end-user installation and usage, see [README.md](README.md). For a detailed introduction to the GitHub Actions workflow, see [YML.md](YML.md).

## Technology stack

| Area | Technology |
|---|---|
| Language | C++23 |
| Build system | CMake 3.20+ |
| Desktop UI | Qt 6 Core and Widgets |
| Cryptography | libsodium |
| Serialization | nlohmann/json |
| Error handling | `std::expected` and domain error types |
| CI | GitHub Actions, Ninja, and ccache |

libsodium and nlohmann/json are pinned in the root `CMakeLists.txt` and downloaded with CMake `FetchContent`.

## Development environment

### Linux dependencies

Ubuntu or Debian:

```bash
sudo apt update
sudo apt install build-essential cmake git ninja-build qt6-base-dev
```

You need a compiler and standard library that implement the C++23 features used by the project, particularly `std::expected`, `std::format`, and chrono time-zone support.

### Clone and build

```bash
git clone https://github.com/ra1zzzengpt/password-manager.git
cd password-manager

cmake -S . -B build-debug \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DSODIUM_DISABLE_TESTS=ON

cmake --build build-debug --target password-manager --parallel
./build-debug/client/password-manager
```

`SODIUM_DISABLE_TESTS=ON` disables the upstream libsodium test targets. It does not disable or alter Password Manager code.

## Repository layout

```text
.
├── CMakeLists.txt                 dependencies and root configuration
├── client/
│   ├── CMakeLists.txt             pwd-core and password-manager targets
│   ├── assets/
│   │   ├── logs/                  local diagnostic log
│   │   └── save/                  local encrypted vault
│   └── src/
│       ├── constants/             paths and generation alphabets
│       ├── controllers/           application and storage operations
│       ├── crypto/                libsodium wrapper and vault container
│       ├── domain/                Service and domain error types
│       ├── generate/              password generation and strength estimate
│       ├── logs/                  internal file logger
│       ├── ui/                    Qt screens and navigation
│       └── utils/                 small shared helpers
├── .github/workflows/ci.yml       automated C++ build
├── README.md                      user documentation
├── CONTRIBUTING.md                developer documentation
├── YML.md                         GitHub Actions guide
└── src/                           previous Rust CLI implementation
```

The active desktop application lives under `client/`. The Rust files at the repository root belong to the previous CLI version and are not part of the CMake build.

## CMake targets

### `pwd-core`

A static library containing controllers, cryptography, domain models, generation, logging, and Qt screens.

### `password-manager`

The executable target. Its `main.cpp` creates `QApplication`, the logger, `MainController`, and `Runner`, then starts the Qt event loop.

When adding a new `.cpp` or header file, include it in `add_library(pwd-core STATIC ...)` in `client/CMakeLists.txt`.

## Architecture

### Startup object graph

```text
main
  ├── QApplication
  ├── Logs
  ├── MainController
  │   └── StorageController
  │       └── crypto::Sodium
  └── Runner
      └── QStackedWidget
          ├── PWScreen
          ├── MainScreen
          └── SettingsScreen
```

`QApplication` is created before the application components. Initialization failures can therefore be reported through a native `QMessageBox`. `main` handles `err::Error`, standard exceptions, and unknown exceptions, and returns a non-zero exit code after a fatal failure.

### Unlock flow

```text
PWScreen
  → MainController::setMasterPassword
  → MainController::loadStorage
  → StorageController::load
  → crypto::import
  → Sodium::decrypt
  → JSON → std::vector<Service>
  → MainScreen::refresh
```

The UI displays domain errors but does not perform filesystem or cryptographic operations directly.

### Save flow

When a credential is added, edited, or removed, `StorageController`:

1. copies the current in-memory vector;
2. applies the requested change;
3. serializes the full vector to JSON;
4. encrypts the JSON payload;
5. writes it to a temporary file;
6. replaces the main vault file;
7. restores the previous vector if saving fails.

Keep persistence behavior centralized in `StorageController` so that every mutation follows the same encryption and rollback path.

## Cryptography and vault format

The encryption key is derived from the master password and a random salt through libsodium `crypto_pwhash` with Argon2id. The serialized vault is encrypted and authenticated with `crypto_secretbox`.

The current binary layout is:

```text
nonce || salt || ciphertext-with-MAC
```

Nonce, salt, and MAC sizes come from libsodium constants. The format does not currently include magic bytes, a version number, or stored KDF parameters. Changing the order or algorithm without a migration path will make existing vaults unreadable.

### Rules for cryptographic changes

- Never implement custom encryption algorithms.
- Never use a general-purpose PRNG for keys, salts, nonces, or generated passwords.
- Check every meaningful libsodium return code.
- Never include plaintext or cryptographic material in errors.
- Clear secret buffers once they are no longer needed.
- Introduce format versioning and migration before changing the persisted layout.
- Add tests for valid passwords, invalid passwords, damaged vaults, and truncated vaults.
- Treat compatibility with existing vault files as part of the public behavior.

## Logging policy

`Logs` is created in `main` and injected into internal components. UI classes must not emit diagnostic log entries directly.

Only fixed, technical events should be logged:

```cpp
logs_.info_log("Starting encrypted storage save");
logs_.warning_log("Storage payload authentication failed");
```

Never log:

- a master password or stored password;
- a login or service name;
- a `Service` object;
- serialized or decrypted JSON;
- decrypted plaintext;
- clipboard contents;
- `err::Error::message` or `exception::what()` without a security review;
- lengths or other metadata unless they are genuinely required for diagnosis.

Bad:

```cpp
logs_.error_log("Parse failed: " + decrypted_json);
```

Good:

```cpp
logs_.error_log("Decrypted storage deserialization failed");
```

The log file is recreated for every application run and is intended to describe the current session only.

## Error handling

Expected failures are returned as `std::expected<T, err::Error>`. The error category is held in the `std::variant` declared in `domain/error/error.hpp`.

Use exceptions only when an object cannot be initialized or the application cannot safely continue. Fatal exceptions are handled in `main` and displayed in a critical message box.

When adding an error category:

1. add an enum or enum value in `domain/error/error_type.hpp`;
2. add the category to the `err::Error` variant if necessary;
3. return `std::unexpected{err::Error{...}}` at the failure site;
4. ensure that `message` contains no credential or decrypted storage data;
5. show only actionable information to the user.

Do not use exceptions for ordinary validation failures that the caller can handle.

## UI development

The Qt screens live in `client/src/ui` and are hosted by a `QStackedWidget` in `Runner`:

| Screen | Responsibility |
|---|---|
| `PWScreen` | Master-password entry and vault deletion |
| `MainScreen` | Credential list, editing, deletion, clipboard, and generation |
| `SettingsScreen` | Master-password change |

Screens communicate through Qt signals and slots and call the public `MainController` API. Do not move filesystem access or cryptographic work into UI callbacks.

Give every `QObject` or `QWidget` an appropriate parent whenever possible so that Qt can manage its lifetime. Keep signal captures narrow, and be especially careful when a callback rebuilds or deletes widgets.

## Adding a feature

A typical implementation order is:

1. add or update the domain model;
2. implement the operation in the appropriate internal component;
3. return recoverable failures with `std::expected`;
4. expose the operation through `MainController`;
5. connect the UI;
6. add fixed, privacy-safe internal log events;
7. add automated tests;
8. update user documentation if behavior changes;
9. update this guide if architecture or contributor workflow changes.

Avoid bypassing `MainController` from the UI. It is the boundary between presentation and internal application behavior.

## Testing

Run all tests registered with CTest:

```bash
ctest --test-dir build-debug --output-on-failure
```

The application does not have its own automated tests yet. When introducing tests, enable CTest in the root CMake file and add a dedicated test target.

Priority test cases:

- `port`/`import` round trip;
- encrypt/decrypt round trip;
- incorrect master password;
- retry after an incorrect master password;
- corrupted and truncated vault data;
- storage save, load, and rollback;
- invalid credential indices;
- password-generator boundary lengths;
- master-password change and persistence;
- log messages containing no user-provided values.

Tests must use synthetic credentials. Never copy a real vault or real account data into fixtures.

## Local checks before committing

Run the same major stages as CI:

```bash
cmake -S . -B build-check \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DSODIUM_DISABLE_TESTS=ON

cmake --build build-check --target password-manager --parallel 2
ctest --test-dir build-check --output-on-failure
git diff --check
```

Do not commit:

- `build/`, `cmake-build-*`, or other build artifacts;
- your local encrypted vault;
- diagnostic logs;
- IDE settings;
- temporary files;
- real credentials in tests, screenshots, documentation, issues, or pull requests.

The repository `.gitignore` protects the standard local vault and log paths, but always inspect `git status` before committing.

## Continuous integration

GitHub Actions runs after every push and for pull requests targeting `main`. The workflow:

1. creates an Ubuntu runner;
2. checks out the repository;
3. installs Qt 6, Ninja, and ccache;
4. restores the compilation cache;
5. configures a Release build;
6. builds the `password-manager` target;
7. runs CTest.

The workflow is defined in `.github/workflows/ci.yml`. See [YML.md](YML.md) for a line-by-line explanation and examples for extending it.

## Git workflow

Create branches from an up-to-date `main`:

| Prefix | Purpose |
|---|---|
| `feature/<name>` | New user-facing behavior |
| `fix/<name>` | Bug fixes |
| `docs/<name>` | Documentation only |
| `refactor/<name>` | Structural changes without new behavior |

Keep each commit focused on one logical change. Write messages that explain the result rather than the editing process.

A pull request should include:

- what changed;
- why the change is needed;
- how reviewers can verify it;
- whether the vault format changes;
- whether security or secret handling is affected;
- screenshots for visible UI changes;
- tests added or a reason tests were not added.

Do not merge a pull request while required CI checks are failing.

## Reporting bugs

Include:

- operating system and version;
- compiler, CMake, and Qt versions;
- exact reproduction steps;
- expected and actual behavior;
- a minimal, privacy-reviewed log excerpt if useful.

Before posting any log or screenshot publicly, inspect it manually for credentials, filesystem usernames, or other private information. Never attach the encrypted vault itself.

## Security-related reports

If a report describes a potential credential disclosure, authentication bypass, vault corruption, or cryptographic weakness, avoid publishing real secrets or a real vault as evidence. Use synthetic test data and clearly describe the affected code path and reproduction conditions.

Security-sensitive changes should be small, independently reviewable, and accompanied by tests whenever possible.
