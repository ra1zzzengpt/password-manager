#include "add_service_dialog.hpp"

#include <expected>
#include <iostream>
#include <QCheckBox>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QSpinBox>
#include <QMenu>
#include <QRadioButton>

#include "domain/service.hpp"
#include "domain/error/error.hpp"
#include "generate/generator.hpp"

#include "custom_list_widget.hpp"

AddServiceDialog::AddServiceDialog(MainController& controller, QWidget *parent) : controller_(controller), QDialog(parent){

    this->setFixedWidth(900);
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

    password_layout->addWidget(password_label);
    password_layout->addWidget(password_input);

    // ------------------------ OPTIONS ----------------------------
    QHBoxLayout* options = new QHBoxLayout();

    QCheckBox* generating_checkbox = new QCheckBox("Generate");
    generating_checkbox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    QWidget* generation_options_widget = new QWidget(this);
    generation_options_widget->setVisible(false);
    QGridLayout* generation_options = new QGridLayout(generation_options_widget);
    generation_options->setContentsMargins(0, 0, 0, 0);
    generation_options->setHorizontalSpacing(10);
    generation_options->setVerticalSpacing(6);

    QRadioButton* symbols_option = new QRadioButton("Symbols", this);
    symbols_option->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    QRadioButton* seed_phrase = new QRadioButton("Seed phrase", this);
    seed_phrase->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    QComboBox* generation_combo_box = new QComboBox(this);
    generation_combo_box->addItems({"Low", "Medium", "High"});
    generation_combo_box->setCurrentIndex(1);
    generation_combo_box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // gen box (visible false)
    // todo spin sound
    QSpinBox* generate_box = new QSpinBox(this);
    generate_box->setMinimum(8);
    generate_box->setMaximum(500);
    generate_box->setValue(8);
    generate_box->setSingleStep(1);
    generate_box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QWidget* symbols_generation_options_widget = new QWidget(this);
    QHBoxLayout* symbols_generation_options = new QHBoxLayout(symbols_generation_options_widget);
    symbols_generation_options->setContentsMargins(0, 0, 0, 0);
    symbols_generation_options->addWidget(new QLabel("Level:", this));
    symbols_generation_options->addWidget(generation_combo_box, 1);
    symbols_generation_options->addWidget(new QLabel("Length:", this));
    symbols_generation_options->addWidget(generate_box, 1);

    // gen box for seed
    // todo spin sound
    QSpinBox* generate_box_seed = new QSpinBox(this);
    generate_box_seed->setMinimum(3);
    generate_box_seed->setMaximum(50);
    generate_box_seed->setValue(3);
    generate_box_seed->setSingleStep(1);
    generate_box_seed->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QLineEdit* separator_line_edit = new QLineEdit(this);
    separator_line_edit->setPlaceholderText("separator...");
    separator_line_edit->setText("-");
    separator_line_edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QWidget* seed_generation_options_widget = new QWidget(this);
    QHBoxLayout* seed_generation_options = new QHBoxLayout(seed_generation_options_widget);
    seed_generation_options->setContentsMargins(0, 0, 0, 0);
    seed_generation_options->addWidget(new QLabel("Words:", this));
    seed_generation_options->addWidget(generate_box_seed, 1);
    seed_generation_options->addWidget(new QLabel("Separator:", this));
    seed_generation_options->addWidget(separator_line_edit, 2);

    symbols_generation_options_widget->setVisible(false);
    seed_generation_options_widget->setVisible(false);

    generation_options->addWidget(symbols_option, 0, 0, Qt::AlignLeft);
    generation_options->addWidget(symbols_generation_options_widget, 0, 1);
    generation_options->addWidget(seed_phrase, 1, 0, Qt::AlignLeft);
    generation_options->addWidget(seed_generation_options_widget, 1, 1);
    generation_options->setColumnStretch(1, 1);

    options->addWidget(generating_checkbox, 0, Qt::AlignLeft | Qt::AlignTop);
    options->addWidget(generation_options_widget, 1);

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
        password_label->setVisible(!checked);
        password_input->setVisible(!checked);
        generation_options_widget->setVisible(checked);

        if (checked)
        {
            symbols_option->setChecked(true);
            symbols_generation_options_widget->setVisible(true);
            seed_generation_options_widget->setVisible(false);
        } else
        {
            symbols_generation_options_widget->setVisible(false);
            seed_generation_options_widget->setVisible(false);
        }
    });

    connect(symbols_option, &QRadioButton::toggled, [=](const bool checked)
    {
        symbols_generation_options_widget->setVisible(generating_checkbox->isChecked() && checked);
    });

    connect(seed_phrase, &QRadioButton::toggled, [=](const bool checked)
    {
        seed_generation_options_widget->setVisible(generating_checkbox->isChecked() && checked);
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

    connect(custom_list_widget, &QCustomListWidget::optionSelected, [=,this](const QString& text) {
        QString current_name = name_input->text();
        if (name_input->text().endsWith("."))
        {
            current_name.removeLast();
        }
        name_input->setText(current_name + text);
    });

    connect(custom_list_widget_login, &QCustomListWidget::optionSelected,[=,this](const QString& text) {
        QString current_login = login_input->text();
        if (current_login.endsWith("@"))
        {
            current_login.removeLast();
        }
        login_input->setText(current_login + text);
    });

    connect(login_input, &QLineEdit::textChanged, [=,this](const QString &text) {
        if (text.endsWith("@")) {
            custom_list_widget_login->showList();
        }
    });

    connect(name_input, &QLineEdit::textChanged, [=,this](const QString &text) {
        if (text.endsWith(".")) {
            custom_list_widget->showList();
        }
    });

    // ADD BUTTON
    connect(add_button, &QPushButton::clicked, [=,this]()->void
    {
        if (!name_input->text().isEmpty()
            && !login_input->text().isEmpty()
            && (!password_input->text().isEmpty() || generating_checkbox->isChecked()))
        {
            std::string password;
            if (generating_checkbox->isChecked())
            {
                if (symbols_option->isChecked())
                {
                    password = Generator::generate_random_password(generate_box->value(), generation_level_);
                } else
                {
                    const auto result = Generator::generate_random_seed_phrase(
                        generate_box_seed->value(), separator_line_edit->text().toStdString());
                    if (!result.has_value())
                    {
                        QMessageBox::warning(this, "Generation Error", QString::fromStdString(result.error().message));
                        return;
                    }
                    password = result.value();
                }
            } else
            {
                password = password_input->text().toStdString();
            }

            const Service service{
                .name = name_input->text().toStdString(),
                .login = login_input->text().toStdString(),
                .password = password,
                .created_at = current_time()
            };
            if (const std::expected<std::uint32_t, err::Error> res_add = controller_.addService(service); !res_add.has_value())
            {
                QMessageBox::warning(this, "Save Error", QString::fromStdString(res_add.error().message));
                return;
            } else
            {
                addService(res_add.value());
            }
            name_input->clear();
            login_input->clear();
            password_input->clear();
            accept();
        }
    });
    connect(cancel_button, &QPushButton::clicked, [=,this]()->void
    {
        reject();
    });
}

std::string AddServiceDialog::current_time()
{
    return std::format("{:%Y-%m-%d %H:%M:%S}", std::chrono::zoned_time{std::chrono::current_zone(),std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now())});
}
