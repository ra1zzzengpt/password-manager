#pragma once
#include <atomic>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "network_controller.hpp"
#include "storage_controller.hpp"

class Logs;


class MainController final
{

public:
    explicit MainController(const std::string& host, const std::string& port, Logs& logs);
    ~MainController() = default;
    MainController(const MainController&) = delete;
    MainController(MainController&&) = delete;
    MainController& operator=(const MainController&) = delete;
    MainController& operator=(MainController&&) = delete;

    const std::unordered_map<std::uint32_t, Service>& getServices();

    std::expected<void, err::Error> setMasterPassword(const std::string& password);
    std::expected<void, err::Error> changeMasterPassword(const std::string& old_password, const std::string& password);
    std::expected<void, err::Error> loadStorage();
    std::expected<void, err::Error> deleteStorage();

    std::expected<std::uint32_t, err::Error> addService(const Service& service);
    std::expected<void, err::Error> removeService(const std::size_t& index);
    std::expected<void, err::Error> rewriteService(const Service& service, const std::size_t& index);

    std::expected<void, err::Error> importCSV(const std::string& file_path);
    std::expected<void, err::Error> exportCSV();

    std::expected<std::uint32_t, err::Error> registration(const std::uint32_t& password_hash);
    std::expected<bool, err::Error> login(std::uint32_t account_id, std::uint32_t password_hash);
    std::expected<bool, err::Error> askVault();
    std::expected<void, err::Error> fetchVault();
    std::expected<bool, err::Error> newVault();

    [[nodiscard]] std::uint32_t accountId() const noexcept;
    [[nodiscard]] std::uint32_t passwordHash() const noexcept;
    [[nodiscard]] std::uint32_t vaultVersion() const noexcept;
private:
    /*
     * Before adding tests (if they will?)
     * We can store original version of object
     * we currently use :)
    */
    std::uint32_t account_id_{};
    std::uint32_t password_hash_{};
    std::atomic<std::uint32_t> vault_version_{};
    StorageController storageController_;
    NetworkController networkController_;
};
