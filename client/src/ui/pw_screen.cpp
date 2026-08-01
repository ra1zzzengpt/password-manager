#include <QHBoxLayout>
#include <QLabel>
#include <QFont>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <ui/pw_screen.hpp>

PWScreen::PWScreen(MainController &controller, QWidget* parent) : QWidget(parent), controller_(controller)
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* label = new QLabel("PASSWORD-MANAGER", this);
    label->setAlignment(Qt::AlignCenter);
    QFont font = label->font();
    font.setPointSize(24);
    font.setBold(true);
    label->setFont(font);

    QLabel* entry_label = new QLabel("Enter your master-password:", this);
    entry_label->setAlignment(Qt::AlignCenter);
    QFont entry_font = entry_label->font();
    entry_font.setBold(true);
    entry_label->setFont(entry_font);

    QLineEdit* password_input = new QLineEdit(this);
    password_input->setEchoMode(QLineEdit::Password);
    password_input->setPlaceholderText("Enter your password...");

    QHBoxLayout* hbox_button_layout = new QHBoxLayout();
    QPushButton* exit_button = new QPushButton("Exit", this);
    QPushButton* next_button = new QPushButton("Next", this);
    QPushButton* delete_button = new QPushButton("Delete", this);
    hbox_button_layout->addWidget(exit_button);
    hbox_button_layout->addWidget(delete_button);
    hbox_button_layout->addWidget(next_button);

    QLabel* error = new QLabel(this);
    error->setAlignment(Qt::AlignCenter);
    error->setStyleSheet("color: red;");

    connect(password_input,&QLineEdit::returnPressed,[=,this]()
    {
        error->setText(QString());
        if (const std::expected<void, err::Error> set_result = controller_.setMasterPassword(password_input->text().toStdString()); !set_result.has_value())
        {
            error->setText(QString(set_result.error().message.c_str()));
            return;
        }
        if (const std::expected<void, err::Error> load_result = controller_.loadStorage(); !load_result.has_value())
        {
            error->setText(QString(load_result.error().message.c_str()));
            return;
        }
        emit unlocked();
    });

    connect(exit_button,&QPushButton::clicked,[&]()
    {
        window()->close();
    });

    connect(delete_button,&QPushButton::clicked,[this,error]()
    {
        QMessageBox* message_box = new QMessageBox();
        message_box->setWindowTitle("Deletion confirmation");
        message_box->setText("Are you sure you want to delete all password information?");
        message_box->setInformativeText(" This action cannot be undone.");
        message_box->setDetailedText("Use this button only if you do not need the information in the database or if you have forgotten your password., Use this button only if you do not need the information in the database or if you have forgotten the password.(We CANNOT recover the password).");
        message_box->setIcon(QMessageBox::Warning);
        message_box->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        message_box->setDefaultButton(QMessageBox::No);
        if (const int ans = message_box->exec(); ans == QMessageBox::Yes)
        {
            if (const std::expected<void, err::Error> delete_res = MainController::deleteStorage(); !delete_res.has_value())
            {
                error->setText(QString(delete_res.error().message.c_str()));
                return;
            }
            QMessageBox::information(this,"Information","Successful.");
        }
    });

    connect(next_button,&QPushButton::clicked,[=,this]()
    {
        error->setText(QString());
        if (const std::expected<void, err::Error> set_result = controller_.setMasterPassword(password_input->text().toStdString()); !set_result.has_value())
        {
            error->setText(QString(set_result.error().message.c_str()));
            return;
        }
        if (const std::expected<void, err::Error> load_result = controller_.loadStorage(); !load_result.has_value())
        {
            error->setText(QString(load_result.error().message.c_str()));
            return;
        }
        emit unlocked();
    });

    layout->addStretch();
    layout->addWidget(label);
    layout->addWidget(entry_label);
    layout->addWidget(password_input);
    layout->addLayout(hbox_button_layout);
    layout->addWidget(error);
    layout->addStretch();
}
