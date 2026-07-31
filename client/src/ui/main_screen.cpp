#include "main_screen.hpp"

#include <QApplication>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QScrollArea>
#include <QClipboard>
#include <QMessageBox>
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
// 🗑

MainScreen::MainScreen(MainController &controller, QWidget* parent) : QWidget(parent), controller_(controller)
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    // ------------ TOP LAYOUT -----------
    QHBoxLayout* top_layout = new QHBoxLayout();

    QPushButton* exit_button = new QPushButton("Exit", this);

    QLabel* label = new QLabel("password-manager", this);
    QFont font = label->font();
    font.setPointSize(24);
    font.setBold(true);
    label->setFont(font);

    QPushButton* change_button = new QPushButton("Settings", this);

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
    QLabel* name_label = new QLabel("Service name:", this);
    QLineEdit* name_input = new QLineEdit(this);
    name_input->setPlaceholderText("name...");
    name_label->setFont(boldFont);
    name_layout->addWidget(name_label);
    name_layout->addWidget(name_input);

    // ----------- LAYOUT FOR LOGIN ---------------
    QHBoxLayout* login_layout = new QHBoxLayout();
    QLabel* login_label = new QLabel("Login:", this);
    QLineEdit* login_input = new QLineEdit(this);
    login_input->setPlaceholderText("login...");
    login_label->setFont(boldFont);
    login_layout->addWidget(login_label);
    login_layout->addWidget(login_input);

    // ----------- LAYOUT FOR PASSWORD (ADDING) ---------
    QHBoxLayout* password_layout = new QHBoxLayout();
    QLabel* password_label = new QLabel("Password:", this);
    password_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    QLineEdit* password_input = new QLineEdit(this);
    password_input->setPlaceholderText("password...");
    password_input->setEchoMode(QLineEdit::Password);
    password_label->setFont(boldFont);
    password_layout->addWidget(password_label);
    password_layout->addWidget(password_input);

    // ----------- LAYOUT FOR PASSWORD (GENERATING) -------
    QSpinBox* generate_box = new QSpinBox(this);
    generate_box->setMinimum(8);
    generate_box->setMaximum(500);
    generate_box->setValue(8);
    generate_box->setSingleStep(1);
    generate_box->setVisible(false);
    password_layout->addWidget(generate_box);

    // ------------ CHECKBOX LAYOUT ------------------
    QHBoxLayout* checkbox_layout = new QHBoxLayout();
    QCheckBox* generating_checkbox = new QCheckBox("Generate");

    // todo create work with generating level
    QComboBox* generation_combo_box = new QComboBox(this);
    generation_combo_box->addItems({"Low", "Medium", "High", "Maximum"});
    generation_combo_box->setCurrentIndex(1);

    checkbox_layout->addWidget(generating_checkbox);
    checkbox_layout->addWidget(generation_combo_box);
    // TODO: MAKE MORE FLAGS

    // ------------ ADD BUTTON ------------------
    QPushButton* add_button = new QPushButton("Add Service", this);

    error_ = new QLabel(this);

    // ------------ SCROLL AREA -----------------
    QScrollArea* scroll_area = new QScrollArea(this);
    scroll_area->setWidgetResizable(true);
    QWidget *container = new QWidget;
    container_layout_ = new QVBoxLayout(container);
    container_layout_->setAlignment(Qt::AlignTop);
    container_layout_->setSpacing(5);
    scroll_area->setWidget(container);

    // ------------- CONNECTS -------------
    connect(exit_button, &QPushButton::clicked, [this]()->void
    {
        window()->close();
    });

    connect(generating_checkbox, &QCheckBox::toggled, [=](const bool checked)
    {
        password_input->setVisible(!checked);
        generate_box->setVisible(checked);
    });

    connect(generation_combo_box, &QComboBox::currentIndexChanged, [this](const int index)
    {
        switch (index)
        {
            case 0:
                generation_level_ = GenerationLevel::Low;
                break;
            case 1:
                generation_level_ = GenerationLevel::Medium;
                break;
            case 2:
                generation_level_ = GenerationLevel::High;
                break;
            case 3:
                generation_level_ = GenerationLevel::Maximum;
                break;
            default:
                generation_level_ = GenerationLevel::Medium;
        }
    });

    connect(add_button, &QPushButton::clicked, [=,this]()->void
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
                    error_->setText(QString(res_add.error().message.c_str()));
                    return;
                }
            } else
            {
                if (const std::expected<void, err::Error> res_add = controller_.addService(Service{name_input->text().toStdString(),login_input->text().toStdString(),password_input->text().toStdString()}); !res_add.has_value())
                {
                    error_->setText(QString(res_add.error().message.c_str()));
                    return;
                }
            }
            name_input->clear();
            login_input->clear();
            password_input->clear();
        }
        refresh();
    });

    connect(change_button, &QPushButton::clicked, [this]()->void
    {
        QMessageBox::information(this,"Information","WORK IN PROGRESS");
    });

    layout->addLayout(top_layout);
    layout->addLayout(name_layout);
    layout->addLayout(login_layout);
    layout->addLayout(password_layout);
    layout->addLayout(checkbox_layout);
    layout->addWidget(add_button);
    layout->addWidget(error_);
    layout->addWidget(scroll_area);
}

void MainScreen::refresh()
{
    QLayoutItem* item;
    while ((item = container_layout_->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    for (std::size_t i = 0; i < controller_.getServices().size(); ++i) {
        Service service = controller_.getServices()[i];
        QWidget* serviceWidget = serviceToWidget(
            QString(service.name.c_str()),
            QString(service.login.c_str()),
            QString(service.password.c_str()),
            i
        );
        container_layout_->addWidget(serviceWidget);
    }
}

// todo check const quality
QWidget* MainScreen::serviceToWidget(const QString &name, const QString &login, const QString &password, const std::size_t index)
{
    QWidget* widget = new QWidget;

    QHBoxLayout* layout = new QHBoxLayout(widget);
    widget->setLayout(layout);

    QLabel* label_label = new QLabel(name, widget);

    QLineEdit* name_input = new QLineEdit(name, widget);
    name_input->setVisible(false);

    QLabel* login_label = new QLabel(login, widget);

    QLineEdit* login_input = new QLineEdit(login, widget);
    login_input->setVisible(false);

    QLabel* password_label = new QLabel(cutString(QString(password.length(),'*')), widget);

    QLineEdit* password_input = new QLineEdit(password, widget);
    password_input->setVisible(false);

    // todo level of password

    QCheckBox* visible_checkbox = new QCheckBox("👁", widget);

    QPushButton* rewrite_button = new QPushButton("✏", widget);

    QPushButton* delete_button = new QPushButton("🗑", widget);

    QPushButton* copy_button = new QPushButton("Copy", widget);

    connect(visible_checkbox, &QCheckBox::toggled, [=](const bool checked)
    {
        if (checked)
        {
            password_label->setText(cutString(password));
        } else
        {
            password_label->setText(cutString(QString(password.length(),'*')));
        }
    });

    connect(rewrite_button, &QPushButton::clicked, [=, this] {
    if (rewrite_button->text() == "✏") {
        rewrite_button->setText("✓");
        label_label->setVisible(false);
        login_label->setVisible(false);
        password_label->setVisible(false);
        name_input->setVisible(true);
        login_input->setVisible(true);
        password_input->setVisible(true);
        password_input->setEchoMode(QLineEdit::Normal);
    }
    else if (rewrite_button->text() == "✓") {
        if (const auto res = controller_.rewriteService(
                name_input->text().toStdString(),
                login_input->text().toStdString(),
                password_input->text().toStdString(),
                index); !res.has_value()) {
            error_->setText(QString(res.error().message.c_str()));
            return;
        }
        refresh();
    }
});

    connect(delete_button, &QPushButton::clicked, [this, index]()
    {
        if (const QMessageBox::StandardButton answer = QMessageBox::question(this,"Deleting","Are you sure you want to remove this service?"); answer == QMessageBox::Yes)
        {
            if (const std::expected<void, err::Error> rm_res = controller_.removeService(index); !rm_res.has_value())
            {
                error_->setText(rm_res.error().message.c_str());
                return;
            }
            refresh();
        }
    });

    connect(copy_button, &QPushButton::clicked, [=]()->void
    {
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setText(password);
    });

    layout->addWidget(label_label);
    layout->addWidget(name_input);
    layout->addWidget(login_label);
    layout->addWidget(login_input);
    layout->addWidget(password_label);
    layout->addWidget(password_input);
    layout->addStretch();
    layout->addWidget(visible_checkbox);
    layout->addWidget(rewrite_button);
    layout->addWidget(delete_button);
    layout->addWidget(copy_button);
    return widget;
}

