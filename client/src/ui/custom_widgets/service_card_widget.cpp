#include "service_card_widget.hpp"

#include <QApplication>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QMouseEvent>

#include "ui/custom_dialogs/more_info_dialog.hpp"

namespace
{
    QString cutString(const QString& str)
    {
        QString result;
        if (str.length() > 20)
        {
            result = str.left(20) + "...";
        } else
        {
            result = str;
        }
        return result;
    }
}

QServiceCardWidget::QServiceCardWidget(
    MainController& controller,
    const std::uint32_t& id,
    const QIcon& copy_login_icon,
    const QIcon& copy_password_icon,
    SoundController& sound_controller,
    QWidget *parent)
: controller_(controller),
sound_controller_(sound_controller),
id_(id),
QFrame(parent)
{
    QHBoxLayout* rootLayout = new QHBoxLayout(this);

    QVBoxLayout* text_layout = new QVBoxLayout();

    serviceNameLabel_ = new QLabel(this);
    serviceNameLabel_->setObjectName("serviceName");
    serviceNameLabel_->setAttribute(Qt::WA_TransparentForMouseEvents);
    serviceNameLabel_->setText(cutString(controller_.getServices().at(id_).name.c_str()));

    loginLabel_ = new QLabel(this);
    loginLabel_->setObjectName("serviceLogin");
    loginLabel_->setAttribute(Qt::WA_TransparentForMouseEvents);
    loginLabel_->setContentsMargins(0,0,0,0);
    loginLabel_->setText(cutString(controller_.getServices().at(id_).login.c_str()));

    text_layout->addWidget(serviceNameLabel_);
    text_layout->addWidget(loginLabel_);

    text_layout->setContentsMargins(5,5,0,5);

    rootLayout->addLayout(text_layout);

    loginCopyButton_ = new QPushButton(this);
    loginCopyButton_->setIcon(copy_login_icon);

    passwordCopyButton_ = new QPushButton(this);
    passwordCopyButton_->setIcon(copy_password_icon);

    connect(loginCopyButton_, &QPushButton::clicked, [this]
    {
        if (auto res = sound_controller_.playSound(SoundType::Click); !res.has_value())
        {
            QMessageBox::warning(this, "Warning", res.error().message.c_str());
        }
        emit loginCopyButtonClicked(id_);
    });
    connect(passwordCopyButton_, &QPushButton::clicked, [this]
    {
        if (auto res = sound_controller_.playSound(SoundType::Click); !res.has_value())
        {
            QMessageBox::warning(this, "Warning", res.error().message.c_str());
        }
        emit passwordCopyButtonClicked(id_);
    });

    rootLayout->addWidget(loginCopyButton_);
    rootLayout->addWidget(passwordCopyButton_);

    createdAtLabel_ = new QLabel(this);
    createdAtLabel_->setText(controller_.getServices().at(id_).created_at.c_str());
    rootLayout->addWidget(createdAtLabel_);
}

void QServiceCardWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (auto res = sound_controller_.playSound(SoundType::Click); !res.has_value())
        {
            QMessageBox::warning(this, "Warning", res.error().message.c_str());
        }
        QMoreInfoDialog* more_info_dialog = new QMoreInfoDialog(controller_,id_,sound_controller_,this);
        connect(more_info_dialog, &QMoreInfoDialog::updateService, this, &QServiceCardWidget::updateService);
        connect(more_info_dialog, &QMoreInfoDialog::deleteService, [this]
        {
            this->deleteLater();
        });
        more_info_dialog->exec();
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

void QServiceCardWidget::updateService(const Service &service)
{
    serviceNameLabel_->setText(service.name.c_str());
    loginLabel_->setText(service.login.c_str());
    createdAtLabel_->setText(service.created_at.c_str());
}
