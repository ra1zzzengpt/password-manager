find_package(OpenSSL REQUIRED)

find_package(libpqxx REQUIRED)

find_package(nlohmann_json REQUIRED)

add_executable(server main.cpp
        listener/listener.cpp
        listener/listener.hpp
        connection/http_connection.cpp
        connection/http_connection.hpp
        handlers/request_handler.cpp
        handlers/request_handler.hpp
        database/database.cpp
        database/database.hpp
        logging/logger.cpp
        logging/logger.hpp
        SECRETS.hpp)

target_compile_features(server PRIVATE cxx_std_23)

target_link_libraries(server PRIVATE OpenSSL::SSL OpenSSL::Crypto libpqxx::pqxx nlohmann_json::nlohmann_json)

# OpenSSL libpqxx boost
