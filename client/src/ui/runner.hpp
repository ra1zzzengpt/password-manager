#pragma once
#include <controllers/main_controller.hpp>

class Runner final
{
public:
    explicit Runner(MainController& controller);
    ~Runner() = default;
    Runner(const Runner &) = delete;
    Runner &operator=(const Runner &) = delete;
    Runner(Runner &&) = delete;
    Runner &operator=(Runner &&) = delete;

    void run();

private:
    MainController& controller_;
};
