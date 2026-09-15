#pragma once

#include <QStackedWidget>
#include <QWidget>
#include <QSoundEffect>

#include <controllers/main_controller.hpp>

class PWScreen final : public QWidget
{
    Q_OBJECT

    // todo will be add SoundController or SoundLibrary for playing any sound on screen-widget
    public:
        explicit PWScreen(MainController& controller, QSoundEffect* type_sound, QWidget* parent = nullptr);

    signals:
        void unlocked();

    private:
        MainController& controller_;
};
