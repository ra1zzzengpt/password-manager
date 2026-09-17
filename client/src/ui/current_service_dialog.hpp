//
// Created by arch on 9/17/26.
//

#ifndef PASSWORD_MANAGER_CURRENT_SERVICE_DIALOG_HPP
#define PASSWORD_MANAGER_CURRENT_SERVICE_DIALOG_HPP

#include <QDialog>
#include <controllers/main_controller.hpp>

class CurrentServiceDialog : public QDialog {
    Q_OBJECT
public:
    explicit CurrentServiceDialog(MainController& controller, QWidget* parent = nullptr);
private:
    MainController& controller_;
};


#endif //PASSWORD_MANAGER_CURRENT_SERVICE_DIALOG_HPP
