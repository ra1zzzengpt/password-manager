#pragma once
#include <QDialog>

#include "controllers/sound_controller.hpp"
#include "domain/service.hpp"
#include <QEvent>

#include "controllers/main_controller.hpp"

class QMoreInfoDialog : public QDialog
{
    Q_OBJECT
public:
    explicit QMoreInfoDialog(MainController& controller, const std::uint32_t& id, SoundController& sound_controller, QWidget* parent = nullptr);

signals:
    void updateService(const Service& service);

private:
    std::uint32_t id_;
    MainController& controller_;
    SoundController& sound_controller_;
};
