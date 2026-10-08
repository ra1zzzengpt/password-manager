#include "network_controller.hpp"

NetworkController::NetworkController(std::string &host, std::string port) : host_(host), port_(port) {}

template<typename T, typename N>
http::response<T> NetworkController::request_response(const http::request<N> &request) {
    net::io_context io;

    ssl::context ctx(ssl::context_base::tls_client);

    ctx.load_verify_file("/cert/ca.sert.pem");

    tcp::resolver resolver(io);

    auto endpoints = resolver.resolve(host_,port_);

    beast::ssl_stream<beast::tcp_stream> stream(io,ctx);

    beast::get_lowest_layer(stream).connect(endpoints);

    stream.handshake(ssl::stream_base::client);

    http::write(stream,request);

    beast::flat_buffer buffer;
    http::response<T> response;

    http::read(stream,buffer,response);

    beast::get_lowest_layer(stream).socket().shutdown(tcp::socket::shutdown_both);

    stream.shutdown();

    return response;
}

void NetworkController::registration()
{
    http::request<http::string_body>
}
