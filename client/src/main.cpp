#include <QMessageBox>
#include <QApplication>
#include <cstdlib>
#include <memory>
#include <exception>
#include <logs/logs.hpp>
#include <ui/runner.hpp>

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

        MainController mainController{*logs};
        Runner runner{mainController};
        runner.run();
        const int exit_code = app.exec();

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
