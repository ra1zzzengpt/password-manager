#pragma once

#include <fstream>
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

    void info_log(const std::string& message, std::source_location location = std::source_location::current());
    void error_log(const std::string& message, std::source_location location = std::source_location::current());
    void warning_log(const std::string& message, std::source_location location = std::source_location::current());

private:
    std::ofstream out_stream_;
};