#pragma once

#include <QDialog>

#include <controllers/main_controller.hpp>

class SettingsDialog final : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(MainController& controller, QWidget* parent = nullptr);

private:
    MainController& controller_;
};
