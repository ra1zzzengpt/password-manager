#include "main_widget.hpp"

#include <iostream>
#include <QApplication>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QScrollArea>
#include <QClipboard>
#include <QMessageBox>
#include <QStandardItemModel>
#include <generate/generator.hpp>
#include <utils/transform.hpp>

#include "add_service_dialog.hpp"
#include "custom_widgets/service_card_widget.hpp"

namespace
{
    QString cutString(const QString& str)
    {
        QString result;
        if (str.length() > 30)
        {
            result = str.left(30) + "...";
        } else
        {
            result = str;
        }
        return result;
    }
}

MainWidget::MainWidget(MainController &controller, QWidget* parent) : QWidget(parent), controller_(controller)
{
    QVBoxLayout* rootLayout = new QVBoxLayout(this);

    // ------------------------- SET ------------------------------------

    QIcon add_icon = QIcon(":/assets/icons/add.png");
    QPushButton* add_button = new QPushButton("Add", this);
    add_button->setIcon(add_icon);

    // ------------------------- SCROLL AREA -----------------------------
    QScrollArea* scroll_area = new QScrollArea(this);
    scroll_area->setWidgetResizable(true);

    QWidget *container = new QWidget;

    container_layout_ = new QVBoxLayout(container);
    container_layout_->setAlignment(Qt::AlignTop);
    container_layout_->setSpacing(5);

    scroll_area->setWidget(container);
    connect(add_button, &QPushButton::clicked, [&,this] {
        AddServiceDialog* service_dialog = new AddServiceDialog{controller_,this};
        connect(service_dialog, &AddServiceDialog::addService, this, &MainWidget::addService);
        service_dialog->exec();
    });

    rootLayout->addWidget(add_button);
    rootLayout->addWidget(scroll_area);
}

// todo refresh now very big data taking function full rework this
void MainWidget::load_all()
{
    for (const auto& [id,service] : controller_.getServices()) {
        QWidget* serviceWidget = serviceToWidget(service,id);
        container_layout_->addWidget(serviceWidget);
    }
}

QWidget* MainWidget::serviceToWidget(const Service& service, const std::uint32_t id)
{
    QIcon edit_icon = QIcon(":/assets/icons/edit.png");
    QIcon copy_login_icon = QIcon(":/assets/icons/copy_login.png");
    QIcon copy_password_icon = QIcon(":/assets/icons/copy_password.png");
    QServiceCardWidget* serviceWidget = new QServiceCardWidget(service,id,copy_login_icon,copy_password_icon,edit_icon,this);

    connect(serviceWidget, &QServiceCardWidget::loginCopyButtonClicked, [this](const std::uint32_t& id) {
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setText(controller_.getServices().at(id).login.c_str());
    });

    connect(serviceWidget, &QServiceCardWidget::passwordCopyButtonClicked, [this](const std::uint32_t& id) {
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setText(controller_.getServices().at(id).password.c_str());
    });

    connect(serviceWidget, &QServiceCardWidget::moreInfoButtonClicked, [this](const std::uint32_t& id) {
        // todo add more info call
    });

    connect(serviceWidget, &QServiceCardWidget::serviceClicked, [this](const std::uint32_t& id)
    {
        // todo add more info call
    });
    return serviceWidget;
}

void MainWidget::addService(const Service &service, const std::uint32_t id) {
    QWidget* serviceWidget = serviceToWidget(service, id);
    container_layout_->addWidget(serviceWidget);
}
