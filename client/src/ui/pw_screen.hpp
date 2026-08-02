#pragma once

#include <QStackedWidget>
#include <QWidget>

#include <controllers/main_controller.hpp>

class PWScreen final : public QWidget
{
    Q_OBJECT

    public:
        explicit PWScreen(MainController& controller, QWidget* parent = nullptr);

    signals:
        void unlocked();

    private:
        MainController& controller_;
};
