#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <ui/settings_screen.hpp>

SettingsScreen::SettingsScreen(MainController &controller, QWidget *parent) : QWidget(parent), controller_(controller)
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* password_label = new QLabel("Change master-password",this);
    password_label->setAlignment(Qt::AlignCenter);
    password_label->setObjectName("subTitle");

    QLineEdit* old_password = new QLineEdit(this);
    old_password->setPlaceholderText("old master-password...");
    old_password->setEchoMode(QLineEdit::Password);

    QLineEdit* new_password = new QLineEdit(this);
    new_password->setPlaceholderText("new master-password...");
    new_password->setEchoMode(QLineEdit::Password);

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

    connect(password_button, &QPushButton::clicked,new_password,&QLineEdit::returnPressed);

    connect(back_button, &QPushButton::clicked, [this]{done();});

    layout->addStretch();
    layout->addWidget(password_label);
    layout->addWidget(old_password);
    layout->addWidget(new_password);
    layout->addWidget(error);
    layout->addLayout(low_layout);
    layout->addStretch();
}
