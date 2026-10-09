#include "storage_controller.hpp"

#include <algorithm>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <constants/paths.hpp>
#include <expected>
#include <domain/error/error.hpp>
#include <fstream>
#include <iostream>
#include <limits>
#include <logs/logs.hpp>
#include <utility>

StorageController::StorageController(Logs& logs, std::atomic<std::uint32_t>& vault_version)
    : sodium_(logs), logs_(logs), vault_version_(vault_version)
{
    logs_.info_log("Storage controller initialized");
}

std::uint32_t StorageController::takeNextId()
{
    id_ = ++id_;
    return id_;
}

std::expected<void, err::Error> StorageController::save()
{
    logs_.info_log("Starting encrypted storage save");
    const std::uint32_t current_vault_version = vault_version_.load();
    if (current_vault_version == std::numeric_limits<std::uint32_t>::max())
    {
        logs_.error_log("Vault version overflow");
        return std::unexpected{err::Error{
            err::StorageError::VaultVersionOverflow,
            "Vault version reached its maximum value",
        }};
    }

    const std::uint32_t next_vault_version = current_vault_version + 1;
    const std::filesystem::path path{cnt::savePath()};
    const std::filesystem::path temp_path{path.string() + ".temp"};
    if (!std::filesystem::exists(path))
    {
        std::error_code error_code;
        std::filesystem::create_directories(path.parent_path(), error_code);
        if (error_code)
        {
            logs_.error_log("Storage directory creation failed");
            return std::unexpected{
                err::Error{err::StorageError::CreateDirectoryFailed, "Can't create directory at: " + path.string()}
            };
        }
    }

    std::ofstream file{temp_path, std::ios::binary};
    if (!file.is_open())
    {
        logs_.error_log("Temporary storage file open failed");
        std::filesystem::remove(temp_path);
        return std::unexpected{err::Error{err::StorageError::OpenFileFailed, "Can't open file at: " + path.string()}};
    }
    const nlohmann::json vault = {
        {"vault_version", next_vault_version},
        {"services", services_},
    };
    const std::expected<crypto::SodiumInfo, err::Error> encrypted_result = sodium_.encrypt(vault.dump());
    if (!encrypted_result.has_value())
    {
        logs_.error_log("Storage save encryption failed");
        std::filesystem::remove(temp_path);
        return std::unexpected{encrypted_result.error()};
    }
    std::vector<uint8_t> write_ready = crypto::port(encrypted_result.value());
    file.write(reinterpret_cast<const std::ostream::char_type *>(write_ready.data()),
               static_cast<long>(write_ready.size()));
    if (file.bad() || file.fail())
    {
        logs_.error_log("Writing encrypted storage failed");
        std::filesystem::remove(temp_path);
        return std::unexpected{err::Error{err::StorageError::FileStreamError, "Error with file stream: " + path.string()}};
    }
    try
    {
        std::filesystem::rename(temp_path, path);
    } catch (std::filesystem::filesystem_error& e)
    {
        logs_.error_log("Replacing storage file failed");
        return std::unexpected{err::Error{err::StorageError::RenameFailed, e.what()}};
    }
    vault_version_ = next_vault_version;
    logs_.info_log("Encrypted storage saved");
    return {};
}

std::expected<void, err::Error> StorageController::load()
{
    logs_.info_log("Starting encrypted storage load");
    const std::filesystem::path path{cnt::savePath()};
    if (!std::filesystem::exists(path))
    {
        std::error_code error_code;
        std::filesystem::create_directories(path.parent_path(), error_code);
        if (error_code)
        {
            logs_.error_log("Storage directory creation failed during load");
            return std::unexpected{err::Error{err::StorageError::CreateDirectoryFailed, "Can't create directory at: " + path.string()}};
        }
    }

    std::ifstream file{path, std::ios::binary};

    if (file.is_open() && file.peek() != std::ifstream::traits_type::eof())
    {
        logs_.info_log("Existing storage file found");
        file.seekg(0, std::ios::end);
        std::streamsize file_size{file.tellg()};
        file.seekg(0, std::ios::beg);

        if (file_size > 50000000)
        {
            return std::unexpected{err::Error{.type = err::StorageError::FileIsTooBig, .message = "file is too big"}};
        }

        std::vector<uint8_t> data(file_size);
        file.read(reinterpret_cast<std::istream::char_type *>(data.data()), file_size);
        const std::expected<crypto::SodiumInfo, err::Error> import_result = crypto::import(data);
        if (!import_result.has_value())
        {
            logs_.warning_log("Encrypted storage container import failed");
            return std::unexpected{import_result.error()};
        }

        const std::expected<std::string,err::Error> decrypt_result = sodium_.decrypt(import_result.value());

        if (!decrypt_result.has_value())
        {
            logs_.warning_log("Encrypted storage decryption failed");
            return std::unexpected{decrypt_result.error()};
        }
        try
        {
            const nlohmann::json vault = nlohmann::json::parse(decrypt_result.value());
            std::uint32_t loaded_vault_version;
            std::unordered_map<std::uint32_t, Service> loaded_services;
            if (vault.is_object() && vault.contains("vault_version") && vault.contains("services"))
            {
                loaded_vault_version = vault.at("vault_version").get<std::uint32_t>();
                loaded_services = vault.at("services").get<std::unordered_map<std::uint32_t, Service>>();
            }
            else
            {
                loaded_vault_version = 0;
                loaded_services = vault.get<std::unordered_map<std::uint32_t, Service>>();
            }

            vault_version_ = loaded_vault_version;
            services_ = std::move(loaded_services);
            id_ = 0;
            for (const auto& service : services_)
                id_ = std::max(id_, service.first);
        } catch (...)
        {
            logs_.error_log("Decrypted storage deserialization failed");
            return std::unexpected{err::Error{err::StorageError::ParseFailed, "Can't deserialize storage data."}};
        }
        return {};
    }
    else
    {
        logs_.info_log("Storage file is missing or empty; initializing new storage");
        services_ = std::unordered_map<std::uint32_t, Service>{};
        id_ = 0;
        vault_version_ = 0;
    }
    return save();
}

std::expected<void, err::Error> StorageController::del()
{
    logs_.warning_log("Encrypted storage deletion requested");
    const std::filesystem::path path{cnt::savePath()};
    if (!std::filesystem::exists(path))
    {
        services_.clear();
        id_ = 0;
        vault_version_ = 0;
        logs_.info_log("Storage deletion skipped because file does not exist");
        return {};
    }
    try
    {
        std::filesystem::remove(path);
    } catch (const std::filesystem::filesystem_error& e)
    {
        logs_.error_log("Storage file deletion failed");
        return std::unexpected{err::Error{err::StorageError::DeleteFailed,e.what()}};
    }
    services_.clear();
    id_ = 0;
    vault_version_ = 0;
    logs_.info_log("Encrypted storage deleted");
    return {};
}

std::expected<std::vector<std::uint8_t>, err::Error> StorageController::encryptedVault() const
{
    std::ifstream file{cnt::savePath(), std::ios::binary};
    if (!file)
        return std::unexpected{err::Error{err::StorageError::OpenFileFailed, "Can't open encrypted vault"}};
    std::vector<std::uint8_t> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return data;
}

std::expected<void, err::Error> StorageController::replaceVault(const std::vector<std::uint8_t>& data)
{
    const auto imported = crypto::import(data);
    if (!imported)
        return std::unexpected{imported.error()};
    const auto decrypted = sodium_.decrypt(imported.value());
    if (!decrypted)
        return std::unexpected{decrypted.error()};

    std::uint32_t loaded_version;
    std::unordered_map<std::uint32_t, Service> loaded_services;
    try
    {
        const auto vault = nlohmann::json::parse(decrypted.value());
        loaded_version = vault.at("vault_version").get<std::uint32_t>();
        loaded_services = vault.at("services").get<std::unordered_map<std::uint32_t, Service>>();
    }
    catch (...)
    {
        return std::unexpected{err::Error{err::StorageError::ParseFailed, "Can't deserialize fetched vault"}};
    }

    std::ofstream file{cnt::savePath(), std::ios::binary | std::ios::trunc};
    if (!file)
        return std::unexpected{err::Error{err::StorageError::OpenFileFailed, "Can't save fetched vault"}};
    file.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
    if (!file)
        return std::unexpected{err::Error{err::StorageError::FileStreamError, "Can't save fetched vault"}};

    services_ = std::move(loaded_services);
    vault_version_ = loaded_version;
    id_ = 0;
    for (const auto& service : services_)
        id_ = std::max(id_, service.first);
    return {};
}

const std::unordered_map<std::uint32_t, Service>& StorageController::services()
{
    return services_;
}

std::expected<std::uint32_t, err::Error> StorageController::addService(const Service &service)
{
    logs_.info_log("Adding credential record");
    const std::unordered_map old_services{services_};
    std::uint32_t id = takeNextId();
    services_.emplace(id,service);
    if (const std::expected<void,err::Error> save_result = save(); !save_result.has_value())
    {
        services_ = old_services;
        logs_.error_log("Credential record addition rolled back");
        return std::unexpected{save_result.error()};
    }
    logs_.info_log("Credential record added");
    return {id};
}

std::expected<void,err::Error> StorageController::setMasterPassword(const std::string &password)
{
    logs_.info_log("Updating master password");
    return sodium_.setMasterPassword(password);
}

std::expected<void, err::Error> StorageController::changeMasterPassword(const std::string& old_password, const std::string &password)
{
    if (const std::expected<void, err::Error> set_res = sodium_.setMasterPassword(old_password); !set_res.has_value())
    {
        logs_.warning_log("Master password update rejected");
        return std::unexpected{err::Error{err::SettingsError::PasswordsNotEqual, "Old password not equal."}};
    }
    if (const std::expected<void, err::Error> set_res = sodium_.setMasterPassword(password); !set_res.has_value())
    {
        logs_.warning_log("New master password validation failed");
        return std::unexpected{set_res.error()};
    }
    const auto save_result = save();
    if (!save_result.has_value())
    {
        logs_.error_log("Master password update could not be persisted");
        return std::unexpected{save_result.error()};
    }
    logs_.info_log("Master password updated");
    return {};
}

std::expected<void, err::Error> StorageController::removeService(const std::size_t &index)
{
    logs_.info_log("Removing credential record");
    const std::unordered_map old_services{services_};
    services_.erase(index);
    if (const std::expected<void,err::Error> save_result = save(); !save_result.has_value())
    {
        services_ = old_services;
        logs_.error_log("Credential record removal rolled back");
        return std::unexpected{save_result.error()};
    }
    logs_.info_log("Credential record removed");
    return {};
}

std::expected<void, err::Error> StorageController::rewriteService(const Service& service, const std::size_t& index)
{
    logs_.info_log("Updating credential record");
    const std::unordered_map old_services{services_};
    services_[index].name = service.name;
    services_[index].login = service.login;
    services_[index].password = service.password;
    if (const std::expected<void,err::Error> save_result = save(); !save_result.has_value())
    {
        services_ = old_services;
        logs_.error_log("Credential record update rolled back");
        return std::unexpected{save_result.error()};
    }
    logs_.info_log("Credential record updated");
    return {};
}

std::expected<void, err::Error> StorageController::importCSV(const std::string& file_path) {
    if (auto res = CSVParser::parseCSV(file_path); !res.has_value()) {
        return std::unexpected{res.error()};
    } else {
        for (const auto& service : res.value()) {
            services_.emplace(takeNextId(), service);
        }
    }
    return save();
}

std::expected<void, err::Error> StorageController::exportCSV() {
    if (auto res = CSVParser::exportCSV((cnt::getAssetsBasePath()/"export"/"export.csv").c_str(),services_); !res.has_value()) {
        return std::unexpected{res.error()};
    }
    return {};
}
