#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>

#include <nlohmann/json.hpp>

struct Config
{
    std::string theme = "dark"; // dark, white, nord or blue.
};

inline void to_json(nlohmann::json& json, const Config& config)
{
    json = {{"theme", config.theme}};
}

inline void from_json(const nlohmann::json& json, Config& config)
{
    if (!json.is_object()
        || !json.contains("theme") || !json.at("theme").is_string())
    {
        throw std::invalid_argument("Invalid configuration fields");
    }

    const auto theme = json.at("theme").get<std::string>();
    if (theme != "dark" && theme != "white" && theme != "nord" && theme != "blue")
    {
        throw std::invalid_argument("Unknown configuration theme: " + theme);
    }

    config.theme = theme;
}
