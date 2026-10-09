//
// Created by devnull on 16.09.2026.
//

#include "window.hpp"

#include <QMessageBox>
#include <QPushButton>
#include <QThread>

#include "entry_widget.hpp"
#include "main_widget.hpp"
#include "constants/paths.hpp"
#include <ui/settings_dialog.hpp>

#include "network_entry_widget.hpp"
#include "vault_asker.hpp"

MainWindow::MainWindow(MainController &controller,
                       ConfigurationController& configuration_controller, QApplication& app)
    : controller_(controller),
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
    settings_button->setVisible(false);

    top_layout->addWidget(exit_button);
    top_layout->addStretch();
    top_layout->addWidget(label);
    top_layout->addStretch();
    top_layout->addWidget(settings_button);

    reconnect_button_ = new QPushButton("Reconnect", this);
    reconnect_button_->setVisible(false);

    // QStackedWidget* stack = new QStackedWidget(this);

    NetworkEntryWidget* network_entry_widget = new NetworkEntryWidget(controller_,this);

    // EntryWidget* entry_widget = new EntryWidget(controller_,this); // stack
    // MainWidget* main_widget = new MainWidget(controller_,sound_controller_,this);

    // stack->addWidget(entry_widget);
    // stack->addWidget(main_widget);

    // connect(entry_widget, &EntryWidget::unlocked, main_widget, &MainWidget::refresh);

    connect(network_entry_widget, &NetworkEntryWidget::loggedIn, [=,this]
    {
        network_entry_widget->deleteLater();
        EntryWidget* entry_widget = new EntryWidget(controller_,this);
        connect(entry_widget, &EntryWidget::unlocked, this, [this,rootLayout,entry_widget]()
        {
            entry_widget->deleteLater();
            main_widget_ = new MainWidget(controller_,this);
            main_widget_->refresh();
            rootLayout->addWidget(main_widget_);
            asker();
        });
        rootLayout->addWidget(entry_widget);
        settings_button->setVisible(true);
    });

    connect(exit_button, &QPushButton::clicked, [this]()->void
    {
        stopAsker();
        window()->close();
    });

    connect(settings_button, &QPushButton::clicked, [&]()->void
    {
        SettingsDialog dialog{controller_,configuration_controller_, app_, this};
        dialog.exec();
    });

    connect(this, &MainWindow::askedSuccessfully, this, [this]
    {
        fetchVault();
    });

    connect(this, &MainWindow::connectionRefused, this, [this, settings_button]
    {
        stopAsker();
        reconnect_button_->setVisible(true);
        settings_button->setEnabled(false);
        if (main_widget_) main_widget_->setEnabled(false);
    });

    connect(reconnect_button_, &QPushButton::clicked, this, [this, settings_button]
    {
        const auto result = controller_.askVault();
        if (!result)
        {
            QMessageBox::warning(this, "Warning", QString::fromStdString(result.error().message));
            return;
        }
        if (!result.value() && !fetchVault())
            return;
        reconnect_button_->setVisible(false);
        settings_button->setEnabled(true);
        if (main_widget_)
            main_widget_->setEnabled(true);
        if (asker_thread_ && asker_thread_->isRunning())
        {
            asker_thread_->quit();
            asker_thread_->wait();
        }
        asker();
    });

    rootLayout->addLayout(top_layout);
    rootLayout->addWidget(reconnect_button_);
    rootLayout->addWidget(network_entry_widget);
}

bool MainWindow::fetchVault()
{
    const auto result = controller_.fetchVault();
    if (!result)
    {
        QMessageBox::warning(this, "Warning", QString::fromStdString(result.error().message));
        emit connectionRefused();
        return false;
    }
    if (main_widget_)
        main_widget_->refresh();
    return true;
}

MainWindow::~MainWindow()
{
    if (!asker_thread_ || !asker_thread_->isRunning())
        return;

    emit stopAskerRequested();
    asker_thread_->quit();
    asker_thread_->wait();
}

void MainWindow::asker()
{
    if (asker_thread_ && asker_thread_->isRunning())
        return;

    auto* thread = new QThread(this);
    auto* worker = new VaultAsker(controller_);
    asker_thread_ = thread;
    asker_worker_ = worker;
    worker->moveToThread(thread);

    connect(thread, &QThread::started, worker, &VaultAsker::start);
    connect(this, &MainWindow::stopAskerRequested, worker, &VaultAsker::stop,
            Qt::QueuedConnection);
    connect(worker, &VaultAsker::newerVaultFound,
            this, &MainWindow::askedSuccessfully);
    connect(worker, &VaultAsker::connectionRefused,
            this, &MainWindow::connectionRefused);
    connect(worker, &VaultAsker::stopped, thread, &QThread::quit,
            Qt::DirectConnection);
    connect(thread, &QThread::finished, worker, &QObject::deleteLater);
    connect(thread, &QThread::finished, this, [this, thread]
    {
        if (asker_thread_ == thread)
        {
            asker_thread_ = nullptr;
            asker_worker_ = nullptr;
        }
        thread->deleteLater();
    });

    thread->start();
}

void MainWindow::stopAsker()
{
    if (!asker_thread_ || !asker_thread_->isRunning())
        return;

    emit stopAskerRequested();
}
