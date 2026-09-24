#pragma once

#include <QStackedWidget>
#include <QWidget>
#include <QSoundEffect>

#include <controllers/main_controller.hpp>

#include "controllers/sound_controller.hpp"

class EntryWidget final : public QWidget
{
    Q_OBJECT


    public:
        explicit EntryWidget(MainController& controller, SoundController& sound_controller, QWidget* parent = nullptr);

    signals:
        void unlocked();

    private:
        MainController& controller_;
        SoundController& sound_controller_;
};
