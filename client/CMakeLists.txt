add_library(pwd-core STATIC
        src/controllers/configuration_controller.cpp
        src/controllers/main_controller.cpp
        src/crypto/sodium.cpp
        src/controllers/storage_controller.cpp
        src/logs/logs.cpp
        src/domain/service.cpp
        src/ui/entry_widget.cpp
        src/ui/main_widget.cpp
        src/crypto/helper/sodium_info.cpp
        src/generate/generator.cpp
        src/ui/settings_dialog.cpp
        src/ui/window.cpp
        src/ui/add_service_dialog.cpp
        src/ui/custom_list_widget.cpp
        src/ui/custom_widgets/service_card_widget.cpp
        src/controllers/sound_controller.cpp
        src/ui/custom_dialogs/more_info_dialog.cpp
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
add_executable(password-manager src/main.cpp
        src/utils/csv_parser.cpp
        src/utils/csv_parser.hpp)

qt_add_resources(password-manager theme
        PREFIX "/"
        FILES
        assets/dark_theme.qss
        assets/white_theme.qss
        assets/nord_theme.qss

        assets/icons/settings.png
        assets/icons/exit.png
        assets/icons/mail.png
        assets/icons/edit.png
        assets/icons/apply.png
        assets/icons/delete.png
        assets/icons/copy.png
        assets/icons/add.png
        assets/icons/web.png
        assets/icons/copy_login.png
        assets/icons/copy_password.png
        assets/icons/eye.png
        assets/icons/happy.png
        assets/icons/neutral.png
        assets/icons/sad.png
        assets/icons/save.png
        assets/icons/import.png
        assets/icons/export.png

        assets/icons/white_theme.png
        assets/icons/dark_theme.png
        assets/icons/nord_theme.png

        assets/sounds/click.wav
        assets/sounds/notification.wav
        assets/sounds/type.wav
)

target_link_libraries(password-manager PRIVATE
        pwd-core
        Qt6::Core
        Qt6::Widgets
        Qt6::Multimedia
        Qt6::MultimediaWidgets
)
