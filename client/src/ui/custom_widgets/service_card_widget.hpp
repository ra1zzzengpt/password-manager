#pragma once
#include <QFrame>
#include <QPushButton>
#include <QLabel>

#include "domain/service.hpp"

class QServiceCardWidget : public QFrame
{
    Q_OBJECT
public:
    explicit QServiceCardWidget(const Service& service,const std::uint32_t& id, const QIcon& copy_login_icon, const QIcon& copy_password_icon, const QIcon& more_info_icon, QWidget *parent = nullptr);

    void setServiceNameLabel(const QString& text);
    void setLoginLabel(const QString& text);

signals:
    void serviceClicked(std::uint32_t id);

    void loginCopyButtonClicked(std::uint32_t id);
    void passwordCopyButtonClicked(std::uint32_t id);
    void moreInfoButtonClicked(std::uint32_t id);

protected:
    void mousePressEvent(QMouseEvent *event) override;
private:
    std::uint32_t id_;
    QLabel* serviceNameLabel_;
    QLabel* loginLabel_;
    QPushButton* loginCopyButton_;
    QPushButton* passwordCopyButton_;
    QPushButton* moreInfoButton_;
};
