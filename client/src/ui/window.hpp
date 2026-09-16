#ifndef MAIN_WINDOW_PASSWORD_MANAGER_HPP
#define MAIN_WINDOW_PASSWORD_MANAGER_HPP
#include <QWidget>

#include "controllers/main_controller.hpp"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(MainController& controller);
};

#endif
