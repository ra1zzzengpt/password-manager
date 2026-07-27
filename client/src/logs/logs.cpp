#include <logs/logs.hpp>
#include <constants/paths.hpp>

#include <fstream>
#include <filesystem>
#include <chrono>
#include <format>

namespace
{
    std::string string_factory(std::string_view lvl, std::string_view message, const std::source_location location)
    {
        return std::format("<{:%Y-%m-%d %H:%M:%S}> [{}]: {} - {} : {} in {}\n",
                                         std::chrono::zoned_time{
                                             std::chrono::current_zone(), std::chrono::system_clock::now()
                                         }, lvl, message, location.file_name(), location.line(), location.function_name());
    }
}

Logs::Logs() {
    if (!std::filesystem::exists(cnt::logs))
    {
        std::error_code error_code;
        std::filesystem::create_directory(cnt::logs.parent_path(), error_code);
        if (error_code)
        { // todo throw custom errors for messagebox
            throw std::runtime_error(error_code.message());
        }
        out_stream_ = std::ofstream(cnt::logs);
        if (!out_stream_.is_open())
        {
            throw std::runtime_error("logs init failed");
        }
    }
}

Logs::~Logs()
{
    out_stream_.flush();
    out_stream_.close();
}

void Logs::info_log(const std::string& message, std::source_location location)
{
    out_stream_ << string_factory("INFO", message, location) << std::flush;
}
void Logs::error_log(const std::string& message, std::source_location location)
{
    out_stream_ << string_factory("ERROR", message, location) << std::flush;
}
void Logs::warning_log(const std::string& message, std::source_location location)
{
    out_stream_ << string_factory("WARNING", message, location) << std::flush;
}
