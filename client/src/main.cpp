#include <QMessageBox>
#include <QApplication>
#include <QFile>
#include <cstdlib>
#include <memory>
#include <exception>
#include <logs/logs.hpp>

#include "constants/paths.hpp"
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
    if (QFile theme(":/assets/dark_theme.qss"); theme.open(QFile::ReadOnly))
        app.setStyleSheet(QString(theme.readAll()));

    std::unique_ptr<Logs> logs;

    try
    {
        logs = std::make_unique<Logs>();
        logs->info_log("Application startup");

        MainController mainController{*logs};
        SoundController sound_controller;

        MainWindow* mainWindow = new MainWindow(mainController,sound_controller);
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
