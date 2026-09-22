#include <QMessageBox>
#include <QApplication>
#include <QFile>
#include <cstdlib>
#include <memory>
#include <exception>
#include <logs/logs.hpp>

#include "constants/paths.hpp"
#include "controllers/configuration_controller.hpp"
#include "controllers/sound_controller.hpp"
#include "ui/window.hpp"

namespace
{
    void showCriticalError(const QString& message)
    {
        QMessageBox critical_message_box;
        critical_message_box.setWindowTitle("Critical Error");
        critical_message_box.setIcon(QMessageBox::Critical);
        critical_message_box.setText(message);
        critical_message_box.exec();
    }
}

int main(int argc, char* argv[])
{
    QApplication app{argc, argv};
    std::unique_ptr<Logs> logs;

    try
    {
        logs = std::make_unique<Logs>();
        logs->info_log("Application startup");

        ConfigurationController configuration_controller;
        QString load_error;
        bool loaded = false;
        try
        {
            if (auto result = configuration_controller.load(); result)
            {
                loaded = true;
            } else
            {
                load_error = QString::fromStdString(result.error().message);
            }
        } catch (const err::Error& e)
        {
            load_error = QString::fromStdString(e.message);
        } catch (const std::exception& e)
        {
            load_error = QString::fromUtf8(e.what());
        } catch (...)
        {
            load_error = "Unknown configuration error.";
        }

        if (!loaded)
        {
            const auto answer = QMessageBox::question(nullptr, "Configuration error",
                "Could not load configuration: " + load_error + "\nOverwrite it with defaults?",
                QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
            if (answer != QMessageBox::Yes)
            {
                logs->info_log("Application shutdown after configuration load failure");
                return EXIT_FAILURE;
            }
            if (auto result = configuration_controller.to_default(); !result)
            {
                throw result.error();
            }
        }

        const auto& config = configuration_controller.config();
        const QString theme_path = ":/assets/" + QString::fromStdString(config.theme) + "_theme.qss";
        QFile theme(theme_path);
        if (!theme.open(QFile::ReadOnly))
        {
            throw err::Error{err::StorageError::OpenFileFailed,
                "Can't open theme resource: " + theme_path.toStdString()};
        }
        app.setStyleSheet(QString::fromUtf8(theme.readAll()));

        MainController mainController{*logs};
        SoundController sound_controller{config.volume};

        MainWindow* mainWindow = new MainWindow(mainController, sound_controller,
                                               configuration_controller, app);
        mainWindow->show();
        const int exit_code = QApplication::exec();

        logs->info_log("Application shutdown");
        return exit_code;
    } catch (const err::Error& e)
    {
        if (logs)
        {
            logs->error_log("Application stopped by an internal error");
        }
        showCriticalError(QString::fromStdString(e.message));
    } catch (const std::exception& e)
    {
        if (logs)
        {
            logs->error_log("Application stopped by a standard exception");
        }
        showCriticalError(QString::fromUtf8(e.what()));
    } catch (...)
    {
        if (logs)
        {
            logs->error_log("Application stopped by an unknown exception");
        }
        showCriticalError("An unknown critical error occurred.");
    }

    return EXIT_FAILURE;
}
