#pragma once
#include <QFrame>
#include <QPushButton>
#include <QLabel>

#include "controllers/main_controller.hpp"
#include "controllers/sound_controller.hpp"
#include "domain/service.hpp"

class QServiceCardWidget : public QFrame
{
    Q_OBJECT
public:
    explicit QServiceCardWidget(MainController& controller, const std::uint32_t& id, const QIcon& copy_login_icon, const QIcon& copy_password_icon, const QIcon& more_info_icon, SoundController& sound_controller, QWidget *parent = nullptr);

    void setServiceNameLabel(const QString& text);
    void setLoginLabel(const QString& text);

signals:
    void loginCopyButtonClicked(std::uint32_t id);
    void passwordCopyButtonClicked(std::uint32_t id);
    void moreInfoButtonClicked(std::uint32_t id);

public slots:
    void updateService(const Service& service);

protected:
    void mousePressEvent(QMouseEvent *event) override;
private:
    std::uint32_t id_;
    MainController& controller_;
    SoundController& sound_controller_;

    QLabel* serviceNameLabel_;
    QLabel* loginLabel_;
    QLabel* createdAtLabel_;
    QPushButton* loginCopyButton_;
    QPushButton* passwordCopyButton_;
    QPushButton* moreInfoButton_;
};
