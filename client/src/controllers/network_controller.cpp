#include "network_controller.hpp"

#include <expected>
#include <nlohmann/json.hpp>
#include <domain/error/error.hpp>
#include <utility>

namespace
{
    err::NetworkError network_error_from_status(const unsigned status_code)
    {
        switch (status_code)
        {
        case 400:
            return err::NetworkError::BadRequest;
        case 401:
            return err::NetworkError::Unauthorized;
        case 403:
            return err::NetworkError::Forbidden;
        case 404:
            return err::NetworkError::NotFound;
        case 405:
            return err::NetworkError::MethodNotAllowed;
        case 408:
            return err::NetworkError::RequestTimeout;
        case 409:
            return err::NetworkError::Conflict;
        case 413:
            return err::NetworkError::PayloadTooLarge;
        case 415:
            return err::NetworkError::UnsupportedMediaType;
        case 422:
            return err::NetworkError::UnprocessableEntity;
        case 429:
            return err::NetworkError::TooManyRequests;
        case 500:
            return err::NetworkError::InternalServerError;
        case 501:
            return err::NetworkError::NotImplemented;
        case 502:
            return err::NetworkError::BadGateway;
        case 503:
            return err::NetworkError::ServiceUnavailable;
        case 504:
            return err::NetworkError::GatewayTimeout;
        default:
            if (status_code >= 100 && status_code < 200)
                return err::NetworkError::InformationalResponse;
            if (status_code >= 300 && status_code < 400)
                return err::NetworkError::Redirection;
            if (status_code >= 400 && status_code < 500)
                return err::NetworkError::ClientError;
            if (status_code >= 500 && status_code < 600)
                return err::NetworkError::ServerError;
            return err::NetworkError::UnexpectedHttpStatus;
        }
    }

    template<typename Body>
    err::Error error_from_response(const http::response<Body>& response)
    {
        const unsigned status_code = response.result_int();
        std::string message = "HTTP " + std::to_string(status_code);
        if (!response.reason().empty())
            message += " " + std::string(response.reason());

        return err::Error{
            .type = network_error_from_status(status_code),
            .message = std::move(message),
        };
    }

    http::request<http::string_body> base_request(
        const http::verb& method,
        const std::string& target,
        const std::uint32_t& version,
        const std::string& host)
    {
        http::request<http::string_body> request{method,target,version};
        request.set(http::field::host,host);
        request.set(http::field::user_agent, "password-manager");
        request.set(http::field::content_type, "application/json");
        request.set(http::field::accept, "application/json");
        return request;
    }
}

NetworkController::NetworkController(std::string host, std::string port) : host_(std::move(host)), port_(std::move(port)) {}

std::expected<http::response<http::string_body>,err::Error> NetworkController::request_response(const http::request<http::string_body> &request) {
    net::io_context io;

    ssl::context ctx(ssl::context_base::tls_client);

    try
    {
        ctx.load_verify_file("/home/devnull/CLionProjects/password-manager/client/cert/ca.cert.pem");
    }
    catch (...)
    {
        return std::unexpected{err::Error{.type = err::NetworkError::CantLoadSertificate,.message = "cant load sertificate"}};
    }

    tcp::resolver resolver(io);

    beast::ssl_stream<beast::tcp_stream> stream(io,ctx);

    try
    {
        auto endpoints = resolver.resolve(host_,port_);

        beast::get_lowest_layer(stream).connect(endpoints);

        stream.handshake(ssl::stream_base::client);

        http::write(stream,request);
    } catch (...)
    {
        return std::unexpected{err::Error{.type = err::NetworkError::CantConnectToServer, .message = "cant connect to server"}};
    }

    beast::flat_buffer buffer;
    http::response<http::string_body> response;

    try
    {
        http::read(stream,buffer,response);
    }
    catch (...)
    {
        return std::unexpected{err::Error{.type = err::NetworkError::CantReadResponse,.message = "cant read server response"}};
    }

    beast::error_code shutdown_error;
    stream.shutdown(shutdown_error);
    if (shutdown_error == net::error::eof || shutdown_error == ssl::error::stream_truncated)
        shutdown_error = {};

    if (shutdown_error)
    {
        return std::unexpected{err::Error{.type = err::NetworkError::CantShutdownConnection,.message = "cant shutdown connection"}};
    }

    const unsigned status_code = response.result_int();
    if (status_code < 200 || status_code >= 300)
        return std::unexpected{error_from_response(response)};

    return response;
}

std::expected<std::uint32_t, err::Error> NetworkController::registration(const std::uint32_t& password_hash)
{
    http::request<http::string_body> request = base_request(http::verb::post,"/registration",11,host_);
    request.body() = nlohmann::json{{"hash",password_hash}}.dump();
    request.prepare_payload();

    if (auto res = request_response(request); !res.has_value())
    {
        return std::unexpected{res.error()};
    } else
    {
        std::uint32_t result;
        try
        {
            const nlohmann::json response_json = nlohmann::json::parse(res.value().body());
            result = response_json.at("id").get<std::uint32_t>();
        } catch (...)
        {
            return std::unexpected{err::Error{.type = err::TransformError::TransformFailed,.message = "parse error"}};
        }
        return result;
    }
}

std::expected<bool,err::Error> NetworkController::login(const std::uint32_t& account_id, const std::uint32_t& password_hash)
{
    http::request<http::string_body> request = base_request(http::verb::post,"/login",11,host_);
    request.body() = nlohmann::json{{"id",account_id},{"hash",password_hash}}.dump();
    request.prepare_payload();

    if (auto res = request_response(request); !res.has_value())
    {
        return std::unexpected{res.error()};
    } else
    {
        bool result;
        try
        {
            const nlohmann::json response_json = nlohmann::json::parse(res.value().body());
            result = response_json.at("success").get<bool>();
        } catch (...)
        {
            return std::unexpected{err::Error{.type = err::TransformError::TransformFailed,.message = "parse error"}};
        }
        return result;
    }
}

std::expected<bool, err::Error> NetworkController::ask_vault(const std::uint32_t &account_id, const std::uint32_t &password_hash, const std::uint32_t& vault_version)
{
    http::request<http::string_body> request = base_request(http::verb::get,"/vault/get",11,host_);
    request.body() = nlohmann::json{{"id",account_id},{"hash",password_hash},{"vault_version",vault_version}}.dump();
    request.prepare_payload();

    if (auto res = request_response(request); !res.has_value())
    {
        return std::unexpected{res.error()};
    } else
    {
        bool result;
        try
        {
            const nlohmann::json response_json = nlohmann::json::parse(res.value().body());
            result = response_json.at("success").get<bool>();
        } catch (...)
        {
            return std::unexpected{err::Error{.type = err::TransformError::TransformFailed,.message = "parse error"}};
        }
        return result;
    }
}

std::expected<std::vector<std::uint8_t>, err::Error> NetworkController::fetch_vault(
    const std::uint32_t& account_id,
    const std::uint32_t& password_hash,
    const std::uint32_t& vault_version)
{
    http::request<http::string_body> request = base_request(http::verb::get,"/vault/fetch",11,host_);
    request.body() = nlohmann::json{
        {"id",account_id},
        {"hash",password_hash},
        {"vault_version",vault_version},
    }.dump();
    request.prepare_payload();

    const auto response = request_response(request);
    if (!response.has_value())
        return std::unexpected{response.error()};

    try
    {
        const nlohmann::json response_json = nlohmann::json::parse(response->body());
        return response_json.at("vault").get<std::vector<std::uint8_t>>();
    }
    catch (...)
    {
        return std::unexpected{err::Error{
            .type = err::TransformError::TransformFailed,
            .message = "can't parse vault from server response",
        }};
    }
}

std::expected<bool, err::Error> NetworkController::new_vault(const std::uint32_t &account_id, const std::uint32_t &password_hash, const std::uint32_t& vault_version, const std::vector<std::uint8_t> &vault)
{
    http::request<http::string_body> request = base_request(http::verb::post,"/vault/new",11,host_);
    request.body() = nlohmann::json{{"id",account_id},{"hash",password_hash},{"vault_version",vault_version},{"vault",vault}}.dump();
    request.prepare_payload();

    if (auto res = request_response(request); !res.has_value())
    {
        return std::unexpected{res.error()};
    } else
    {
        bool result;
        try
        {
            const nlohmann::json response_json = nlohmann::json::parse(res.value().body());
            result = response_json.at("success").get<bool>();
        } catch (...)
        {
            return std::unexpected{err::Error{.type = err::TransformError::TransformFailed,.message = "parse error"}};
        }
        return result;
    }
}
