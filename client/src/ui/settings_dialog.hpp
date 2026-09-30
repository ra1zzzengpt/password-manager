#pragma once

#include <QDialog>

#include <controllers/configuration_controller.hpp>
#include <controllers/main_controller.hpp>


class SettingsDialog final : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(MainController& controller,
                            ConfigurationController& configuration_controller, QApplication& app,
                            QWidget* parent = nullptr);

private:
    MainController& controller_;
    ConfigurationController& configuration_controller_;
    QApplication& app_;
};
