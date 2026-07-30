#include "main_screen.hpp"

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QScrollArea>
#include <QClipboard>
#include <generate/generator.hpp>
#include <utils/transform.hpp>

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
// 🗑️

MainScreen::MainScreen(MainController &controller) : controller_(controller) { }

QWidget* MainScreen::build(QWidget*& root, QStackedWidget*& stack)
{
    QWidget* widget = new QWidget(root);

    QVBoxLayout* layout = new QVBoxLayout(widget);

    // ------------ TOP LAYOUT -----------
    QHBoxLayout* top_layout = new QHBoxLayout();

    QPushButton* exit_button = new QPushButton("Exit", widget);

    QLabel* label = new QLabel("password-manager", widget);
    QFont font = label->font();
    font.setPointSize(24);
    font.setBold(true);
    label->setFont(font);

    QPushButton* change_button = new QPushButton("Settings", widget);

    top_layout->addWidget(exit_button);
    top_layout->addStretch();
    top_layout->addWidget(label);
    top_layout->addStretch();
    top_layout->addWidget(change_button);
    // ------------- FONT --------------
    QFont boldFont;
    boldFont.setBold(true);

    QFont lightFont;
    lightFont.setPointSize(10);

    // ----------- LAYOUT FOR NAME ---------------
    QHBoxLayout* name_layout = new QHBoxLayout();
    QLabel* name_label = new QLabel("Service name:", widget);
    QLineEdit* name_input = new QLineEdit(widget);
    name_input->setPlaceholderText("name...");
    name_label->setFont(boldFont);
    name_layout->addWidget(name_label);
    name_layout->addWidget(name_input);

    // ----------- LAYOUT FOR LOGIN ---------------
    QHBoxLayout* login_layout = new QHBoxLayout();
    QLabel* login_label = new QLabel("Login:", widget);
    QLineEdit* login_input = new QLineEdit(widget);
    login_input->setPlaceholderText("login...");
    login_label->setFont(boldFont);
    login_layout->addWidget(login_label);
    login_layout->addWidget(login_input);

    // ----------- LAYOUT FOR PASSWORD (ADDING) ---------
    QHBoxLayout* password_layout = new QHBoxLayout();
    QLabel* password_label = new QLabel("Password:", widget);
    QLineEdit* password_input = new QLineEdit(widget);
    password_input->setPlaceholderText("password...");
    password_input->setEchoMode(QLineEdit::Password);
    password_label->setFont(boldFont);
    password_layout->addWidget(password_label);
    password_layout->addWidget(password_input);

    // ----------- LAYOUT FOR PASSWORD (GENERATING) -------
    QSpinBox* generate_box = new QSpinBox(widget);
    generate_box->setMinimum(8);
    generate_box->setMaximum(500);
    generate_box->setValue(8);
    generate_box->setSingleStep(1);
    generate_box->setVisible(false);
    password_layout->addWidget(generate_box);

    // ------------ CHECKBOX LAYOUT ------------------
    QHBoxLayout* checkbox_layout = new QHBoxLayout();
    QCheckBox* generating_checkbox = new QCheckBox("Generate");
    checkbox_layout->addWidget(generating_checkbox);
    // TODO: MAKE MORE FLAGS

    // ------------ ADD BUTTON ------------------
    QPushButton* add_button = new QPushButton("Add Service", widget);

    QLabel* error = new QLabel(widget);

    // ------------ SCROLL AREA -----------------
    QScrollArea* scroll_area = new QScrollArea(widget);
    scroll_area->setWidgetResizable(true);
    QWidget *container = new QWidget;
    QVBoxLayout *containerLayout = new QVBoxLayout(container);
    containerLayout->setAlignment(Qt::AlignTop);
    containerLayout->setSpacing(5);
    scroll_area->setWidget(container);
    const std::function<void()> update_area = [this, containerLayout]() {
        QLayoutItem* item;
        while ((item = containerLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }

        for (const Service& service : controller_.getServices()) {
            QWidget* serviceWidget = addService(
                QString(service.name.c_str()),
                QString(service.login.c_str()),
                QString(service.password.c_str())
            );
            containerLayout->addWidget(serviceWidget);
        }
    };

    // ------------- CONNECTS -------------
    QObject::connect(exit_button, &QPushButton::clicked, [&root]()->void
    {
        root->close();
    });

    QObject::connect(generating_checkbox, &QCheckBox::toggled, [=](const bool checked)
    {
        password_input->setVisible(!checked);
        generate_box->setVisible(checked);
    });

    QObject::connect(add_button, &QPushButton::clicked, [=,this]()->void
    {
        if (!name_input->text().isEmpty()
            && !login_input->text().isEmpty()
            && (!password_input->text().isEmpty() || generating_checkbox->isChecked()))
        {
            if (generating_checkbox->isChecked())
            {
                const Service service{
                    .name = name_input->text().toStdString(), .login = login_input->text().toStdString(),
                    .password = Generator::generate_random_password(transform<uint32_t>(generate_box->text().toStdString()).value())
                };
                if (const std::expected<void, err::Error> res_add = controller_.addService(service); !res_add.has_value())
                {
                    error->setText(QString(res_add.error().message.c_str()));
                }
            } else
            {
                if (const std::expected<void, err::Error> res_add = controller_.addService(Service{name_input->text().toStdString(),login_input->text().toStdString(),password_input->text().toStdString()}); !res_add.has_value())
                {
                    error->setText(QString(res_add.error().message.c_str()));
                }
            }
        }
    });
    layout->addLayout(top_layout);
    layout->addLayout(name_layout);
    layout->addLayout(login_layout);
    layout->addLayout(password_layout);
    layout->addLayout(checkbox_layout);
    layout->addWidget(add_button);
    layout->addWidget(error);
    layout->addWidget(scroll_area);

    update_area();
    return widget;
}

QWidget* MainScreen::addService(const QString &name, const QString &login,const QString &password)
{
    QWidget* widget = new QWidget;

    QHBoxLayout* layout = new QHBoxLayout(widget);
    widget->setLayout(layout);

    QLabel* label_label = new QLabel(name, widget);

    QLabel* login_label = new QLabel(login, widget);

    QLabel* password_label = new QLabel(cutString(QString(password.length(),'*')), widget);

    // todo level of password

    QCheckBox* visible_checkbox = new QCheckBox("👁", widget);

    QPushButton* copy_button = new QPushButton("Copy", widget);

    QObject::connect(visible_checkbox, &QCheckBox::toggled, [=](const bool checked)
    {
        if (checked)
        {
            password_label->setText(cutString(password));
        } else
        {
            password_label->setText(cutString(QString(password.length(),'*')));
        }
    });

    QObject::connect(copy_button, &QPushButton::clicked, [=]()->void
    {
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setText(password);
    });

    layout->addWidget(label_label);
    layout->addWidget(login_label);
    layout->addWidget(password_label);
    layout->addStretch();
    layout->addWidget(visible_checkbox);
    layout->addWidget(copy_button);
    return widget;
}
