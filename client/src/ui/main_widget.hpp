#pragma once

#include <QVBoxLayout>
#include <QLabel>
#include <generate/generator.hpp>

#include <controllers/main_controller.hpp>

class MainWidget final : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(MainController& controller,QWidget* parent = nullptr);

signals:
    void settings();
public slots:
    void load_all();
    void addService(const Service& service, std::uint32_t id);
private:
    MainController& controller_;
    QVBoxLayout* container_layout_ = nullptr;
    QLabel* error_ = nullptr;
    GenerationLevel generation_level_{GenerationLevel::Medium};
    // todo error type
    [[nodiscard]] QWidget* serviceToWidget(const Service& service, std::uint32_t id);
};
