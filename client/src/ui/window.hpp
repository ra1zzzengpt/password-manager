#ifndef MAIN_WINDOW_PASSWORD_MANAGER_HPP
#define MAIN_WINDOW_PASSWORD_MANAGER_HPP
#include <QWidget>

#include "controllers/configuration_controller.hpp"
#include "controllers/main_controller.hpp"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(MainController& controller,
               ConfigurationController& configuration_controller, QApplication& app);
private:
    MainController& controller_;
    ConfigurationController& configuration_controller_;
    QApplication& app_;
};

#endif
