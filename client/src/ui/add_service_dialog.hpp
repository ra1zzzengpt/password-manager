//
// Created by arch on 9/17/26.
//

#ifndef PASSWORD_MANAGER_ADD_SERVICE_WIDGET_HPP
#define PASSWORD_MANAGER_ADD_SERVICE_WIDGET_HPP
#include <QDialog>
#include <QWidget>

#include "generate/generator.hpp"
#include <domain/error/error.hpp>

#include "controllers/main_controller.hpp"
#include "controllers/sound_controller.hpp"


class AddServiceDialog : public QDialog {
    Q_OBJECT
public:
    explicit AddServiceDialog(MainController& controller, SoundController& sound_controller, QWidget* parent = nullptr);
signals:
    void addService(std::uint32_t id);
private:
    GenerationLevel generation_level_{GenerationLevel::Medium};
    MainController& controller_;
    SoundController& sound_controller_;

    // consteval std::format checker
    static std::string current_time();
};


#endif //PASSWORD_MANAGER_ADD_SERVICE_WIDGET_HPP
