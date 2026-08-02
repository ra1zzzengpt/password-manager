#pragma once

#include <fstream>
#include <mutex>
#include <string>
#include <source_location>

class Logs final
{
public:
    Logs();
    ~Logs();
    Logs(const Logs&) = delete;
    Logs(const Logs&&) = delete;
    Logs& operator=(const Logs&) = delete;
    Logs& operator=(Logs&&) = delete;

    void info_log(std::string_view message, std::source_location location = std::source_location::current());
    void error_log(std::string_view message, std::source_location location = std::source_location::current());
    void warning_log(std::string_view message, std::source_location location = std::source_location::current());

private:
    void write(std::string_view level, std::string_view message, std::source_location location);

    std::ofstream out_stream_;
    std::mutex mutex_;
};
