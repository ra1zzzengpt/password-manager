//
// Created by devnull on 09.10.2026.
//

#include "listener.hpp"

#include "../connection/http_connection.hpp"
#include "../logging/logger.hpp"

Listener::Listener(net::io_context &io, ssl::context &ctx, const tcp::endpoint& endpoint,
                   Database& database)
    : acceptor_(io), ctx_(std::move(ctx)), database_(database)
{
    acceptor_.open(endpoint.protocol());

    acceptor_.set_option(net::socket_base::reuse_address(true));

    acceptor_.bind(endpoint);

    acceptor_.listen(net::socket_base::max_listen_connections);
    server_log::info("listener", "listening on ", endpoint.address().to_string(),
                     ':', endpoint.port());
}

void Listener::run()
{
    accept();
}

void Listener::accept()
{
    server_log::debug("listener", "waiting for a connection");
    acceptor_.async_accept(beast::bind_front_handler(&Listener::onAccept, shared_from_this()));
}

void Listener::onAccept(beast::error_code ec, tcp::socket socket)
{
    if (ec)
    {
        server_log::error("listener", "accept failed: ", ec.message(),
                          " (", ec.value(), ')');
        return;
    }
    beast::error_code endpoint_ec;
    const auto endpoint = socket.remote_endpoint(endpoint_ec);
    if (endpoint_ec)
        server_log::info("listener", "connection accepted; peer address unavailable: ",
                         endpoint_ec.message());
    else
        server_log::info("listener", "connection accepted from ",
                         endpoint.address().to_string(), ':', endpoint.port());

    accept();
    std::make_shared<HttpConnection>(socket,ctx_,database_)->run();
}
