//
// Created by devnull on 16.09.2026.
//

#include "window.hpp"

#include <QMessageBox>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QPushButton>

#include "entry_widget.hpp"
#include "main_widget.hpp"
#include "constants/paths.hpp"
#include <ui/settings_dialog.hpp>

MainWindow::MainWindow(MainController &controller, SoundController& sound_controller,
                       ConfigurationController& configuration_controller, QApplication& app)
    : controller_(controller), sound_controller_(sound_controller),
      configuration_controller_(configuration_controller), app_(app)
{
    QVBoxLayout* rootLayout = new QVBoxLayout(this);

    this->setLayout(rootLayout);

    QHBoxLayout* top_layout = new QHBoxLayout();

    QIcon exit_icon = QIcon(":/assets/icons/exit.png");

    QPushButton* exit_button = new QPushButton(this);
    exit_button->setIcon(exit_icon);
    exit_button->setIconSize(QSize(16,16));

    QLabel* label = new QLabel("password-manager", this);
    label->setObjectName("subTitle");

    QFont font = label->font();
    font.setPointSize(24);
    font.setBold(true);

    label->setFont(font);

    QIcon settings_icon = QIcon(":/assets/icons/settings.png");

    QPushButton* settings_button = new QPushButton(this);
    settings_button->setIcon(settings_icon);
    settings_button->setIconSize(QSize(16,16));

    top_layout->addWidget(exit_button);
    top_layout->addStretch();
    top_layout->addWidget(label);
    top_layout->addStretch();
    top_layout->addWidget(settings_button);

    QStackedWidget* stack = new QStackedWidget(this);

    EntryWidget* entry_widget = new EntryWidget(controller_,sound_controller_,stack);
    MainWidget* main_widget = new MainWidget(controller_,sound_controller_,stack); // todo to signals work

    stack->addWidget(entry_widget);
    stack->addWidget(main_widget);

    connect(entry_widget, &EntryWidget::unlocked, main_widget, &MainWidget::refresh);

    connect(entry_widget, &EntryWidget::unlocked, stack, [stack, main_widget]()
    {
        stack->setCurrentWidget(main_widget);
    });

    connect(exit_button, &QPushButton::clicked, [this]()->void
    {
        window()->close();
    });

    connect(settings_button, &QPushButton::clicked, [&]()->void
    {
        if (auto res = sound_controller_.playSound(SoundType::Click); !res.has_value())
        {
            QMessageBox::warning(this, "Warning", res.error().message.c_str());
        }
        SettingsDialog* dialog = new SettingsDialog(controller_, sound_controller_,
                                                    configuration_controller_, app_, this);
        dialog->exec();
    });

    rootLayout->addLayout(top_layout);
    rootLayout->addWidget(stack);
}
