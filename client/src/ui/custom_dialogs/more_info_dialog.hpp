#pragma once
#include <QDialog>

#include "domain/service.hpp"
#include <QEvent>

#include "controllers/main_controller.hpp"

class QMoreInfoDialog : public QDialog
{
    Q_OBJECT
public:
    explicit QMoreInfoDialog(MainController& controller, const std::uint32_t& id, QWidget* parent = nullptr);

signals:
    void updateService(const Service& service);
    void deleteService();

private:
    std::uint32_t id_;
    MainController& controller_;
};
