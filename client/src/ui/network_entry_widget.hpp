#pragma once
#include <QWidget>

#include "controllers/main_controller.hpp"

class NetworkEntryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NetworkEntryWidget(MainController& controller, QWidget* parent);
signals:
    void loggedIn();
private:
    MainController& mainController_;
};
