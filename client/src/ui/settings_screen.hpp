#pragma once

#include <QWidget>

#include <controllers/main_controller.hpp>

class SettingsScreen final : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsScreen(MainController& controller, QWidget* parent = nullptr);

signals:
    void done();
private:
    MainController& controller_;
};
