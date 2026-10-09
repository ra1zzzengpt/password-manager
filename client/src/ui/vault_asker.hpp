#pragma once

#include <cstdint>
#include <QObject>

class MainController;
class QTimer;

class VaultAsker final : public QObject
{
    Q_OBJECT

public:
    explicit VaultAsker(MainController& controller, QObject* parent = nullptr);

public slots:
    void start();
    void stop();

signals:
    void newerVaultFound();
    void connectionRefused();
    void stopped();

private:
    MainController& controller_;
    QTimer* timer_{};
    bool running_{};
    std::uint32_t vault_version_{};

    void ask();
};
