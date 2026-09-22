<div align="center">

# Password Manager

**Local-first password vault built with C++23, Qt 6, and libsodium.**

[![C++ CI](https://github.com/ra1zzzengpt/password-manager/actions/workflows/ci.yml/badge.svg)](https://github.com/ra1zzzengpt/password-manager/actions/workflows/ci.yml)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-00599C?logo=cplusplus)](https://en.cppreference.com/w/cpp/23)
[![Qt 6](https://img.shields.io/badge/Qt-6-41CD52?logo=qt&logoColor=white)](https://www.qt.io/)
[![CMake](https://img.shields.io/badge/CMake-3.20%2B-064F8C?logo=cmake)](https://cmake.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE.md)

No account, no cloud service, and no network access at runtime. Your credentials stay in one encrypted vault on your device.

</div>

> [!WARNING]
> This project is under active development and has not received an independent security audit. Keep a separate backup of important credentials and do not rely on Password Manager as their only copy.

![Password Manager main screen](img/image1.png)

## Features

- encrypted local credential storage;
- add, edit, and remove credential entries;
- copy logins and passwords to the system clipboard;
- password generation with Low, Medium, and High presets;
- password-strength estimation;
- master-password changes with vault re-encryption;
- CSV import and export;
- Dark, White, and Nord themes;
- configurable interface sound effects and volume;
- privacy-conscious per-session diagnostic logging;
- an isolated experimental Android port.

## Security model

The desktop client serializes the vault to JSON in memory and protects it with libsodium:

- the encryption key is derived from the master password using Argon2id (`crypto_pwhash`);
- vault contents are encrypted and authenticated with `crypto_secretbox`;
- each new vault receives a random salt, and every save receives a random nonce;
- the persisted binary layout is `nonce || salt || ciphertext-with-MAC`;
- the key, salt, and in-memory master-password buffer are cleared when the crypto component is destroyed.

The master password must contain at least 8 characters. It is never stored, and there is no password-recovery mechanism.

> [!CAUTION]
> The **Delete** button on the unlock screen permanently removes the encrypted vault. Use it only when you no longer need the stored credentials or have a verified backup.

## Requirements

| Component | Requirement |
|---|---|
| Language | C++23 |
| Build system | CMake 3.20 or newer |
| UI | Qt 6 Core, Widgets, Multimedia, and MultimediaWidgets |
| Cryptography | libsodium, downloaded automatically by CMake |
| Serialization | nlohmann/json, downloaded automatically by CMake |
| Compiler | C++23 support for `std::expected`, `std::format`, ranges, and chrono time zones |
| Network | Needed during the first CMake configuration to fetch pinned dependencies |

GitHub Actions currently builds the desktop client on Linux, Windows, and macOS. Prebuilt releases and installers are not provided yet.

## Build the desktop client

### Linux dependencies

Ubuntu or Debian:

```bash
sudo apt update
sudo apt install build-essential cmake git ninja-build qt6-base-dev qt6-multimedia-dev
```

Package names vary between distributions. A recent GCC or Clang toolchain may be required for the C++23 library features used by the project.

### Configure and compile

```bash
git clone https://github.com/ra1zzzengpt/password-manager.git
cd password-manager

cmake -S . -B build \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DSODIUM_DISABLE_TESTS=ON

cmake --build build --target password-manager --parallel
```

`SODIUM_DISABLE_TESTS=ON` skips the upstream libsodium test targets and does not disable any Password Manager functionality.

### Run

From the repository root:

```bash
./build/client/password-manager
```

The application searches the current directory and up to three parent directories for `client/assets` or `assets`. Running it from the repository root or its CMake build directory keeps resource discovery predictable.

## First launch
q
<div align="center">
  <img src="img/image.png" alt="Master-password screen" width="420">
</div>

1. Enter a master password containing at least 8 characters.
2. Select **Next**.
3. If no vault exists, the application creates a new encrypted vault.

On later launches, enter the same password to decrypt the existing vault. An incorrect password cannot open it.

## Using the vault

### Add a credential

Select **Add**, enter the service name, login, and password, then select **Add Service**. The encrypted vault is saved immediately.

Enable **Generate** to create a password instead. The available presets are Low, Medium, and High, and generated passwords can be between 8 and 500 characters; the High preset requires at least 16 characters.

### Manage credentials

Each credential card shows the service, login, and creation time. From the card you can:

- copy the login;
- copy the password;
- open details;
- edit or delete the credential;
- inspect the password-strength indicator.

> [!NOTE]
> Copied values remain in the system clipboard until another application replaces them. Password Manager does not currently clear the clipboard automatically.

### Settings

Open the settings dialog to:

- change the master password;
- choose the Dark, White, or Nord theme;
- enable or disable interface sounds for the current session;
- set sound volume from 0 to 100.

Theme and volume are saved in `client/assets/config.conf`. The sound-enabled toggle itself is not currently persisted between launches.

### CSV import and export

The toolbar contains import and export actions. Import currently expects a header row and reads the first three comma-separated columns as:

```csv
url,login,password
https://example.com,user@example.com,secret
```

The `https://` prefix is removed from imported service names. Empty records are skipped. Export writes to `client/assets/export/export.csv`.

> [!CAUTION]
> CSV files are plaintext and expose every exported login and password. Store them securely and delete them when they are no longer needed.

> [!NOTE]
> CSV support is experimental. Quoted commas, embedded newlines, and a lossless export/import round trip are not supported reliably yet.

## Local files

Paths below are relative to the repository when the desktop application is run from the recommended location.

| Path | Purpose | Protected? |
|---|---|:---:|
| `client/assets/save/save.save` | Encrypted credential vault | Yes |
| `client/assets/config.conf` | Theme and volume preferences | No |
| `client/assets/logs/pwd-session.log` | Diagnostic events from the current session | No |
| `client/assets/export/export.csv` | Optional CSV export containing credentials | **No** |

The log is recreated for each run and is designed not to include passwords, logins, service names, decrypted JSON, or clipboard contents. Back up the encrypted vault only while the application is closed, and never publish vault or CSV files.

## Android port

An experimental, isolated Qt/C++ Android port lives in [`android-port/`](android-port/README.md). It uses Android's private application-data directory, requests neither network nor external-storage permissions, and excludes the vault from Android backup.

The port requires Qt 6.8 or newer with an Android `arm64-v8a` kit. It has not yet completed a real-device validation pass, so the desktop client remains the primary supported implementation. See the [Android build guide](android-port/README.md) for setup and packaging instructions.

## Project status and limitations

- no prebuilt desktop packages or installers;
- no synchronization, cloud storage, or account recovery;
- no automatic vault backups;
- no automatic clipboard clearing;
- no versioned vault format or migration system;
- CSV support is incomplete and exports plaintext secrets;
- no project-specific automated test suite yet;
- no independent security audit;
- Android packaging and device behavior still require validation.

## Repository layout

```text
.
├── client/                  desktop Qt client and bundled assets
│   ├── src/
│   │   ├── controllers/    application and persistence operations
│   │   ├── crypto/         libsodium wrapper and vault container
│   │   ├── domain/         credential, configuration, and error models
│   │   ├── generate/       password generation and strength estimation
│   │   ├── logs/           session logger
│   │   ├── ui/             Qt widgets and dialogs
│   │   └── utils/          CSV parser and shared helpers
│   └── assets/             themes, icons, sounds, and local runtime data
├── android-port/           separate experimental Android application
├── img/                    documentation screenshots
├── .github/workflows/      cross-platform CI configuration
├── CMakeLists.txt          desktop project entry point
└── CONTRIBUTING.md         contributor documentation
```

## Contributing

Architecture notes, security rules, local checks, and the development workflow are documented in [CONTRIBUTING.md](CONTRIBUTING.md).

To run all tests registered with CMake:

```bash
ctest --test-dir build --build-config Release --output-on-failure
```

The project does not currently register its own automated tests; this command primarily provides a stable workflow for future test targets.

## License

Distributed under the MIT License. See [LICENSE.md](LICENSE.md).
