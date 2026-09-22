#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>

#include <nlohmann/json.hpp>

struct Config
{
    std::uint32_t volume = 20; // Percent, 0..100.
    std::string theme = "dark"; // dark, white, nord or blue.
};

inline void to_json(nlohmann::json& json, const Config& config)
{
    json = {{"volume", config.volume}, {"theme", config.theme}};
}

inline void from_json(const nlohmann::json& json, Config& config)
{
    if (!json.is_object() || !json.contains("volume") || !json.at("volume").is_number_integer()
        || json.at("volume") < 0 || json.at("volume") > 100
        || !json.contains("theme") || !json.at("theme").is_string())
    {
        throw std::invalid_argument("Invalid configuration fields");
    }

    const auto theme = json.at("theme").get<std::string>();
    if (theme != "dark" && theme != "white" && theme != "nord" && theme != "blue")
    {
        throw std::invalid_argument("Unknown configuration theme: " + theme);
    }

    config.volume = json.at("volume").get<std::uint32_t>();
    config.theme = theme;
}
