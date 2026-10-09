//
// Created by arch on 10/8/26.
//

#ifndef PASSWORD_MANAGER_NETWORK_CONTROLLER_HPP
#define PASSWORD_MANAGER_NETWORK_CONTROLLER_HPP
#include <expected>
#include <string>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/ssl.hpp>

#include "domain/error/error.hpp"

namespace http = boost::beast::http;
namespace net = boost::asio;
namespace beast = boost::beast;

namespace ssl = net::ssl;

using tcp = net::ip::tcp;

class NetworkController final
{
public:
    NetworkController(std::string host, std::string port);

    std::expected<bool,err::Error> new_vault(const std::uint32_t& account_id, const std::uint32_t& password_hash, const std::uint32_t& vault_version, const std::vector<std::uint8_t>& vault);

    std::expected<bool,err::Error> ask_vault(const std::uint32_t& account_id, const std::uint32_t& password_hash, const std::uint32_t& vault_version);

    std::expected<std::vector<std::uint8_t>,err::Error> fetch_vault(const std::uint32_t& account_id, const std::uint32_t& password_hash, const std::uint32_t& vault_version);

    std::expected<std::uint32_t,err::Error> registration(const std::uint32_t& password_hash);

    std::expected<bool,err::Error> login(const std::uint32_t& account_id, const std::uint32_t& password_hash);

private:
    std::string host_;
    std::string port_;

    std::expected<http::response<http::string_body>,err::Error> request_response(const http::request<http::string_body>& request);
};


#endif //PASSWORD_MANAGER_NETWORK_CONTROLLER_HPP
