add_library(pwd-core STATIC
src/main.cpp)

target_include_directories(
        pwd-core PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

target_link_libraries(pwd-core PRIVATE
        Qt6::Core
        Qt6::Widgets
        sodium
        nlohmann_json::nlohmann_json
)
add_executable(password-manager src/main.cpp)

target_link_libraries(password-manager PRIVATE
        pwd-core
        Qt6::Core
        Qt6::Widgets
)