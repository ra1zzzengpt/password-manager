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
        src/ui/entry_widget.cpp
        src/ui/entry_widget.hpp
        src/ui/main_widget.cpp
        src/ui/main_widget.hpp
        src/constants/paths.hpp
        src/crypto/helper/sodium_info.cpp
        src/crypto/helper/sodium_info.hpp
        src/generate/generator.cpp
        src/generate/generator.hpp
        src/utils/transform.hpp
        src/constants/symbols.hpp
        src/ui/settings_dialog.cpp
        src/ui/settings_dialog.hpp
        src/ui/window.cpp
        src/ui/window.hpp
        src/ui/add_service_dialog.cpp
        src/ui/add_service_dialog.hpp
        src/ui/current_service_dialog.cpp
        src/ui/current_service_dialog.hpp
        src/ui/custom_list_widget.cpp
        src/ui/custom_list_widget.hpp
)

target_include_directories(
        pwd-core PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Multimedia MultimediaWidgets)

target_link_libraries(pwd-core PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Multimedia
        Qt6::MultimediaWidgets
        sodium
        nlohmann_json::nlohmann_json
)
add_executable(password-manager src/main.cpp)

qt_add_resources(password-manager theme
        PREFIX "/"
        FILES assets/theme.qss assets/icons/settings.png assets/icons/exit.png assets/icons/mail.png
        assets/icons/edit.png assets/icons/apply.png assets/icons/delete.png assets/icons/copy.png
        assets/icons/add.png
)

target_link_libraries(password-manager PRIVATE
        pwd-core
        Qt6::Core
        Qt6::Widgets
        Qt6::Multimedia
        Qt6::MultimediaWidgets
)