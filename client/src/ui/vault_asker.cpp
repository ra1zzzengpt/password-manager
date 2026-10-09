#include "vault_asker.hpp"

#include <QTimer>

#include "controllers/main_controller.hpp"

VaultAsker::VaultAsker(MainController& controller, QObject* parent) : QObject(parent), controller_(controller)
{ }

void VaultAsker::start()
{
    if (running_)
        return;

    running_ = true;
    timer_ = new QTimer(this);
    timer_->setInterval(5000);
    connect(timer_, &QTimer::timeout, this, &VaultAsker::ask);
    timer_->start();
}

void VaultAsker::stop()
{
    if (!running_)
        return;

    running_ = false;
    if (timer_)
        timer_->stop();
    emit stopped();
}

void VaultAsker::ask()
{
    if (!running_)
        return;

    const auto result = controller_.askVault();
    if (!result)
    {
        running_ = false;
        timer_->stop();
        emit connectionRefused();
        emit stopped();
        return;
    }

    if (!result.value())
    {
        emit newerVaultFound();
        return;
    }

    const auto current_version = controller_.vaultVersion();
    if (current_version == vault_version_)
        return;

    const auto upload = controller_.newVault();
    if (!upload)
    {
        running_ = false;
        timer_->stop();
        emit connectionRefused();
        emit stopped();
        return;
    }
    if (upload.value())
        vault_version_ = current_version;
}
