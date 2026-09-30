#pragma once

#include <QWidget>
#include <controllers/main_controller.hpp>

class EntryWidget final : public QWidget
{
    Q_OBJECT


    public:
        explicit EntryWidget(MainController& controller, QWidget* parent = nullptr);

    signals:
        void unlocked();

    private:
        MainController& controller_;
};
