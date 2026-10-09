#ifndef MAIN_WINDOW_PASSWORD_MANAGER_HPP
#define MAIN_WINDOW_PASSWORD_MANAGER_HPP
#include <QWidget>

#include "controllers/configuration_controller.hpp"
#include "controllers/main_controller.hpp"

class QThread;
class VaultAsker;
class QPushButton;
class MainWidget;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(MainController& controller,
               ConfigurationController& configuration_controller, QApplication& app);
    ~MainWindow() override;

public slots:
    void stopAsker();

private:
    MainController& controller_;
    ConfigurationController& configuration_controller_;
    QApplication& app_;
    QThread* asker_thread_{};
    VaultAsker* asker_worker_{};
    QPushButton* reconnect_button_{};
    MainWidget* main_widget_{};

    void asker();
    bool fetchVault();

signals:
    void askedSuccessfully();
    void connectionRefused();
    void stopAskerRequested();
};

#endif
