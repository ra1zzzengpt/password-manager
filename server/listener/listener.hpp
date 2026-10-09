#pragma once
#include <memory>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

namespace net = boost::asio;
namespace beast = boost::beast;
namespace ssl = net::ssl;
namespace http = beast::http;

using tcp = net::ip::tcp;

class Database;

class Listener : public std::enable_shared_from_this<Listener>
{
public:
    Listener(net::io_context& io, ssl::context& ctx, const tcp::endpoint& endpoint,
             Database& database);

    void run();

private:
    tcp::acceptor acceptor_;
    ssl::context ctx_;
    Database& database_;

    void accept();

    void onAccept(beast::error_code ec, tcp::socket socket);
};
