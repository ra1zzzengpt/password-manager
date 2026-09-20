#pragma once

#include <QDialog>

#include <controllers/main_controller.hpp>

#include "controllers/sound_controller.hpp"

class SettingsDialog final : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(MainController& controller, SoundController& sound_controller, QWidget* parent = nullptr);

private:
    MainController& controller_;
    SoundController& sound_controller_;
};
