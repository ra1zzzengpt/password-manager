add_library(pwd-core STATIC
        src/controllers/main_controller.cpp
        src/crypto/sodium.cpp
        src/crypto/sodium.hpp
        src/controllers/storage_controller.cpp
        src/controllers/storage_controller.hpp
        src/domain/error/error.hpp
        src/logs/logs.cpp
        src/logs/logs.hpp
        src/domain/error/error_type.hpp
        src/domain/service.hpp
        src/domain/service.cpp
        src/ui/pw_screen.cpp
        src/ui/pw_screen.hpp
        src/ui/main_screen.cpp
        src/ui/main_screen.hpp
        src/constants/paths.hpp
        src/crypto/helper/sodium_info.cpp
        src/crypto/helper/sodium_info.hpp
        src/ui/runner.cpp
        src/ui/runner.hpp
        src/generate/generator.cpp
        src/generate/generator.hpp
        src/utils/transform.hpp
        src/constants/symbols.hpp
        src/ui/settings_screen.cpp
        src/ui/settings_screen.hpp
)

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