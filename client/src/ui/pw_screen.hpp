#pragma once

#include <QWidget>

#include <controllers/main_controller.hpp>

class PWScreen final
{
public:
    explicit PWScreen(MainController& controller);
    ~PWScreen() = default;
    PWScreen(const PWScreen&) = delete;
    PWScreen(PWScreen&&) = delete;
    PWScreen& operator=(const PWScreen&) = delete;
    PWScreen& operator=(PWScreen&&) = delete;

    QWidget* build(QWidget* root, int32_t& screen_index);

private:
    MainController& controller_;
};
