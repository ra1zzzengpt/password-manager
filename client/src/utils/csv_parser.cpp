//
// Created by arch on 9/22/26.
//

#include "csv_parser.hpp"
#include <filesystem>
#include <fstream>
#include <chrono>
#include <format>
#include <iostream>

std::expected<std::vector<Service>, err::Error> CSVParser::parseCSV(std::string file_path)
{
    std::filesystem::path filepath(file_path);
    if (!std::filesystem::exists(filepath)) {
        return std::unexpected{err::Error{.type = err::StorageError::OpenFileFailed, .message = "file not found."}};
    }
    std::ifstream file(filepath);
    std::vector<Service> services;
    if (file.is_open()) {
        std::string line;
        bool first_line{true};
        while (std::getline(file, line))
        {
            if (line.empty() || first_line)
            {
                first_line = false;
                continue;
            }

            std::erase(line,'"');

            Service service{};
            std::stringstream ss(line);

            std::getline(ss, service.name, ',');
            std::getline(ss, service.login, ',');
            std::getline(ss, service.password, ',');

            if (service.name.contains("https://")) {
                service.name.erase(0,8);
            }

            if ((service.name == "url" && service.login == "login" && service.password == "password") || service.name.empty() || service.login.empty() || service.password.empty()) {
                continue;
            }

            service.created_at = std::format("{:%Y-%m-%d %H:%M:%S}", std::chrono::zoned_time{std::chrono::current_zone(),std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now())});
            services.push_back(std::move(service));
        }
        return services;
    }
    return std::unexpected{err::Error{.type = err::StorageError::OpenFileFailed, .message = "file not found."}};
}

std::expected<void, err::Error> CSVParser::exportCSV(std::string file_path, const std::unordered_map<std::uint32_t,Service>& services) {
    std::filesystem::path filepath(file_path);
    if (!std::filesystem::exists(filepath)) {
        std::error_code ec;
        std::filesystem::create_directories(filepath.parent_path(), ec);
        if (ec) {
            return std::unexpected{err::Error{.type = err::StorageError::CreateDirectoryFailed, .message = ec.message()}};
        }
    }
    std::ofstream file(file_path);
    if (!file.is_open()) {
        return std::unexpected{err::Error{.type = err::StorageError::OpenFileFailed, .message = "file not found."}};
    }
    for (const Service& service : services | std::views::values) {
        file << '"' << "https://" << service.name << "\",\"" << service.login << "\",\"" << service.password << "\",\"" << service.created_at << ";\n";
    }
    return {};
}
