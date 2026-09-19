#include "service_card_widget.hpp"

#include <QApplication>
#include <QHBoxLayout>
#include <QMouseEvent>

QServiceCardWidget::QServiceCardWidget(const Service& service,const std::uint32_t& id, const QIcon& copy_login_icon, const QIcon& copy_password_icon, const QIcon& more_info_icon, QWidget *parent) : id_(id), QFrame(parent)
{
    QHBoxLayout* rootLayout = new QHBoxLayout(this);

    QVBoxLayout* text_layout = new QVBoxLayout();

    serviceNameLabel_ = new QLabel(this);
    serviceNameLabel_->setObjectName("serviceName");
    serviceNameLabel_->setAttribute(Qt::WA_TransparentForMouseEvents);
    serviceNameLabel_->setText(service.name.c_str());

    loginLabel_ = new QLabel(this);
    loginLabel_->setObjectName("serviceLogin");
    loginLabel_->setAttribute(Qt::WA_TransparentForMouseEvents);
    loginLabel_->setContentsMargins(0,0,0,0);
    loginLabel_->setText(service.login.c_str());

    text_layout->addWidget(serviceNameLabel_);
    text_layout->addWidget(loginLabel_);

    text_layout->setContentsMargins(5,5,0,5);

    rootLayout->addLayout(text_layout);

    loginCopyButton_ = new QPushButton(this);
    loginCopyButton_->setIcon(copy_login_icon);

    passwordCopyButton_ = new QPushButton(this);
    passwordCopyButton_->setIcon(copy_password_icon);

    moreInfoButton_ = new QPushButton(this);
    moreInfoButton_->setIcon(more_info_icon);

    connect(loginCopyButton_, &QPushButton::clicked, [this]{emit loginCopyButtonClicked(id_);});
    connect(passwordCopyButton_, &QPushButton::clicked, [this]{emit passwordCopyButtonClicked(id_);});
    connect(moreInfoButton_, &QPushButton::clicked, [this]{emit moreInfoButtonClicked(id_);});

    rootLayout->addStretch();
    rootLayout->addWidget(loginCopyButton_);
    rootLayout->addStretch();
    rootLayout->addWidget(passwordCopyButton_);
    rootLayout->addStretch();
    rootLayout->addWidget(moreInfoButton_);
    rootLayout->addStretch();
}

void QServiceCardWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit serviceClicked(id_);
        event->accept();
        return;
    }
    QFrame::mousePressEvent(event);
}

void QServiceCardWidget::setServiceNameLabel(const QString &text)
{
    serviceNameLabel_->setText(text);
}

void QServiceCardWidget::setLoginLabel(const QString &text)
{
    loginLabel_->setText(text);
}