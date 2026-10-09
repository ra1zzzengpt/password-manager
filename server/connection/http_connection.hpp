#pragma once
#include <atomic>
#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/ssl.hpp>

namespace net = boost::asio;
namespace beast = boost::beast;
namespace ssl = net::ssl;
namespace http = beast::http;

using tcp = net::ip::tcp;

class Database;

class HttpConnection : public std::enable_shared_from_this<HttpConnection>
{
public:
    HttpConnection(tcp::socket& socket, ssl::context& context, Database& database);

    void run();

private:
    beast::ssl_stream<beast::tcp_stream> stream_;
    beast::flat_buffer buffer_;
    http::request<http::string_body> request_;
    http::response<http::string_body> response_;
    Database& database_;
    std::uint64_t id_;
    std::string peer_;
    std::chrono::steady_clock::time_point request_started_;

    static std::atomic_uint64_t next_id_;

    void handshake();

    void onHandshake(beast::error_code ec);

    void read();

    void onRead(beast::error_code ec, std::size_t transported_bytes);

    void write();

    void onWrite(beast::error_code ec, std::size_t transported_bytes);

    void close();

    void onClose(beast::error_code ec);
};
