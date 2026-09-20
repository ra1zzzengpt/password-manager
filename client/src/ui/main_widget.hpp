#pragma once

#include <QVBoxLayout>
#include <QLabel>
#include <generate/generator.hpp>

#include <controllers/main_controller.hpp>

#include "controllers/sound_controller.hpp"

class MainWidget final : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(MainController& controller, SoundController& sound_controller, QWidget* parent = nullptr);

public slots:
    void load_all();
    void addService(std::uint32_t id);
private:
    MainController& controller_;
    SoundController& sound_controller_;
    QVBoxLayout* container_layout_ = nullptr;
    QLabel* error_ = nullptr;
    GenerationLevel generation_level_{GenerationLevel::Medium};
    // todo error type
    [[nodiscard]] QWidget* serviceToWidget(std::uint32_t id);
};
