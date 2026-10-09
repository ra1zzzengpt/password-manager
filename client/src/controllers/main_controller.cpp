#include <controllers/main_controller.hpp>

MainController::MainController(const std::string& host, const std::string& port, Logs& logs)
    : storageController_(logs, vault_version_), networkController_(host,port)
{
}

const std::unordered_map<std::uint32_t, Service>& MainController::getServices()
{
    return storageController_.services();
}

std::expected<void, err::Error> MainController::setMasterPassword(const std::string& password)
{
    return storageController_.setMasterPassword(password);
}

std::expected<void, err::Error> MainController::changeMasterPassword(const std::string& old_password, const std::string &password)
{
    return storageController_.changeMasterPassword(old_password, password);
}

std::expected<void, err::Error> MainController::loadStorage()
{
    return storageController_.load();
}

std::expected<void, err::Error> MainController::deleteStorage()
{
    return storageController_.del();
}

std::expected<std::uint32_t, err::Error> MainController::addService(const Service &service)
{
    return storageController_.addService(service);
}

std::expected<void, err::Error> MainController::removeService(const std::size_t& index)
{
    return storageController_.removeService(index);
}

std::expected<void, err::Error> MainController::rewriteService(const Service& service, const std::size_t& index)
{
    return storageController_.rewriteService(service, index);
}

std::expected<void, err::Error> MainController::importCSV(const std::string& file_path) {
    return storageController_.importCSV(file_path);
}

std::expected<void, err::Error> MainController::exportCSV() {
    return storageController_.exportCSV();
}

std::expected<std::uint32_t, err::Error> MainController::registration(const std::uint32_t& password_hash)
{
    auto result = networkController_.registration(password_hash);
    if (result)
    {
        account_id_ = result.value();
        password_hash_ = password_hash;
    }
    return result;
}

std::expected<bool, err::Error> MainController::login(const std::uint32_t account_id,
                                                       const std::uint32_t password_hash)
{
    auto result = networkController_.login(account_id, password_hash);
    if (result && result.value())
    {
        account_id_ = account_id;
        password_hash_ = password_hash;
    }
    return result;
}

std::expected<bool, err::Error> MainController::askVault()
{
    return networkController_.ask_vault(account_id_, password_hash_, vault_version_.load());
}

std::expected<void, err::Error> MainController::fetchVault()
{
    const auto vault = networkController_.fetch_vault(account_id_, password_hash_, vault_version_.load());
    if (!vault)
        return std::unexpected{vault.error()};
    return storageController_.replaceVault(vault.value());
}

std::expected<bool, err::Error> MainController::newVault()
{
    const auto vault = storageController_.encryptedVault();
    if (!vault)
        return std::unexpected{vault.error()};
    return networkController_.new_vault(account_id_, password_hash_, vault_version_.load(), vault.value());
}

std::uint32_t MainController::accountId() const noexcept
{
    return account_id_;
}

std::uint32_t MainController::passwordHash() const noexcept
{
    return password_hash_;
}

std::uint32_t MainController::vaultVersion() const noexcept
{
    return vault_version_.load();
}
