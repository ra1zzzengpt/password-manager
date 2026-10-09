//
// Created by devnull on 09.10.2026.
//

#include "network_entry_widget.hpp"

#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

#include "utils/transform.hpp"

NetworkEntryWidget::NetworkEntryWidget(MainController& controller, QWidget *parent) : mainController_(controller), QWidget(parent)
{
    QVBoxLayout* rootLayout = new QVBoxLayout(this);

    QLabel* main_label = new QLabel("Server auth",this);
    main_label->setObjectName("headerTitle");
    main_label->setAlignment(Qt::AlignCenter);

    QHBoxLayout* radiobutton_layout = new QHBoxLayout();
    radiobutton_layout->addStretch();
    QRadioButton* registration_radiobutton = new QRadioButton("Registration",this);
    radiobutton_layout->addWidget(registration_radiobutton);

    QRadioButton* login_radiobutton = new QRadioButton("Login",this);
    login_radiobutton->setChecked(true);
    radiobutton_layout->addWidget(login_radiobutton);
    radiobutton_layout->addStretch();

    QHBoxLayout* account_layout = new QHBoxLayout();

    QLabel* account_label = new QLabel("Account ID:",this);
    account_layout->addWidget(account_label);

    QLineEdit* account_id_line_edit = new QLineEdit(this);
    account_id_line_edit->setPlaceholderText("Account ID...");
    account_id_line_edit->setValidator(new QRegularExpressionValidator(QRegularExpression{"[0-9]*"}, account_id_line_edit));
    account_layout->addWidget(account_id_line_edit);

    QHBoxLayout* password_layout = new QHBoxLayout();

    QLabel* password_label = new QLabel("Password:",this);
    password_layout->addWidget(password_label);

    QLineEdit* password_line_edit = new QLineEdit(this);
    password_line_edit->setPlaceholderText("Password...");
    password_line_edit->setEchoMode(QLineEdit::Password);
    password_layout->addWidget(password_line_edit);

    QPushButton* register_button = new QPushButton("Register new account",this);
    QPushButton* login_button = new QPushButton("Login in your account",this);
    login_button->setVisible(false);

    connect(registration_radiobutton, &QRadioButton::toggled, [=](const bool& checked)
    {
        account_label->setVisible(!checked);
        account_id_line_edit->setVisible(!checked);
        register_button->setVisible(checked);
        login_button->setVisible(!checked);
    });

    connect(login_radiobutton, &QRadioButton::toggled, [=](const bool& checked)
    {
        account_label->setVisible(checked);
        account_id_line_edit->setVisible(checked);
        register_button->setVisible(!checked);
        login_button->setVisible(checked);
    });

    connect(register_button, &QPushButton::clicked, [=,this]()
    {
        const std::hash<std::string> hasher;
        const std::size_t password_hash = hasher(password_line_edit->text().toStdString());
        if (auto result = mainController_.registration(password_hash); !result.has_value())
        {
            QMessageBox::warning(this, "Warning", QString::fromStdString(result.error().message));
            return;
        }
        emit loggedIn();
    });

    connect(login_button, &QPushButton::clicked, [=,this]()
    {
        const std::hash<std::string> hasher;
        const std::size_t password_hash = hasher(password_line_edit->text().toStdString());
        const auto account_id = transform<std::uint32_t>(account_id_line_edit->text().toStdString());
        if (!account_id)
        {
            QMessageBox::warning(this, "Warning", QString::fromStdString(account_id.error().message));
            return;
        }

        const auto result = mainController_.login(account_id.value(), password_hash);
        if (!result)
        {
            QMessageBox::warning(this, "Warning", QString::fromStdString(result.error().message));
            return;
        }
        if (!result.value())
        {
            QMessageBox::warning(this, "Warning", "Invalid account ID or password");
            return;
        }
        emit loggedIn();
    });

    rootLayout->addStretch();
    rootLayout->addWidget(main_label);
    rootLayout->addLayout(radiobutton_layout);
    rootLayout->addLayout(account_layout);
    rootLayout->addLayout(password_layout);
    rootLayout->addWidget(register_button);
    rootLayout->addWidget(login_button);
    rootLayout->addStretch();
}
