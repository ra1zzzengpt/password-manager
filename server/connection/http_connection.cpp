//
// Created by devnull on 09.10.2026.
//

#include "http_connection.hpp"

#include "../handlers/request_handler.hpp"
#include "../logging/logger.hpp"

std::atomic_uint64_t HttpConnection::next_id_{0};

HttpConnection::HttpConnection(tcp::socket &socket, ssl::context &context, Database& database)
    : stream_(std::move(socket),context), database_(database), id_(++next_id_)
{
    beast::error_code ec;
    const auto endpoint = beast::get_lowest_layer(stream_).socket().remote_endpoint(ec);
    peer_ = ec ? "unknown" : endpoint.address().to_string() + ':' + std::to_string(endpoint.port());
    server_log::info("connection", '#', id_, " created peer=", peer_);
}

void HttpConnection::run()
{
    handshake();
}

void HttpConnection::handshake()
{
    server_log::debug("connection", '#', id_, " starting TLS handshake");
    stream_.async_handshake(ssl::stream_base::server, beast::bind_front_handler(&HttpConnection::onHandshake, shared_from_this()));
}

void HttpConnection::onHandshake(beast::error_code ec)
{
    if (ec)
    {
        server_log::error("connection", '#', id_, " TLS handshake failed peer=",
                          peer_, ": ", ec.message(), " (", ec.value(), ')');
        return;
    }
    server_log::info("connection", '#', id_, " TLS handshake completed peer=", peer_);
    read();
}

void HttpConnection::read()
{
    server_log::debug("connection", '#', id_, " waiting for HTTP request");
    http::async_read(stream_,buffer_,request_,beast::bind_front_handler(&HttpConnection::onRead, shared_from_this()));
}

void HttpConnection::onRead(beast::error_code ec, std::size_t transported_bytes)
{
    boost::ignore_unused(transported_bytes);
    if (ec == http::error::end_of_stream)
    {
        server_log::info("connection", '#', id_, " client closed HTTP stream");
        close();
        return;
    }
    if (ec)
    {
        server_log::error("connection", '#', id_, " HTTP read failed: ",
                          ec.message(), " (", ec.value(), ')');
        return;
    }
    request_started_ = std::chrono::steady_clock::now();
    server_log::info("http", '#', id_, " request method=", request_.method_string(),
                     " target=", request_.target(), " body_bytes=", request_.body().size(),
                     " received_bytes=", transported_bytes,
                     " keep_alive=", request_.keep_alive() ? "true" : "false");
    response_ = request_handler(request_, database_);
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - request_started_);
    server_log::info("http", '#', id_, " response status=", response_.result_int(),
                     " body_bytes=", response_.body().size(),
                     " duration_ms=", elapsed.count());
    request_ = {};

    write();
}

void HttpConnection::write()
{
    http::async_write(stream_,response_,beast::bind_front_handler(&HttpConnection::onWrite, shared_from_this()));
}

void HttpConnection::onWrite(beast::error_code ec, std::size_t transported_bytes)
{
    boost::ignore_unused(transported_bytes);
    if (ec)
    {
        server_log::error("connection", '#', id_, " HTTP write failed: ",
                          ec.message(), " (", ec.value(), ')');
        return;
    }
    server_log::debug("connection", '#', id_, " response sent bytes=", transported_bytes);
    if (response_.keep_alive())
    {
        read();
        return;
    }
    close();
}

void HttpConnection::close()
{
    server_log::debug("connection", '#', id_, " shutting down TLS");
    stream_.async_shutdown(beast::bind_front_handler(&HttpConnection::onClose, shared_from_this()));
}

void HttpConnection::onClose(beast::error_code ec)
{
    if (ec && ec != net::error::eof && ec != ssl::error::stream_truncated)
    {
        server_log::warning("connection", '#', id_, " TLS shutdown warning: ",
                            ec.message(), " (", ec.value(), ')');
    }
    beast::error_code ignored;
    beast::get_lowest_layer(stream_).socket().shutdown(tcp::socket::shutdown_both, ignored);
    beast::get_lowest_layer(stream_).socket().close(ignored);
    server_log::info("connection", '#', id_, " closed peer=", peer_);
}
