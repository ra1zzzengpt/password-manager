//
// Created by arch on 10/8/26.
//

#ifndef PASSWORD_MANAGER_NETWORK_CONTROLLER_HPP
#define PASSWORD_MANAGER_NETWORK_CONTROLLER_HPP
#include <string>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/ssl.hpp>

namespace http = boost::beast::http;
namespace net = boost::asio;
namespace beast = boost::beast;

namespace ssl = net::ssl;

using tcp = net::ip::tcp;

class NetworkController final
{
public:
    NetworkController(std::string& host, std::string port);

    void new_vault();

    void registration();

    int login();

private:
    std::string host_;
    std::string port_;

    template <typename T, typename N> http::response<T> request_response(const http::request<N>& request);
};


#endif //PASSWORD_MANAGER_NETWORK_CONTROLLER_HPP
