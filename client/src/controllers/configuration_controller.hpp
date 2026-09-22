#pragma once

#include <expected>
#include <filesystem>
#include <string>

#include <domain/config.hpp>
#include <domain/error/error.hpp>

class ConfigurationController final
{
public:
    [[nodiscard]] const Config& config() const noexcept;

    // A missing file is initialized with defaults; invalid or unreadable files are errors.
    [[nodiscard]] std::expected<Config, err::Error> load();

    // Write to a sibling file first so a failed write does not truncate the old config.
    [[nodiscard]] std::expected<void, err::Error> save() const;

    // On failure, keep the previous in-memory configuration.
    [[nodiscard]] std::expected<void, err::Error> setConfig(Config config);
    [[nodiscard]] std::expected<void, err::Error> setVolume(std::uint32_t volume);
    [[nodiscard]] std::expected<void, err::Error> setTheme(std::string theme);
    [[nodiscard]] std::expected<void, err::Error> to_default();

private:
    static std::filesystem::path configPath();

    Config config_{};
};
