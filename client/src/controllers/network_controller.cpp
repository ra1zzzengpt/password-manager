#include "network_controller.hpp"

#include <expected>
#include <nlohmann/json.hpp>
#include <domain/error/error.hpp>

NetworkController::NetworkController(std::string &host, std::string port) : host_(host), port_(port) {}

template<typename T, typename N>
std::expected<http::response<T>,err::Error> NetworkController::request_response(const http::request<N> &request) {
    net::io_context io;

    ssl::context ctx(ssl::context_base::tls_client);

    try
    {
        ctx.load_verify_file("/cert/ca.sert.pem");
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
    http::response<T> response;

    try {
        http::read(stream,buffer,response);

        beast::get_lowest_layer(stream).socket().shutdown(tcp::socket::shutdown_both);

        stream.shutdown();
    } catch (...)
    {
        return std::unexpected{err::Error{.type = err::NetworkError::CantShutdownConnection,.message = "cant shutdown connection"}};
    }

    if (response.result() != http::status::ok)
    {
        // todo return on result
    }

    return response;
}

std::expected<std::uint32_t, err::Error> NetworkController::registration(const std::string& password_hash)
{
    http::request<http::string_body> request{http::verb::post,"/register",11};
    request.set(http::field::host, host_);
    request.set(http::field::user_agent, "password-manager");
    request.set(http::field::content_type, "application/json");
    request.set(http::field::accept, "application/json");
    request.body() = nlohmann::json{{"hash",password_hash}};
    request.prepare_payload();

    if (auto res = request_response<http::string_body>(request); !res.has_value())
    {
        return std::unexpected{res.error()};
    } else
    {
        nlohmann::json response_json = nlohmann::json{res.value().body()};
        return response_json["id"].get<std::uint32_t>();
    }


}
