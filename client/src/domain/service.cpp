#include <domain/service.hpp>

void from_json(const nlohmann::json& json, Service& service)
{
    service.name = json.value("name", "");
    service.login = json.value("login", "");
    service.password = json.value("password", "");
    service.created_at = json.value("created_at", "");
}

void to_json(nlohmann::json& json, const Service& service)
{
    json = nlohmann::json{
        {"name",service.name},
        {"login", service.login},
        {"password", service.password},
        {"created_at", service.created_at}
    };
}