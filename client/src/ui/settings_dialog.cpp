#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGroupBox>
#include <ui/settings_dialog.hpp>

SettingsDialog::SettingsDialog(MainController &controller, QWidget *parent) : QDialog(parent), controller_(controller)
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    QGroupBox* password_group = new QGroupBox("Change master-password",this);
    password_group->setAlignment(Qt::AlignCenter);

    QVBoxLayout* password_group_layout = new QVBoxLayout(password_group);

    QLineEdit* old_password = new QLineEdit(this);
    old_password->setPlaceholderText("old master-password...");
    old_password->setMinimumSize(QSize(450,14));
    old_password->setEchoMode(QLineEdit::Password);


    QLineEdit* new_password = new QLineEdit(this);
    new_password->setPlaceholderText("new master-password...");
    new_password->setEchoMode(QLineEdit::Password);

    password_group_layout->addWidget(old_password);
    password_group_layout->addWidget(new_password);

    QLabel* error = new QLabel(this);
    error->setAlignment(Qt::AlignCenter);
    error->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    error->setObjectName("error");

    QHBoxLayout* low_layout = new QHBoxLayout();

    QPushButton* back_button = new QPushButton("Back",this);
    QPushButton* password_button = new QPushButton("Change",this);

    low_layout->addWidget(back_button);
    low_layout->addStretch();
    low_layout->addWidget(password_button);

    connect(new_password, &QLineEdit::returnPressed, [=,this]()
    {
        error->clear();
        if (const std::expected<void, err::Error> set_res = controller_.changeMasterPassword(old_password->text().toStdString(),new_password->text().toStdString()); !set_res.has_value())
        {
            error->setText(QString{set_res.error().message.c_str()});
        } else
        {
            new_password->clear();
            old_password->clear();
        }
    });

    connect(password_button, &QPushButton::clicked,new_password, &QLineEdit::returnPressed);


    connect(back_button, &QPushButton::clicked,[this]
    {
        this->close();
    });

    layout->addStretch();
    layout->addWidget(password_group);
    layout->addWidget(error);
    layout->addLayout(low_layout);
    layout->addStretch();
}
