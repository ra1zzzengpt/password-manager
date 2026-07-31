cmake_minimum_required(VERSION 3.20)
project(password-manager VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

include(FetchContent)

FetchContent_Declare(
        libsodium_cmake
        GIT_REPOSITORY https://github.com/robinlinden/libsodium-cmake.git
        GIT_TAG        9b2848dfc1b917a9410f0de9d81059b26cbfaa8d
)

FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json
        GIT_TAG        v3.11.3
)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
# find_package(CURL REQUIRED) for server update

FetchContent_MakeAvailable(libsodium_cmake nlohmann_json)

add_subdirectory(client)