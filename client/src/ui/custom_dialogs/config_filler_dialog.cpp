//
// Created by devnull on 09.10.2026.
//

#include "config_filler_dialog.hpp"

#include <QCheckBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>

#include <utility>

QConfigFillerDialog::QConfigFillerDialog(ConfigurationController& configuration_controller,
                                         QWidget* parent)
    : QDialog(parent), configuration_controller_(configuration_controller)
{
    setWindowTitle("Complete configuration");
    setModal(true);
    setMinimumWidth(520);

    auto* root_layout = new QVBoxLayout(this);
    root_layout->setSizeConstraint(QLayout::SetFixedSize);

    auto* header_layout = new QHBoxLayout;
    auto* header = new QLabel(
        "The configuration file is incomplete. Please fill in the missing settings.", this);
    header->setWordWrap(true);
    header->setObjectName("subTitle");

    auto* close_button = new QPushButton(this);
    close_button->setIcon(QIcon(":/assets/icons/exit.png"));
    close_button->setIconSize(QSize(18, 18));
    close_button->setToolTip("Close");
    close_button->setAccessibleName("Close");

    header_layout->addWidget(header, 1);
    header_layout->addWidget(close_button, 0, Qt::AlignTop | Qt::AlignRight);

    auto* theme_group = new QGroupBox("Theme", this);
    auto* theme_layout = new QHBoxLayout(theme_group);

    auto* dark_theme = new QRadioButton("Dark", theme_group);
    dark_theme->setIcon(QIcon(":/assets/icons/dark_theme.png"));
    dark_theme->setIconSize(QSize(48, 48));

    auto* nord_theme = new QRadioButton("Nord", theme_group);
    nord_theme->setIcon(QIcon(":/assets/icons/nord_theme.png"));
    nord_theme->setIconSize(QSize(48, 48));

    auto* white_theme = new QRadioButton("White", theme_group);
    white_theme->setIcon(QIcon(":/assets/icons/white_theme.png"));
    white_theme->setIconSize(QSize(48, 48));

    theme_layout->addWidget(dark_theme);
    theme_layout->addWidget(nord_theme);
    theme_layout->addWidget(white_theme);

    const Config& current_config = configuration_controller_.config();
    if (current_config.theme == "nord")
        nord_theme->setChecked(true);
    else if (current_config.theme == "white")
        white_theme->setChecked(true);
    else
        dark_theme->setChecked(true);

    auto* sync_checkbox = new QCheckBox("Sync", this);
    sync_checkbox->setChecked(current_config.sync);

    auto* sync_fields = new QWidget(this);
    auto* sync_layout = new QFormLayout(sync_fields);
    sync_layout->setContentsMargins(0, 0, 0, 0);

    auto* host_edit = new QLineEdit(QString::fromStdString(current_config.host), sync_fields);
    host_edit->setPlaceholderText("localhost");

    auto* port_edit = new QLineEdit(QString::fromStdString(current_config.port), sync_fields);
    port_edit->setPlaceholderText("8080");
    port_edit->setValidator(new QIntValidator(1, 65535, port_edit));

    sync_layout->addRow("Host:", host_edit);
    sync_layout->addRow("Port:", port_edit);
    sync_fields->setVisible(sync_checkbox->isChecked());

    auto* buttons_layout = new QHBoxLayout;
    auto* delete_button = new QPushButton("Delete configuration", this);
    delete_button->setIcon(QIcon(":/assets/icons/delete.png"));
    delete_button->setObjectName("dangerButton");

    auto* continue_button = new QPushButton("Continue", this);
    continue_button->setIcon(QIcon(":/assets/icons/apply.png"));
    continue_button->setDefault(true);

    buttons_layout->addWidget(delete_button);
    buttons_layout->addStretch();
    buttons_layout->addWidget(continue_button);

    root_layout->addLayout(header_layout);
    root_layout->addWidget(theme_group);
    root_layout->addWidget(sync_checkbox);
    root_layout->addWidget(sync_fields);
    root_layout->addLayout(buttons_layout);

    connect(close_button, &QPushButton::clicked, this, &QDialog::reject);

    connect(sync_checkbox, &QCheckBox::toggled, this,
            [this, sync_checkbox, sync_fields](const bool checked)
    {
        if (!checked)
            sync_checkbox->setFocus(Qt::OtherFocusReason);
        sync_fields->setVisible(checked);
        adjustSize();
    });

    connect(delete_button, &QPushButton::clicked, this, [this]
    {
        const auto answer = QMessageBox::question(
            this,
            "Delete configuration",
            "Are you sure you want to delete the configuration file?",
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No);
        if (answer != QMessageBox::Yes)
            return;

        if (const auto result = configuration_controller_.deleteConfig(); !result)
        {
            QMessageBox::warning(this, "Configuration error",
                                 QString::fromStdString(result.error().message));
            return;
        }

        QMessageBox::information(this, "Configuration", "The configuration file was deleted.");
    });

    connect(continue_button, &QPushButton::clicked, this,
            [this, dark_theme, nord_theme, white_theme, sync_checkbox, host_edit, port_edit]
    {
        const QString host = host_edit->text().trimmed();
        const QString port = port_edit->text().trimmed();
        bool port_is_valid = false;
        const unsigned short port_number = port.toUShort(&port_is_valid);
        if (sync_checkbox->isChecked()
            && (host.isEmpty() || !port_is_valid || port_number == 0))
        {
            QMessageBox::warning(this, "Configuration error",
                                 "Enter a host and a port between 1 and 65535.");
            return;
        }

        Config updated = configuration_controller_.config();
        if (nord_theme->isChecked())
            updated.theme = "nord";
        else if (white_theme->isChecked())
            updated.theme = "white";
        else if (dark_theme->isChecked())
            updated.theme = "dark";

        updated.sync = sync_checkbox->isChecked();
        if (updated.sync)
        {
            updated.host = host.toStdString();
            updated.port = port.toStdString();
        }

        if (const auto result = configuration_controller_.setConfig(std::move(updated)); !result)
        {
            QMessageBox::warning(this, "Configuration error",
                                 QString::fromStdString(result.error().message));
            return;
        }

        accept();
    });
}

void QConfigFillerDialog::done(const int result)
{
    // Release text-input focus while the Wayland surface still exists.
    setFocus(Qt::OtherFocusReason);
    QDialog::done(result);
}
