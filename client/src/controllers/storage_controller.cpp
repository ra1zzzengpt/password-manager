#include "storage_controller.hpp"

#include <nlohmann/json.hpp>
#include <filesystem>
#include <constants/paths.hpp>
#include <expected>
#include <domain/error/error.hpp>
#include <fstream>
#include <logs/logs.hpp>

StorageController::StorageController(Logs& logs) : sodium_(logs), logs_(logs)
{
    logs_.info_log("Storage controller initialized");
}

std::expected<void, err::Error> StorageController::save()
{
    logs_.info_log("Starting encrypted storage save");
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
    const std::expected<crypto::SodiumInfo, err::Error> encrypted_result = sodium_.encrypt(
        nlohmann::json(services_).dump());
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
            services_ = nlohmann::json::parse(decrypt_result.value());
        } catch (...)
        {
            logs_.error_log("Decrypted storage deserialization failed");
            return std::unexpected{err::Error{err::StorageError::ParseFailed, "Can't deserialize storage data."}};
        }
    } else
    {
        logs_.info_log("Storage file is missing or empty; initializing new storage");
        services_ = std::vector<Service>{};
    }
    return save();
}

std::expected<void, err::Error> StorageController::del()
{
    logs_.warning_log("Encrypted storage deletion requested");
    const std::filesystem::path path{cnt::savePath()};
    if (!std::filesystem::exists(path))
    {
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
    logs_.info_log("Encrypted storage deleted");
    return {};
}

const std::vector<Service>& StorageController::services()
{
    return services_;
}

std::expected<void, err::Error> StorageController::addService(const Service &service)
{
    logs_.info_log("Adding credential record");
    const std::vector old_services{services_};
    services_.push_back(service);
    if (const std::expected<void,err::Error> save_result = save(); !save_result.has_value())
    {
        services_ = old_services;
        logs_.error_log("Credential record addition rolled back");
        return std::unexpected{save_result.error()};
    }
    logs_.info_log("Credential record added");
    return {};
}

std::expected<void,err::Error> StorageController::setMasterPassword(const std::string &password)
{
    logs_.info_log("Updating master password");
    return sodium_.setMasterPassword(password);
}

std::expected<void, err::Error> StorageController::changeMasterPassword(const std::string& old_password, const std::string &password)
{
    if (sodium_.getMasterPassword() != old_password)
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
    const std::vector old_services{services_};
    services_.erase(services_.begin() + static_cast<long>(index));
    if (const std::expected<void,err::Error> save_result = save(); !save_result.has_value())
    {
        services_ = old_services;
        logs_.error_log("Credential record removal rolled back");
        return std::unexpected{save_result.error()};
    }
    logs_.info_log("Credential record removed");
    return {};
}

std::expected<void, err::Error> StorageController::rewriteService(const std::string& name, const std::string& login, const std::string& password, const std::size_t& index)
{
    logs_.info_log("Updating credential record");
    const std::vector old_services{services_};
    services_[index].name = name;
    services_[index].login = login;
    services_[index].password = password;
    if (const std::expected<void,err::Error> save_result = save(); !save_result.has_value())
    {
        services_ = old_services;
        logs_.error_log("Credential record update rolled back");
        return std::unexpected{save_result.error()};
    }
    logs_.info_log("Credential record updated");
    return {};
}
