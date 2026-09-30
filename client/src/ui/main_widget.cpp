#include "main_widget.hpp"

#include <QApplication>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QScrollArea>
#include <QClipboard>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QLayoutItem>
#include <utils/transform.hpp>

#include "add_service_dialog.hpp"
#include "custom_widgets/service_card_widget.hpp"

MainWidget::MainWidget(MainController &controller, QWidget* parent) : QWidget(parent), controller_(controller)
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

    QHBoxLayout* search_layout = new QHBoxLayout();

    search_by_name_ = new QLineEdit(this);
    search_by_name_->setPlaceholderText("service name filer...");

    search_by_login_ = new QLineEdit(this);
    search_by_login_->setPlaceholderText("login filter...");

    search_layout->addWidget(search_by_name_);
    search_layout->addWidget(search_by_login_);

    // ------------------------- SCROLL AREA -----------------------------
    QScrollArea* scroll_area = new QScrollArea(this);
    scroll_area->setWidgetResizable(true);

    QWidget *container = new QWidget;

    container_layout_ = new QVBoxLayout(container);
    container_layout_->setAlignment(Qt::AlignTop);
    container_layout_->setSpacing(5);

    scroll_area->setWidget(container);
    connect(add_button, &QPushButton::clicked, [&,this] {
        AddServiceDialog service_dialog{controller_, this};
        connect(&service_dialog, &AddServiceDialog::addService, this, &MainWidget::addService);
        service_dialog.exec();
    });

    rootLayout->addLayout(serv_layout);
    rootLayout->addLayout(search_layout);
    rootLayout->addWidget(scroll_area);

    connect(import_button, &QPushButton::clicked, [this] {
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
        if (auto res = controller_.exportCSV(); !res.has_value()) {
            QMessageBox::warning(this, "Warning", res.error().message.c_str());
            return;
        }
        QMessageBox::information(this, "Information", "Success.\n Saved to assets/export/export.csv");
    });

    connect(search_by_name_, &QLineEdit::textChanged, this, &MainWidget::applyFilters);

    connect(search_by_login_, &QLineEdit::textChanged, this, &MainWidget::applyFilters);
}

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
    applyFilters();
}

QWidget* MainWidget::serviceToWidget(const std::uint32_t id)
{
    QServiceCardWidget* serviceWidget = new QServiceCardWidget(controller_,id,copy_login_icon_,copy_password_icon_,this);
    serviceWidget->setProperty("serviceID",id);

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
    applyFilters();
}

void MainWidget::applyFilters()
{
    for (std::uint32_t i = 0; i < container_layout_->count(); ++i)
    {
        auto widget = container_layout_->itemAt(i)->widget();
        if (widget == nullptr)
        {
            continue;
        }
        std::uint32_t id = widget->property("serviceID").toUInt();
        auto service = controller_.getServices().find(id);
        if (service != controller_.getServices().end())
        {
            widget->setVisible(QString::fromStdString(service->second.name).contains(search_by_name_->text(), Qt::CaseInsensitive) && QString::fromStdString(service->second.login).contains(search_by_login_->text(), Qt::CaseInsensitive));
        } else
        {
            widget->hide();
        }
    }
}
