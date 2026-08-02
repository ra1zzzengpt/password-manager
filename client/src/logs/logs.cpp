#include <logs/logs.hpp>
#include <constants/paths.hpp>

#include <fstream>
#include <filesystem>
#include <chrono>
#include <format>

#include "domain/error/error.hpp"

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

Logs::Logs()
{
    std::error_code error_code;
    const std::filesystem::path log_path = cnt::logsPath();
    std::filesystem::create_directories(log_path.parent_path(), error_code);
    if (error_code)
    {
        throw err::Error{err::LogsError::CantCreateDirectory, "Can't create logs directory."};
    }

    out_stream_ = std::ofstream(log_path);
    if (!out_stream_.is_open())
    {
        throw err::Error{err::LogsError::InitError, "Can't open the log file."};
    }
}

Logs::~Logs()
{
    out_stream_.flush();
    out_stream_.close();
}

void Logs::write(const std::string_view level, const std::string_view message,
                 const std::source_location location)
{
    const std::scoped_lock lock{mutex_};
    out_stream_ << string_factory(level, message, location) << std::flush;
}

void Logs::info_log(const std::string_view message, const std::source_location location)
{
    write("INFO", message, location);
}

void Logs::error_log(const std::string_view message, const std::source_location location)
{
    write("ERROR", message, location);
}

void Logs::warning_log(const std::string_view message, const std::source_location location)
{
    write("WARNING", message, location);
}
