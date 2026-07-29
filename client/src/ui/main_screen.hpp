#pragma once

#include <QWidget>
#include <QStackedWidget>

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

    QWidget* build(QWidget*& root, QStackedWidget*& stack);

private:
    MainController& controller_;

    // todo error type
    static QWidget* addService(const QString& name, const QString& login, const QString& password);
};
