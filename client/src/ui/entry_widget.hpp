#pragma once

#include <QStackedWidget>
#include <QWidget>
#include <QSoundEffect>

#include <controllers/main_controller.hpp>

#include "controllers/sound_controller.hpp"

class EntryWidget final : public QWidget
{
    Q_OBJECT

    // todo will be add SoundController or SoundLibrary for playing any sound on screen-widget
    public:
        explicit EntryWidget(MainController& controller, QWidget* parent = nullptr);

    signals:
        void unlocked();

    private:
        MainController& controller_;
};
