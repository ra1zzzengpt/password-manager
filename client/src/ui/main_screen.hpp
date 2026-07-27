#pragma once

#include <QWidget>

#include <controllers/main_controller.hpp>

class MainScreen final
{
public:
    explicit MainScreen(MainController& controller);
    ~MainScreen() = default;

    MainScreen(const MainScreen&) = delete;
    MainScreen(MainScreen&&) = delete;

    MainScreen& operator=(const MainScreen&) = delete;
    MainScreen& operator=(MainScreen&&) = delete;

    QWidget* build();

private:
    MainController& controller_;
};
