#include <domain/service.hpp>

void from_json(const nlohmann::json& json, Service& service)
{
    service.name = json.value("name", "");
    service.login = json.value("login", "");
    service.password = json.value("password", "");
}

void to_json(nlohmann::json& json, const Service& service)
{
    json = nlohmann::json{
        {"name",service.name},
        {"login", service.login},
        {"password", service.password}
    };
}