#pragma once

#include <boost/beast/http.hpp>

namespace beast = boost::beast;
namespace http = beast::http;

class Database;

http::response<http::string_body> request_handler(
    const http::request<http::string_body>& request,
    Database& database);
