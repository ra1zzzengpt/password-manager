#include "configuration_controller.hpp"

#include <exception>
#include <fstream>
#include <system_error>
#include <utility>

#include <constants/paths.hpp>

const Config& ConfigurationController::config() const noexcept
{
    return config_;
}

std::expected<Config, err::Error> ConfigurationController::load()
{
    try
    {
        const auto path = configPath();
        std::error_code ec;
        const bool exists = std::filesystem::exists(path, ec);
        if (ec)
        {
            return std::unexpected{err::Error{err::StorageError::OpenFileFailed, ec.message()}};
        }
        if (!exists)
        {
            return std::unexpected{err::Error{err::StorageError::OpenFileFailed,
                "Configuration file does not exist: " + path.string()}};
        }

        std::ifstream file(path);
        if (!file)
        {
            return std::unexpected{err::Error{err::StorageError::OpenFileFailed,
                "Can't open configuration at: " + path.string()}};
        }
        const auto parsed = nlohmann::json::parse(file);
        if (file.bad())
        {
            return std::unexpected{err::Error{err::StorageError::FileStreamError,
                "Can't read configuration at: " + path.string()}};
        }
        Config loaded = parsed.get<Config>();
        config_ = std::move(loaded);
        return config_;
    }
    catch (const err::Error& e)
    {
        return std::unexpected{e};
    }
    catch (const nlohmann::json::exception& e)
    {
        return std::unexpected{err::Error{err::StorageError::ParseFailed, e.what()}};
    }
    catch (const std::invalid_argument& e)
    {
        return std::unexpected{err::Error{err::StorageError::ParseFailed, e.what()}};
    }
    catch (const std::exception& e)
    {
        return std::unexpected{err::Error{err::StorageError::FileStreamError, e.what()}};
    }
}

std::expected<void, err::Error> ConfigurationController::save() const
{
    try
    {
        const auto path = configPath();
        const std::filesystem::path temporary{path.string() + ".tmp"};
        {
            std::ofstream file(temporary, std::ios::trunc);
            if (!file)
            {
                return std::unexpected{err::Error{err::StorageError::OpenFileFailed,
                    "Can't open configuration at: " + temporary.string()}};
            }
            file << nlohmann::json(config_).dump(4) << '\n';
            file.close();
            if (!file)
            {
                std::error_code ignored;
                std::filesystem::remove(temporary, ignored);
                return std::unexpected{err::Error{err::StorageError::FileStreamError,
                    "Can't write configuration at: " + temporary.string()}};
            }
        }
        std::error_code ec;
        std::filesystem::rename(temporary, path, ec);
        if (ec)
        {
            std::error_code ignored;
            std::filesystem::remove(temporary, ignored);
            return std::unexpected{err::Error{err::StorageError::RenameFailed, ec.message()}};
        }
        return {};
    }
    catch (const err::Error& e)
    {
        return std::unexpected{e};
    }
    catch (const std::exception& e)
    {
        return std::unexpected{err::Error{err::StorageError::FileStreamError, e.what()}};
    }
}

std::expected<void, err::Error> ConfigurationController::setConfig(Config config)
{
    auto previous = config_;
    try
    {
        config_ = std::move(nlohmann::json(config).get<Config>());
    }
    catch (const std::exception& e)
    {
        return std::unexpected{err::Error{err::StorageError::ParseFailed, e.what()}};
    }
    if (auto result = save(); !result)
    {
        config_ = std::move(previous);
        return result;
    }
    return {};
}

std::expected<void, err::Error> ConfigurationController::setTheme(std::string theme)
{
    auto updated = config_;
    updated.theme = std::move(theme);
    return setConfig(std::move(updated));
}

std::expected<void, err::Error> ConfigurationController::to_default()
{
    return setConfig(Config{});
}

std::expected<void, err::Error> ConfigurationController::deleteConfig() const
{
    try
    {
        std::error_code ec;
        std::filesystem::remove(configPath(), ec);
        if (ec)
        {
            return std::unexpected{err::Error{err::StorageError::DeleteFailed, ec.message()}};
        }
        return {};
    }
    catch (const std::exception& e)
    {
        return std::unexpected{err::Error{err::StorageError::DeleteFailed, e.what()}};
    }
}

std::filesystem::path ConfigurationController::configPath()
{
    return cnt::getAssetsBasePath() / "config.conf";
}
