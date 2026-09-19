#pragma once
#include <string>
#include <nlohmann/json.hpp>

struct Service
{
    std::string name;
    std::string login;
    std::string password;
    // todo std::string create_data;
};

void to_json(nlohmann::json& json, const Service& service);
void from_json(const nlohmann::json& json, Service& service);
