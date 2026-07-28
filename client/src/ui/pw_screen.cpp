#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFont>
#include <QLineEdit>
#include <QPushButton>
#include <ui/pw_screen.hpp>

PWScreen::PWScreen(MainController &controller) : controller_(controller) { }

QWidget *PWScreen::build(QWidget* root, int32_t& screen_index)
{
    QWidget *widget = new QWidget(root);
    QVBoxLayout *layout = new QVBoxLayout(widget);

    QLabel* label = new QLabel("PASSWORD-MANAGER");
    QFont font = label->font();
    font.setPointSize(24);
    font.setBold(true);
    label->setFont(font);

    QLabel* entry_label = new QLabel("Enter your master-password:");
    QFont entry_font = entry_label->font();
    entry_font.setBold(true);
    entry_label->setFont(entry_font);

    QLineEdit* password_input = new QLineEdit();
    password_input->setEchoMode(QLineEdit::Password);
    password_input->setPlaceholderText("Enter your password...");

    QHBoxLayout* hbox_button_layout = new QHBoxLayout();
    QPushButton* exit_button = new QPushButton("Exit");
    QPushButton* next_button = new QPushButton("Next");
    hbox_button_layout->addWidget(exit_button);
    hbox_button_layout->addWidget(next_button);

    QObject::connect(&password_input,&QLineEdit::returnPressed,[=]()
    {
        // todo controller check password (try decrypt if file exists or set password if non exist)
    });

    QObject::connect(exit_button,&QPushButton::clicked,[&root]()
    {
        root->close();
    });

    QObject::connect(next_button,&QPushButton::clicked,[=]()
    {
        // todo same
    });

    layout->addWidget(label);
    layout->addWidget(entry_label);
    layout->addWidget(password_input);
    layout->addLayout(hbox_button_layout);
    return widget;
}
