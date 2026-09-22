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
#include <QFileDialog>
#include <QLayoutItem>
#include <utils/transform.hpp>

#include "add_service_dialog.hpp"
#include "custom_widgets/service_card_widget.hpp"

MainWidget::MainWidget(MainController &controller, SoundController& sound_controller, QWidget* parent) : QWidget(parent), controller_(controller), sound_controller_(sound_controller)
{
    QVBoxLayout* rootLayout = new QVBoxLayout(this);

    // ------------------------- SET ------------------------------------

    QHBoxLayout* serv_layout = new QHBoxLayout();

    QIcon add_icon = QIcon(":/assets/icons/add.png");
    QPushButton* add_button = new QPushButton("Add", this);
    add_button->setIcon(add_icon);

    QIcon import_icon = QIcon(":/assets/icons/import.png");
    QPushButton* import_button = new QPushButton(this);
    import_button->setIcon(import_icon);
    import_button->setMaximumWidth(import_button->height() + 20);

    QIcon export_icon = QIcon(":/assets/icons/export.png");
    QPushButton* export_button = new QPushButton(this);
    export_button->setIcon(export_icon);
    export_button->setMaximumWidth(export_button->height() + 20);

    serv_layout->addWidget(add_button);
    serv_layout->addWidget(import_button);
    serv_layout->addWidget(export_button);

    // ------------------------- SCROLL AREA -----------------------------
    QScrollArea* scroll_area = new QScrollArea(this);
    scroll_area->setWidgetResizable(true);

    QWidget *container = new QWidget;

    container_layout_ = new QVBoxLayout(container);
    container_layout_->setAlignment(Qt::AlignTop);
    container_layout_->setSpacing(5);

    scroll_area->setWidget(container);
    connect(add_button, &QPushButton::clicked, [&,this] {
        if (auto res = sound_controller_.playSound(SoundType::Click); !res.has_value())
        {
            QMessageBox::warning(this, "Warning", res.error().message.c_str());
        }
        AddServiceDialog* service_dialog = new AddServiceDialog{controller_,sound_controller_,this};
        connect(service_dialog, &AddServiceDialog::addService, this, &MainWidget::addService);
        service_dialog->exec();
    });

    rootLayout->addLayout(serv_layout);
    rootLayout->addWidget(scroll_area);

    connect(import_button, &QPushButton::clicked, [this] {
        if (auto res = sound_controller_.playSound(SoundType::Click); !res.has_value())
        {
            QMessageBox::warning(this, "Warning", res.error().message.c_str());
        }
        QString filepath = QFileDialog::getOpenFileName(this, "Choose csv", "/home","*.csv");
        if (filepath.isEmpty()) {
            return;
        }
        if (auto res = controller_.importCSV(filepath.toStdString()); !res.has_value()) {
            QMessageBox::warning(this, "Warning", res.error().message.c_str());
            return;
        }
        refresh();
        QMessageBox::information(this, "Information", "Success");
    });

    connect(export_button, &QPushButton::clicked, [this] {
        if (auto res = sound_controller_.playSound(SoundType::Click); !res.has_value())
        {
            QMessageBox::warning(this, "Warning", res.error().message.c_str());
        }
        if (auto res = controller_.exportCSV(); !res.has_value()) {
            QMessageBox::warning(this, "Warning", res.error().message.c_str());
            return;
        }
        QMessageBox::information(this, "Information", "Success.\n Saved to assets/export/export.csv");
    });
}

// todo refresh now very big data taking function full rework this
void MainWidget::refresh()
{
    QLayoutItem* item;
    while ((item = container_layout_->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    for (const auto& [id,service] : controller_.getServices()) {
        QWidget* serviceWidget = serviceToWidget(id);
        container_layout_->addWidget(serviceWidget);
    }
}

QWidget* MainWidget::serviceToWidget(const std::uint32_t id)
{
    QIcon edit_icon = QIcon(":/assets/icons/edit.png");
    QIcon copy_login_icon = QIcon(":/assets/icons/copy_login.png");
    QIcon copy_password_icon = QIcon(":/assets/icons/copy_password.png");
    QServiceCardWidget* serviceWidget = new QServiceCardWidget(controller_,id,copy_login_icon,copy_password_icon,edit_icon,sound_controller_,this);

    connect(serviceWidget, &QServiceCardWidget::loginCopyButtonClicked, [this](const std::uint32_t& id) {
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setText(controller_.getServices().at(id).login.c_str());
    });

    connect(serviceWidget, &QServiceCardWidget::passwordCopyButtonClicked, [this](const std::uint32_t& id) {
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setText(controller_.getServices().at(id).password.c_str());
    });
    return serviceWidget;
}

void MainWidget::addService(const std::uint32_t id) {
    QWidget* serviceWidget = serviceToWidget(id);
    container_layout_->addWidget(serviceWidget);
}
