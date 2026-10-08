cmake_minimum_required(VERSION 3.20)
project(password-manager VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

find_package(nlohmann_json CONFIG REQUIRED)

find_package(PkgConfig REQUIRED)

pkg_check_modules(SODIUM REQUIRED IMPORTED_TARGET libsodium)

# find_package(CURL REQUIRED) for server update

add_subdirectory(client)
add_subdirectory(server)