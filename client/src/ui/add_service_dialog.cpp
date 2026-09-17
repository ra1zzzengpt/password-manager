#include "add_service_dialog.hpp"

#include <expected>
#include <QCheckBox>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QPushButton>
#include <QMessageBox>
#include <QSpinBox>
#include <QToolButton>
#include <QMenu>

#include "domain/service.hpp"
#include "domain/error/error.hpp"
#include "generate/generator.hpp"
#include <utils/transform.hpp>

#include "custom_list_widget.hpp"

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

AddServiceDialog::AddServiceDialog(MainController& controller, QWidget *parent) : controller_(controller), QDialog(parent){

    QVBoxLayout* layout = new QVBoxLayout(this);

    QGroupBox* group_box = new QGroupBox("Add service", this);

    QVBoxLayout* group_layout = new QVBoxLayout(group_box);

    QIcon mail_icon = QIcon(":assets/icons/mail.png");

    // ------------------------------- NAME LAYOUT  ------------------------------
    QHBoxLayout* name_layout = new QHBoxLayout();

    QLabel* name_label = new QLabel("Service name:", this);

    QLineEdit* name_input = new QLineEdit(this);
    name_input->setPlaceholderText("service name...");

    QCustomListWidget* custom_list_widget = new QCustomListWidget(mail_icon,this);
    custom_list_widget->addOptions({".com",".xyz",".ai",".cn",".ru"});

    name_layout->addWidget(name_label);
    name_layout->addWidget(name_input);
    name_layout->addWidget(custom_list_widget);

    // ----------- LAYOUT FOR LOGIN ---------------
    QHBoxLayout* login_layout = new QHBoxLayout();

    QLabel* login_label = new QLabel("Login:", this);

    QLineEdit* login_input = new QLineEdit(this);
    login_input->setPlaceholderText("login...");

    // todo position check

    QComboBox* login_combo_box = new QComboBox(this);
    login_combo_box->setPlaceholderText("mail");
    login_combo_box->addItems({"@gmail.com","@protonmail.com","@yandex.ru","@outlook.com","@yahoo.com"});

    login_layout->addWidget(login_label);
    login_layout->addWidget(login_input);
    login_layout->addWidget(login_combo_box);

    // ------------------------ PASSWORD LAYOUT -----------------------------
    QHBoxLayout* password_layout = new QHBoxLayout();

    QLabel* password_label = new QLabel("Password:", this);
    password_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    QLineEdit* password_input = new QLineEdit(this);
    password_input->setPlaceholderText("password...");
    password_input->setEchoMode(QLineEdit::Password);

    // gen box (visible false)
    QSpinBox* generate_box = new QSpinBox(this);
    generate_box->setMinimum(8);
    generate_box->setMaximum(500);
    generate_box->setValue(8);
    generate_box->setSingleStep(1);
    generate_box->setVisible(false);

    password_layout->addWidget(password_label);
    password_layout->addWidget(password_input);
    password_layout->addWidget(generate_box);

    // ------------------------ OPTIONS ----------------------------
    QHBoxLayout* options = new QHBoxLayout();

    QCheckBox* generating_checkbox = new QCheckBox("Generate");
    generating_checkbox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    QComboBox* generation_combo_box = new QComboBox(this);
    generation_combo_box->addItems({"Low", "Medium", "High"});
    generation_combo_box->setCurrentIndex(1);

    options->addWidget(generating_checkbox);
    options->addWidget(generation_combo_box);
    // TODO: MAKE MORE FLAGS

    // ---------------------- ADD BUTTON ---------------------------
    QPushButton* add_button = new QPushButton("Add Service", this);

    group_layout->addLayout(name_layout);
    group_layout->addLayout(login_layout);
    group_layout->addLayout(password_layout);

    group_layout->addLayout(options);
    group_layout->addWidget(add_button);

    layout->addWidget(group_box);


    // ----------------------------- CONNECTS -----------------------------

    // GENERATION CHECKBOX
    connect(generating_checkbox, &QCheckBox::toggled, [=](const bool checked)
    {
        password_input->setVisible(!checked);
        generate_box->setVisible(checked);
    });


    // COMBO BOX
    connect(generation_combo_box, &QComboBox::currentIndexChanged, [=,this](const int index)
    {
        switch (index)
        {
            case 0:
                generation_level_ = GenerationLevel::Low;
                generate_box->setMinimum(8);
                generate_box->setValue(8);
                break;
            case 1:
                generation_level_ = GenerationLevel::Medium;
                generate_box->setMinimum(8);
                generate_box->setValue(8);
                break;
            case 2:
                generation_level_ = GenerationLevel::High;
                generate_box->setMinimum(16);
                generate_box->setValue(16);
                break;
            default:
                generation_level_ = GenerationLevel::Medium;
        }
    });

    // todo парсить по строке при нажатии . вылезает список (кнопка скрытая смещается)

    connect(custom_list_widget, &QCustomListWidget::optionSelected, [=](const QString& text) {
        QString current_name = name_input->text();
        if (name_input->text().endsWith("."))
        {
            current_name.removeLast();
        }
        name_input->setText(current_name + text);
    });

    connect(login_combo_box, &QComboBox::currentIndexChanged,[=] {
        QString current_login = login_input->text();
        if (current_login.endsWith("@"))
        {
            current_login.removeLast();
        }
        login_input->setText(current_login + login_combo_box->currentText());
        login_combo_box->setCurrentIndex(-1);
    });

    connect(login_input, &QLineEdit::textChanged, [=](const QString &text) {
        if (text.endsWith("@")) {
            login_combo_box->showPopup();
        }
    });

    // connect(name_input, &QLineEdit::textChanged, [=](const QString &text) {
    //     if (text.endsWith(".")) {
    //         name_combo_box->showPopup();
    //     }
    // });

    // ADD BUTTON
    connect(add_button, &QPushButton::clicked, [=,this]()->void
    {
        if (!name_input->text().isEmpty()
            && !login_input->text().isEmpty()
            && (!password_input->text().isEmpty() || generating_checkbox->isChecked()))
        {
            Service service;
            if (generating_checkbox->isChecked())
            {
                service = Service{
                    .name = name_input->text().toStdString(), .login = login_input->text().toStdString(),
                    .password = Generator::generate_random_password(transform<uint32_t>(generate_box->text().toStdString()).value(), generation_level_)
                };
                if (const std::expected<void, err::Error> res_add = controller_.addService(service); !res_add.has_value())
                {
                    QMessageBox::warning(this, "Save Error",QString(res_add.error().message.c_str()));
                    return;
                }
            } else
            {
                service = Service{name_input->text().toStdString(),login_input->text().toStdString(),password_input->text().toStdString()};
                if (const std::expected<void, err::Error> res_add = controller_.addService(service); !res_add.has_value())
                {
                    QMessageBox::warning(this, "Save Error",QString(res_add.error().message.c_str()));
                    return;
                }
            }
            name_input->clear();
            login_input->clear();
            password_input->clear();
            addService(service);
        }
    });
}