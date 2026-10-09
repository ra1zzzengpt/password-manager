#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/ssl.hpp>
#include "SECRETS.hpp"

#include "listener/listener.hpp"
#include "database/database.hpp"
#include "logging/logger.hpp"

namespace net = boost::asio;
namespace beast = boost::beast;
namespace ssl = net::ssl;
namespace http = beast::http;

using tcp = net::ip::tcp;

int main()
{
    try
    {
        server_log::info("server", "starting password manager server");
        net::io_context io;

        ssl::context ctx{ssl::context_base::tls_server};

        server_log::info("tls", "loading certificate and private key");
        ctx.use_certificate_chain_file("../../server/certs/server.cert.pem");
        ctx.use_private_key_file("../../server/certs/server.key.pem", ssl::context::pem);
        server_log::info("tls", "TLS configuration loaded");

        tcp::endpoint ep{tcp::v4(), 6767};

        const std::string database_info = "host=" + HOST + " port=" + PORT + " user=" + USERNAME + " dbname=" + DBNAME + " password=" + PASSWORD;
        server_log::info("database", "connecting to PostgreSQL host=", HOST,
                         " port=", PORT, " database=", DBNAME);
        Database database{database_info};

        std::make_shared<Listener>(io,ctx,ep,database)->run();
        server_log::info("server", "event loop started");
        io.run();
        server_log::info("server", "event loop stopped");
    }
    catch (const std::exception& e)
    {
        server_log::error("server", "fatal error: ", e.what());
        return 1;
    }
    return 0;
}
