#include "storage_controller.hpp"

#include <nlohmann/json.hpp>
#include <filesystem>
#include <constants/paths.hpp>
#include <expected>
#include <domain/error/error.hpp>
#include <fstream>

#include <QDebug>

std::expected<void, err::Error> StorageController::save()
{
    const std::filesystem::path path{cnt::save};
    const std::filesystem::path temp_path{path.string() + ".temp"}; // todo lille rework for SAVING file will be temp
    if (!std::filesystem::exists(path))
    {
        std::error_code error_code;
        std::filesystem::create_directories(path.parent_path(), error_code);
        if (error_code)
        {
            return std::unexpected{
                err::Error{err::StorageError::CreateDirectoryFailed, "Can't create directory at: " + path.string()}
            };
        }
    }

    std::ofstream file{temp_path, std::ios::binary};
    if (!file.is_open())
    {
        std::filesystem::remove(temp_path);
        return std::unexpected{err::Error{err::StorageError::OpenFileFailed, "Can't open file at: " + path.string()}};
    }
    const std::expected<crypto::SodiumInfo, err::Error> encrypted_result = sodium_.encrypt(
        nlohmann::json(services_).dump());
    if (!encrypted_result.has_value())
    {
        std::filesystem::remove(temp_path);
        return std::unexpected{encrypted_result.error()};
    }
    std::vector<uint8_t> write_ready = crypto::port(encrypted_result.value());
    file.write(reinterpret_cast<const std::ostream::char_type *>(write_ready.data()),
               static_cast<long>(write_ready.size()));
    if (file.bad() || file.fail())
    {
        std::filesystem::remove(temp_path);
        return std::unexpected{err::Error{err::StorageError::FileStreamError, "Error with file stream: " + path.string()}};
    }
    std::filesystem::rename(temp_path, path);
    return {};
}

std::expected<void, err::Error> StorageController::load()
{
    const std::filesystem::path path{cnt::save};
    if (!std::filesystem::exists(path))
    {
        std::error_code error_code;
        std::filesystem::create_directories(path.parent_path(), error_code);
        if (error_code)
        {
            return std::unexpected{err::Error{err::StorageError::CreateDirectoryFailed, "Can't create directory at: " + path.string()}};
        }
    }

    std::ifstream file{path, std::ios::binary};

    if (file.is_open() && file.peek() != std::ifstream::traits_type::eof())
    {
        file.seekg(0, std::ios::end);
        std::streamsize file_size{file.tellg()};
        file.seekg(0, std::ios::beg);

        std::vector<uint8_t> data(file_size);
        file.read(reinterpret_cast<std::istream::char_type *>(data.data()), file_size);
        const std::expected<crypto::SodiumInfo, err::Error> import_result = crypto::import(data);
        if (!import_result.has_value())
        {
            return std::unexpected{import_result.error()};
        }

        const std::expected<std::string,err::Error> decrypt_result = sodium_.decrypt(import_result.value());

        if (!decrypt_result.has_value())
        {
            return std::unexpected{decrypt_result.error()};
        }
        try
        {
            services_ = nlohmann::json::parse(decrypt_result.value());
        } catch (...)
        {
            return std::unexpected{err::Error{err::StorageError::ParseFailed, "Can't deserialize service this: " + decrypt_result.value()}};
        }
    } else
    {
        services_ = std::vector<Service>{};
    }
    return save();
}

std::expected<void, err::Error> StorageController::del()
{
    const std::filesystem::path path{cnt::save};
    if (!std::filesystem::exists(path))
    {
        return {};
    }
    try
    {
        std::filesystem::remove(path);
    } catch (const std::filesystem::filesystem_error& e)
    {
        return std::unexpected{err::Error{err::StorageError::DeleteFailed,e.what()}};
    }
    return {};
}

const std::vector<Service>& StorageController::services()
{
    return services_;
}

std::expected<void, err::Error> StorageController::addService(const Service &service)
{
    const std::vector old_services{services_};
    services_.push_back(service);
    if (const std::expected<void,err::Error> save_result = save(); !save_result.has_value())
    {
        services_ = old_services;
        return std::unexpected{save_result.error()};
    }
    return {};
}

std::expected<void,err::Error> StorageController::setMasterPassword(const std::string &password)
{
    return sodium_.setMasterPassword(password);
}
