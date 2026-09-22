#include "add_service_dialog.hpp"

#include <expected>
#include <iostream>
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

AddServiceDialog::AddServiceDialog(MainController& controller, SoundController& sound_controller, QWidget *parent) : controller_(controller), sound_controller_(sound_controller), QDialog(parent){

    this->setFixedSize(900,280);
    QVBoxLayout* layout = new QVBoxLayout(this);

    QGroupBox* group_box = new QGroupBox("Add service", this);
    group_box->setAlignment(Qt::AlignCenter);

    QVBoxLayout* group_layout = new QVBoxLayout(group_box);

    QIcon web_icon = QIcon(":assets/icons/web.png");
    QIcon mail_icon = QIcon(":assets/icons/mail.png");

    // ------------------------------- NAME LAYOUT  ------------------------------
    QHBoxLayout* name_layout = new QHBoxLayout();

    QLabel* name_label = new QLabel("Service name:", this);
    name_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    QLineEdit* name_input = new QLineEdit(this);
    name_input->setPlaceholderText("service name...");
    name_input->setMinimumSize(250,16);
    name_input->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    QCustomListWidget* custom_list_widget = new QCustomListWidget(this);
    custom_list_widget->addOptions({".com",".xyz",".ai",".cn",".ru"});
    custom_list_widget->setIcon(web_icon);
    // custom_list_widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    custom_list_widget->setMaximumSize(50,50);

    name_layout->addWidget(name_label);
    name_layout->addWidget(name_input);
    name_layout->addWidget(custom_list_widget);

    // ----------- LAYOUT FOR LOGIN ---------------
    QHBoxLayout* login_layout = new QHBoxLayout();

    QLabel* login_label = new QLabel("Login:", this);

    QLineEdit* login_input = new QLineEdit(this);
    login_input->setPlaceholderText("login...");

    // todo position check

    QCustomListWidget* custom_list_widget_login = new QCustomListWidget(this);
    custom_list_widget_login->addOptions({"@gmail.com","@protonmail.com","@yandex.ru","@outlook.com","@yahoo.com"});
    custom_list_widget_login->setIcon(mail_icon);

    login_layout->addWidget(login_label);
    login_layout->addWidget(login_input);
    login_layout->addWidget(custom_list_widget_login);

    // ------------------------ PASSWORD LAYOUT -----------------------------
    QHBoxLayout* password_layout = new QHBoxLayout();

    QLabel* password_label = new QLabel("Password:", this);
    password_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    QLineEdit* password_input = new QLineEdit(this);
    password_input->setPlaceholderText("password...");
    password_input->setMinimumSize(QSize(350,16));
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
    QHBoxLayout* low_layout = new QHBoxLayout();

    QPushButton* cancel_button = new QPushButton("Cancel", this);
    QPushButton* add_button = new QPushButton("Add Service", this);

    low_layout->addWidget(cancel_button);
    low_layout->addWidget(add_button);

    group_layout->addLayout(name_layout);
    group_layout->addLayout(login_layout);
    group_layout->addLayout(password_layout);

    group_layout->addLayout(options);
    group_layout->addLayout(low_layout);

    layout->addWidget(group_box);


    // ----------------------------- CONNECTS -----------------------------

    // GENERATION CHECKBOX
    connect(generating_checkbox, &QCheckBox::toggled, [=,this](const bool checked)
    {
        if (auto res = sound_controller_.playSound(SoundType::Click); !res.has_value())
        {
            QMessageBox::warning(this, "Warning", res.error().message.c_str());
        }
        password_input->setVisible(!checked);
        generate_box->setVisible(checked);
    });


    // COMBO BOX
    connect(generation_combo_box, &QComboBox::currentIndexChanged, [=,this](const int index)
    {
        if (auto res = sound_controller_.playSound(SoundType::Click); !res.has_value())
        {
            QMessageBox::warning(this, "Warning", res.error().message.c_str());
        }
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

    connect(custom_list_widget, &QCustomListWidget::optionSelected, [=,this](const QString& text) {
        if (auto res = sound_controller_.playSound(SoundType::Click); !res.has_value())
        {
            QMessageBox::warning(this, "Warning", res.error().message.c_str());
        }
        QString current_name = name_input->text();
        if (name_input->text().endsWith("."))
        {
            current_name.removeLast();
        }
        name_input->setText(current_name + text);
    });

    connect(custom_list_widget_login, &QCustomListWidget::optionSelected,[=,this](const QString& text) {
        if (auto res = sound_controller_.playSound(SoundType::Click); !res.has_value())
        {
            QMessageBox::warning(this, "Warning", res.error().message.c_str());
        }
        QString current_login = login_input->text();
        if (current_login.endsWith("@"))
        {
            current_login.removeLast();
        }
        login_input->setText(current_login + text);
    });

    connect(login_input, &QLineEdit::textChanged, [=,this](const QString &text) {
        if (auto res = sound_controller_.playSound(SoundType::Type); !res.has_value())
        {
            QMessageBox::warning(this, "Warning", res.error().message.c_str());
        }
        if (text.endsWith("@")) {
            custom_list_widget_login->showList();
        }
    });

    connect(name_input, &QLineEdit::textChanged, [=,this](const QString &text) {
        if (auto res = sound_controller_.playSound(SoundType::Type); !res.has_value())
        {
            QMessageBox::warning(this, "Warning", res.error().message.c_str());
        }
        if (text.endsWith(".")) {
            custom_list_widget->showList();
        }
    });

    // ADD BUTTON
    connect(add_button, &QPushButton::clicked, [=,this]()->void
    {
        if (auto res = sound_controller_.playSound(SoundType::Click); !res.has_value())
        {
            QMessageBox::warning(this, "Warning", res.error().message.c_str());
        }
        if (!name_input->text().isEmpty()
            && !login_input->text().isEmpty()
            && (!password_input->text().isEmpty() || generating_checkbox->isChecked()))
        {
            Service service;
            if (generating_checkbox->isChecked())
            {
                service = Service{
                    .name = name_input->text().toStdString(),
                    .login = login_input->text().toStdString(),
                    .password = Generator::generate_random_password(transform<uint32_t>(generate_box->text().toStdString()).value(), generation_level_),
                    .created_at = current_time()
                };
                if (const std::expected<std::uint32_t, err::Error> res_add = controller_.addService(service); !res_add.has_value())
                {
                    QMessageBox::warning(this, "Save Error",QString(res_add.error().message.c_str()));
                    return;
                } else
                {
                    addService(res_add.value());
                }
            } else
            {
                service = Service{.name = name_input->text().toStdString(), .login = login_input->text().toStdString(),.password = password_input->text().toStdString(),
                .created_at = current_time()};
                if (const std::expected<std::uint32_t, err::Error> res_add = controller_.addService(service); !res_add.has_value())
                {
                    QMessageBox::warning(this, "Save Error",QString(res_add.error().message.c_str()));
                    return;
                } else
                {
                    addService(res_add.value());
                }
            }
            name_input->clear();
            login_input->clear();
            password_input->clear();
            this->close();
        }
    });
    connect(cancel_button, &QPushButton::clicked, [=,this]()->void
    {
        if (auto res = sound_controller_.playSound(SoundType::Click); !res.has_value())
        {
            QMessageBox::warning(this, "Warning", res.error().message.c_str());
        }
        this->close();
    });
    connect(password_input, &QLineEdit::textChanged, [=,this](const QString &)
    {
        if (auto res = sound_controller_.playSound(SoundType::Type); !res.has_value())
        {
            QMessageBox::warning(this, "Warning", res.error().message.c_str());
        }
    });
}

std::string AddServiceDialog::current_time()
{
    return std::format("{:%Y-%m-%d %H:%M:%S}", std::chrono::zoned_time{std::chrono::current_zone(),std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now())});
}
