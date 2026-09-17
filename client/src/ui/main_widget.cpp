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
void MainWidget::refresh()
{
    // deleting current widgets
    QLayoutItem* item;
    while ((item = container_layout_->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    for (std::size_t i = 0; i < controller_.getServices().size(); ++i) {
        Service service = controller_.getServices()[i];
        QWidget* serviceWidget = serviceToWidget(
            QString(service.name.c_str()),
            QString(service.login.c_str()),
            QString(service.password.c_str()),
            i
        );
        container_layout_->addWidget(serviceWidget);
    }
}

QWidget* MainWidget::serviceToWidget(const QString &name, const QString &login, const QString &password, const std::size_t index)
{
    QIcon edit_icon = QIcon(":/assets/icons/edit.png");
    QIcon apply_icon = QIcon(":/assets/icons/apply.png");
    QIcon copy_icon = QIcon(":/assets/icons/copy.png");
    QIcon delete_icon = QIcon(":/assets/icons/delete.png");

    QWidget* widget = new QWidget;

    QHBoxLayout* layout = new QHBoxLayout(widget);
    widget->setLayout(layout);

    QLabel* name_label = new QLabel(cutString(name), widget);
    name_label->setFixedWidth(150);

    QLineEdit* name_input = new QLineEdit(name, widget);
    name_input->setVisible(false);

    QLabel* login_label = new QLabel(cutString(login), widget);
    login_label->setFixedWidth(250);

    QLineEdit* login_input = new QLineEdit(login, widget);
    login_input->setVisible(false);

    QPushButton* copy_button_login = new QPushButton(widget);
    copy_button_login->setIcon(copy_icon);

    QLabel* password_label = new QLabel(cutString(QString(password.length(),'*')), widget);

    QLineEdit* password_input = new QLineEdit(password, widget);
    password_input->setVisible(false);

    QPushButton* copy_button_password = new QPushButton(widget);
    copy_button_password->setIcon(copy_icon);

    QLabel* level_password = new QLabel(widget);
    EntropyLevel level = Generator::Entropy(password.toStdString());
    if (level == EntropyLevel::Low)
    {
        level_password->setText("🔴");
    } else if (level == EntropyLevel::Medium)
    {
        level_password->setText("🟡");
    } else
    {
        level_password->setText("🟢");
    }

    QCheckBox* visible_checkbox = new QCheckBox("👁", widget);

    QPushButton* rewrite_button = new QPushButton(widget);
    rewrite_button->setObjectName("edit");
    rewrite_button->setIcon(edit_icon);

    QPushButton* delete_button = new QPushButton(widget);
    delete_button->setIcon(delete_icon);
    delete_button->setObjectName("dangerButton");

    connect(visible_checkbox, &QCheckBox::toggled, [=](const bool checked)
    {
        if (checked)
        {
            password_label->setText(cutString(password));
        } else
        {
            password_label->setText(cutString(QString(password.length(),'*')));
        }
    });

    connect(rewrite_button, &QPushButton::clicked, [=, this]
    {
        if (rewrite_button->objectName() == "edit")
        {
            rewrite_button->setIcon(apply_icon);
            rewrite_button->setObjectName("apply");

            name_label->setVisible(false);
            login_label->setVisible(false);
            password_label->setVisible(false);

            name_input->setVisible(true);
            login_input->setVisible(true);
            password_input->setVisible(true);

            password_input->setEchoMode(QLineEdit::Normal);
            return;
        }
        if (rewrite_button->objectName() == "apply")
        {
            if (const auto res = controller_.rewriteService(
                name_input->text().toStdString(),
                login_input->text().toStdString(),
                password_input->text().toStdString(),
                index); !res.has_value())
            {
                error_->setText(QString(res.error().message.c_str()));
                return;
            }
            refresh();
        }
    });

    connect(delete_button, &QPushButton::clicked, [this, index]()
    {
        if (const QMessageBox::StandardButton answer = QMessageBox::question(
            this, "Deleting", "Are you sure you want to remove this service?"); answer == QMessageBox::Yes)
        {
            if (const std::expected<void, err::Error> rm_res = controller_.removeService(index); !rm_res.has_value())
            {
                error_->setText(rm_res.error().message.c_str());
                return;
            }
            refresh();
        }
    });

    connect(copy_button_password, &QPushButton::clicked, [=]()->void
    {
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setText(password);
    });

    connect(copy_button_login, &QPushButton::clicked, [=]()->void {
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setText(login);
    });

    layout->addWidget(name_label);
    layout->addWidget(name_input);

    layout->addWidget(login_label);
    layout->addWidget(login_input);
    layout->addWidget(copy_button_login);

    layout->addWidget(password_label);
    layout->addWidget(password_input);
    layout->addWidget(copy_button_password);

    layout->addStretch();

    layout->addWidget(level_password);
    layout->addWidget(visible_checkbox);
    layout->addWidget(rewrite_button);
    layout->addWidget(delete_button);
    return widget;
}

void MainWidget::addService(const Service &service) {
    QWidget* serviceWidget = serviceToWidget(
            QString(service.name.c_str()),
            QString(service.login.c_str()),
            QString(service.password.c_str()),
            controller_.nextServiceIndex());
    container_layout_->addWidget(serviceWidget);
}
