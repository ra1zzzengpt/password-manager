//
// Created by devnull on 09.10.2026.
//

#include "request_handler.hpp"

#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <nlohmann/json.hpp>

#include "../database/database.hpp"
#include "../logging/logger.hpp"

namespace
{
    constexpr std::string_view server_name = "password-manager-server";

    http::response<http::string_body> json_response(
        const http::request<http::string_body>& request,
        const http::status status,
        std::string body)
    {
        http::response<http::string_body> response{status, request.version()};
        response.set(http::field::server, server_name);
        response.set(http::field::content_type, "application/json");
        response.keep_alive(request.keep_alive());
        response.body() = std::move(body);
        response.prepare_payload();
        return response;
    }

    http::response<http::string_body> not_found(
        const http::request<http::string_body>& request)
    {
        server_log::warning("router", "route not found method=", request.method_string(),
                            " target=", request.target());
        return json_response(request, http::status::not_found,
                             R"({"error":"route not found"})");
    }

    http::response<http::string_body> method_not_allowed(
        const http::request<http::string_body>& request,
        const std::string_view allowed_method)
    {
        server_log::warning("router", "method not allowed target=", request.target(),
                            " actual=", request.method_string(),
                            " allowed=", allowed_method);
        auto response = json_response(request, http::status::method_not_allowed,
                                      R"({"error":"method not allowed"})");
        response.set(http::field::allow, allowed_method);
        return response;
    }

    http::response<http::string_body> error_response(
        const http::request<http::string_body>& request)
    {
        return json_response(request, http::status::internal_server_error,
                             R"({"error":"internal server error"})");
    }
}

http::response<http::string_body> request_handler(
    const http::request<http::string_body>& request,
    Database& database)
{
    const std::string_view target{request.target().data(), request.target().size()};

    try
    {
        if (target == "/registration")
        {
            if (request.method() != http::verb::post)
                return method_not_allowed(request, "POST");

            server_log::debug("handler", "processing registration");
            const auto body = nlohmann::json::parse(request.body());
            const auto account = database.createAccount(body.at("hash").get<std::uint32_t>());
            server_log::info("handler", "registration completed id=", account.id);
            return json_response(request, http::status::created,
                                 nlohmann::json{{"id", account.id}}.dump());
        }

        if (target == "/login")
        {
            if (request.method() != http::verb::post)
                return method_not_allowed(request, "POST");

            const auto body = nlohmann::json::parse(request.body());
            const auto id = body.at("id").get<std::uint32_t>();
            const auto hash = body.at("hash").get<std::uint32_t>();
            const bool authenticated = database.account(id, hash).has_value();
            server_log::info("handler", "login id=", id,
                             " success=", authenticated ? "true" : "false");
            return json_response(request, http::status::ok,
                                 nlohmann::json{{"success", authenticated}}.dump());
        }

        if (target == "/vault/get")
        {
            if (request.method() != http::verb::get)
                return method_not_allowed(request, "GET");

            const auto body = nlohmann::json::parse(request.body());
            const auto id = body.at("id").get<std::uint32_t>();
            const auto account = database.account(id, body.at("hash").get<std::uint32_t>());
            if (!account)
            {
                server_log::warning("handler", "vault version request unauthorized id=", id);
                return json_response(request, http::status::unauthorized,
                                     R"({"error":"unauthorized"})");
            }
            const auto client_version = body.at("vault_version").get<std::uint32_t>();
            const bool up_to_date = client_version >= account->vault_version;
            server_log::info("handler", "vault version checked id=", id,
                             " client_version=", client_version,
                             " server_version=", account->vault_version,
                             " up_to_date=", up_to_date ? "true" : "false");
            return json_response(request, http::status::ok,
                                 nlohmann::json{{"success", up_to_date}}.dump());
        }

        if (target == "/vault/fetch")
        {
            if (request.method() != http::verb::get)
                return method_not_allowed(request, "GET");

            const auto body = nlohmann::json::parse(request.body());
            const auto id = body.at("id").get<std::uint32_t>();
            const auto account = database.account(id, body.at("hash").get<std::uint32_t>());
            if (!account)
            {
                server_log::warning("handler", "vault fetch unauthorized id=", id);
                return json_response(request, http::status::unauthorized,
                                     R"({"error":"unauthorized"})");
            }

            std::ifstream file{account->vault_path, std::ios::binary};
            if (!file)
            {
                server_log::warning("storage", "vault file not found id=", id,
                                    " path=", account->vault_path);
                return json_response(request, http::status::not_found,
                                     R"({"error":"vault not found"})");
            }
            const std::vector<std::uint8_t> vault(
                (std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            server_log::info("storage", "vault loaded id=", id,
                             " bytes=", vault.size());
            return json_response(request, http::status::ok,
                                 nlohmann::json{{"vault", vault}}.dump());
        }

        if (target == "/vault/new")
        {
            if (request.method() != http::verb::post)
                return method_not_allowed(request, "POST");

            const auto body = nlohmann::json::parse(request.body());
            const auto id = body.at("id").get<std::uint32_t>();
            const auto hash = body.at("hash").get<std::uint32_t>();
            const auto version = body.at("vault_version").get<std::uint32_t>();
            const auto vault = body.at("vault").get<std::vector<std::uint8_t>>();
            const auto account = database.account(id, hash);
            if (!account)
            {
                server_log::warning("handler", "vault update unauthorized id=", id);
                return json_response(request, http::status::unauthorized,
                                     R"({"error":"unauthorized"})");
            }
            if (version < account->vault_version)
            {
                server_log::warning("handler", "stale vault update rejected id=", id,
                                    " client_version=", version,
                                    " server_version=", account->vault_version);
                return json_response(request, http::status::ok,
                                     R"({"success":false})");
            }

            const std::filesystem::path path{account->vault_path};
            std::error_code ec;
            std::filesystem::create_directories(path.parent_path(), ec);
            if (ec)
            {
                server_log::error("storage", "cannot create vault directory id=", id,
                                  ": ", ec.message());
                return error_response(request);
            }

            const std::filesystem::path temporary{path.string() + ".tmp"};
            server_log::debug("storage", "writing vault id=", id,
                              " version=", version, " bytes=", vault.size());
            std::ofstream file{temporary, std::ios::binary | std::ios::trunc};
            file.write(reinterpret_cast<const char*>(vault.data()),
                       static_cast<std::streamsize>(vault.size()));
            file.close();
            if (!file)
            {
                server_log::error("storage", "cannot write vault id=", id,
                                  " temporary_path=", temporary.string());
                std::filesystem::remove(temporary, ec);
                return error_response(request);
            }
            std::filesystem::rename(temporary, path, ec);
            if (ec)
            {
                server_log::error("storage", "cannot replace vault file id=", id,
                                  ": ", ec.message());
                std::filesystem::remove(temporary, ec);
                return error_response(request);
            }
            if (!database.updateVaultVersion(id, hash, version))
            {
                server_log::error("handler", "vault saved but version update failed id=", id,
                                  " version=", version);
                return error_response(request);
            }

            server_log::info("storage", "vault saved id=", id,
                             " version=", version, " bytes=", vault.size());
            return json_response(request, http::status::ok,
                                 R"({"success":true})");
        }

        return not_found(request);
    }
    catch (const nlohmann::json::exception& exception)
    {
        // Do not print exception.what(): JSON diagnostics may contain request data.
        server_log::warning("handler", "invalid JSON target=", target,
                            " error_id=", exception.id);
        return json_response(request, http::status::bad_request,
                             R"({"error":"invalid json"})");
    }
    catch (const std::exception& exception)
    {
        server_log::error("handler", "request failed target=", target,
                          ": ", exception.what());
        return error_response(request);
    }
}
