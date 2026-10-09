#pragma once

#include <QDialog>

#include "controllers/configuration_controller.hpp"

class QConfigFillerDialog final : public QDialog
{
public:
    explicit QConfigFillerDialog(ConfigurationController& configuration_controller,
                                 QWidget* parent = nullptr);

private:
    ConfigurationController& configuration_controller_;

protected:
    void done(int result) override;
};
