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

    [[nodiscard]] std::expected<Config, err::Error> load();

    [[nodiscard]] std::expected<void, err::Error> save() const;

    [[nodiscard]] std::expected<void, err::Error> setConfig(Config config);
    [[nodiscard]] std::expected<void, err::Error> setTheme(std::string theme);
    [[nodiscard]] std::expected<void, err::Error> to_default();
    [[nodiscard]] std::expected<void, err::Error> deleteConfig() const;

private:
    static std::filesystem::path configPath();

    Config config_{};
};
