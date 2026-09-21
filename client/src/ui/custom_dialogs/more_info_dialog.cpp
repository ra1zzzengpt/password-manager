#include "more_info_dialog.hpp"

#include <QApplication>
#include <QMessageBox>

#include "domain/service.hpp"
#include <QPushButton>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QClipboard>
#include <QLabel>
#include <QLineEdit>

#include "generate/generator.hpp"

QMoreInfoDialog::QMoreInfoDialog(MainController& controller, const std::uint32_t& id, SoundController &sound_controller, QWidget *parent)
    : QDialog(parent), sound_controller_(sound_controller), controller_(controller), id_(id)
{
    this->setFixedSize(900,350);
    QVBoxLayout* root = new QVBoxLayout(this);

    QHBoxLayout* top_layout = new QHBoxLayout;

    QIcon backIcon(":/assets/icons/exit.png");
    QPushButton* back_button = new QPushButton(this);
    back_button->setIcon(backIcon);

    QLabel* label = new QLabel(QString(controller_.getServices().at(id_).name.c_str()).toUpper(),this);

    QFontMetrics fontMetrics(label->font());

    top_layout->addWidget(back_button);
    top_layout->addStretch();
    top_layout->addWidget(label);
    top_layout->addStretch();

    QHBoxLayout* edit_layout = new QHBoxLayout();

    QIcon deleteIcon(":/assets/icons/delete.png");
    QPushButton* delete_button = new QPushButton(this);
    delete_button->setIcon(deleteIcon);
    delete_button->setObjectName("dangerButton");

    QIcon editIcon(":/assets/icons/edit.png");
    QCheckBox* edit_on = new QCheckBox("Edit",this);
    edit_on->setIcon(editIcon);

    edit_layout->addWidget(delete_button);
    edit_layout->addStretch();
    edit_layout->addWidget(edit_on);

    QHBoxLayout* name_layout = new QHBoxLayout();

    QLabel* name_label = new QLabel(this);
    name_label->setText("Name:");

    QLineEdit* name_line_edit = new QLineEdit(this);
    name_line_edit->setPlaceholderText("Service name...");
    name_line_edit->setText(controller_.getServices().at(id_).name.c_str());
    name_line_edit->setMinimumWidth(fontMetrics.maxWidth() * 15);

    name_layout->addWidget(name_label);
    name_layout->addWidget(name_line_edit);

    QHBoxLayout* login_layout = new QHBoxLayout();

    QLabel* login_label = new QLabel(this);
    login_label->setText("Login:");

    QLineEdit* login_line_edit = new QLineEdit(this);
    login_line_edit->setPlaceholderText("Service login...");
    login_line_edit->setText(controller_.getServices().at(id_).login.c_str());
    login_line_edit->setMinimumWidth(fontMetrics.maxWidth() * 15);

    QIcon copyLoginIcon(":/assets/icons/copy_login.png");
    QPushButton* copy_login_button = new QPushButton(this);
    copy_login_button->setIcon(copyLoginIcon);

    login_layout->addWidget(login_label);
    login_layout->addWidget(login_line_edit);
    login_layout->addWidget(copy_login_button);

    QHBoxLayout* password_layout = new QHBoxLayout();

    QLabel* password_label = new QLabel(this);
    password_label->setText("Password:");

    QLineEdit* password_line_edit = new QLineEdit(this);
    password_line_edit->setPlaceholderText("Service password...");
    password_line_edit->setText(controller_.getServices().at(id_).password.c_str());
    password_line_edit->setEchoMode(QLineEdit::Password);
    password_line_edit->setMinimumWidth(fontMetrics.maxWidth() * 15);

    QPushButton* level_button = new QPushButton(this);

    EntropyLevel level = Generator::Entropy(password_line_edit->text().toStdString());
    QIcon level_icon;
    if (level == EntropyLevel::High)
    {
        level_icon = QIcon(":assets/icons/happy.png");
    } else if (level == EntropyLevel::Medium)
    {
        level_icon = QIcon(":/assets/icons/neutral.png");
    } else
    {
        level_icon = QIcon(":/assets/icons/sad.png");
    }
    level_button->setIcon(level_icon);

    QIcon copyPasswordIcon(":/assets/icons/copy_password.png");
    QPushButton* copy_password_button = new QPushButton(this);
    copy_password_button->setIcon(copyPasswordIcon);

    QIcon seeIcon(":/assets/icons/eye.png");
    QCheckBox* see_password = new QCheckBox(this);
    see_password->setIcon(seeIcon);

    password_layout->addWidget(password_label);
    password_layout->addWidget(password_line_edit);
    password_layout->addWidget(level_button);
    password_layout->addWidget(copy_password_button);
    password_layout->addWidget(see_password);

    QHBoxLayout* time_layout = new QHBoxLayout();

    QLabel* created_label = new QLabel(this);
    created_label->setText("Created at:");

    QLabel* time_label = new QLabel(this);
    time_label->setText(controller_.getServices().at(id_).created_at.c_str());

    time_layout->addWidget(created_label);
    time_layout->addWidget(time_label);
    time_layout->addStretch();

    name_line_edit->setEnabled(false);
    login_line_edit->setEnabled(false);
    password_line_edit->setEnabled(false);

    QPushButton* apply_button = new QPushButton("Apply",this);

    root->addLayout(top_layout);
    root->addLayout(edit_layout);
    root->addLayout(name_layout);
    root->addLayout(login_layout);
    root->addLayout(password_layout);
    root->addLayout(time_layout);
    root->addWidget(apply_button);

    connect(copy_password_button, &QPushButton::clicked, [this]
    {
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setText(controller_.getServices().at(id_).password.c_str());
    });

    connect(copy_login_button, &QPushButton::clicked, [this]
    {
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setText(controller_.getServices().at(id_).login.c_str());
    });

    connect(back_button, &QPushButton::clicked, [this]
    {
        if (auto res = sound_controller_.playSound(SoundType::Click); !res.has_value())
        {
            QMessageBox::warning(this, "Warning", res.error().message.c_str());
        }
        this->close();
    });

    connect(edit_on, &QCheckBox::toggled, [=,this](const bool state)
    {
        if (auto res = sound_controller_.playSound(SoundType::Click); !res.has_value())
        {
            QMessageBox::warning(this, "Warning", res.error().message.c_str());
        }
        if (state)
        {
            name_line_edit->setEnabled(true);
            login_line_edit->setEnabled(true);
            password_line_edit->setEnabled(true);
        } else
        {
            name_line_edit->setEnabled(false);
            login_line_edit->setEnabled(false);
            password_line_edit->setEnabled(false);
        }
    });

    connect(name_line_edit, &QLineEdit::textChanged, [this]
    {
        if (auto res = sound_controller_.playSound(SoundType::Type); !res.has_value())
        {
            QMessageBox::warning(this, "Warning", res.error().message.c_str());
        }
    });

    connect(login_line_edit, &QLineEdit::textChanged, [this]
    {
        if (auto res = sound_controller_.playSound(SoundType::Type); !res.has_value())
        {
            QMessageBox::warning(this, "Warning", res.error().message.c_str());
        }
    });

    connect(password_line_edit, &QLineEdit::textChanged, [=,this]
    {
        if (auto res = sound_controller_.playSound(SoundType::Type); !res.has_value())
        {
            QMessageBox::warning(this, "Warning", res.error().message.c_str());
        }
        EntropyLevel lvl = Generator::Entropy(password_line_edit->text().toStdString());
        QIcon icon;
        if (lvl == EntropyLevel::High)
        {
            icon = QIcon(":assets/icons/happy.png");
        } else if (lvl == EntropyLevel::Medium)
        {
            icon = QIcon(":/assets/icons/neutral.png");
        } else
        {
            icon = QIcon(":/assets/icons/sad.png");
        }
        level_button->setIcon(icon);
    });

    connect(see_password, &QCheckBox::toggled, [=,this](const bool state)
    {
        if (auto res = sound_controller_.playSound(SoundType::Click); !res.has_value())
        {
            QMessageBox::warning(this, "Warning", res.error().message.c_str());
        }
        if (state)
        {
            password_line_edit->setEchoMode(QLineEdit::Normal);
        } else
        {
            password_line_edit->setEchoMode(QLineEdit::Password);
        }
    });
    connect(apply_button, &QPushButton::clicked, [=,this]
    {
        if (auto res = sound_controller_.playSound(SoundType::Click); !res.has_value())
        {
            QMessageBox::warning(this, "Warning", res.error().message.c_str());
        }
        Service service{.name = name_line_edit->text().toStdString(),
        .login = login_line_edit->text().toStdString(),
        .password = password_line_edit->text().toStdString(),
        .created_at = time_label->text().toStdString()};
        if (auto res = controller_.rewriteService(service,id_); !res.has_value())
        {
            QMessageBox::warning(this, "Error", res.error().message.c_str());
        } else
        {
            emit updateService(service);
        }
        this->close();
    });

    connect(level_button, &QPushButton::clicked, [this]
    {
        if (auto res = sound_controller_.playSound(SoundType::Click); !res.has_value())
        {
            QMessageBox::warning(this, "Warning", res.error().message.c_str());
        }
        QMessageBox::information(this, "Entropy information",
                                 "Entropy estimates how difficult a password is to guess.\n\n"
                                 "E = L × log₂(N), where L is the password length and N is the size "
                                 "of the detected character pool.\n\n"
                                 "Character sets: lowercase — 26, uppercase — 26, digits — 10, "
                                 "special characters — 29 (91 in total).\n\n"
                                 "Current strength levels:\n"
                                 "Low: less than 50 bits\n"
                                 "Medium: from 50 to 79 bits\n"
                                 "High: 80 bits or more");
    });

    connect(delete_button, &QPushButton::clicked, [=,this]
    {
        if (auto res = sound_controller_.playSound(SoundType::Click); !res.has_value())
        {
            QMessageBox::warning(this, "Warning", res.error().message.c_str());
        }

        if (auto result = QMessageBox::question(this, "Delete", "Are you sure you want to remove this service?"); result == QMessageBox::Yes)
        {
            if (auto res = controller_.removeService(id_); !res.has_value())
            {
                QMessageBox::warning(this, "Warning", res.error().message.c_str());
            } else
            {
                emit deleteService();
            }
            this->close();
        }
    });
}
