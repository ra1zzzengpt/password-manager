#pragma once

#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <generate/generator.hpp>

#include <controllers/main_controller.hpp>

class MainWidget final : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(MainController& controller, QWidget* parent = nullptr);

public slots:
    void refresh();
    void addService(std::uint32_t id);

    void applyFilters();
private:
    MainController& controller_;

    QVBoxLayout* container_layout_ = nullptr;
    GenerationLevel generation_level_{GenerationLevel::Medium};

    QLineEdit* search_by_name_;
    QLineEdit* search_by_login_;

    [[nodiscard]] QWidget* serviceToWidget(std::uint32_t id);

    QIcon copy_login_icon_ = QIcon(":/assets/icons/copy_login.png");
    QIcon copy_password_icon_ = QIcon(":/assets/icons/copy_password.png");
};
