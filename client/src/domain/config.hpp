#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>

#include <nlohmann/json.hpp>

struct Config
{
    std::string theme = "dark"; // dark, white, nord or blue.
    bool sync = true;
    std::string host = "localhost";
    std::string port = "8080";
};

inline void to_json(nlohmann::json& json, const Config& config)
{
    json = {
        {"theme", config.theme},
        {"sync", config.sync},
        {"host", config.host},
        {"port", config.port},
    };
}

inline void from_json(const nlohmann::json& json, Config& config)
{
    if (!json.is_object() ||
        !json.contains("theme") || !json.at("theme").is_string() ||
        !json.contains("sync") || !json.at("sync").is_boolean())
    {
        throw std::invalid_argument("Invalid configuration fields");
    }

    const auto theme = json.at("theme").get<std::string>();
    if (theme != "dark" && theme != "white" && theme != "nord" && theme != "blue")
    {
        throw std::invalid_argument("Unknown configuration theme: " + theme);
    }

    config.theme = theme;
    config.sync = json.at("sync").get<bool>();

    if (config.sync)
    {
        if (!json.contains("host") || !json.at("host").is_string() ||
            !json.contains("port") || !json.at("port").is_string())
        {
            throw std::invalid_argument("Sync configuration requires host and port");
        }

        config.host = json.at("host").get<std::string>();
        config.port = json.at("port").get<std::string>();
    }
}
