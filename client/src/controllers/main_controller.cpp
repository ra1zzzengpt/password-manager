#include <controllers/main_controller.hpp>

MainController::MainController(Logs& logs) : storageController_(logs) {}

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

std::uint32_t MainController::nextServiceIndex() {
    return storageController_.nextServiceIndex();
}

std::expected<void, err::Error> MainController::importCSV(const std::string& file_path) {
    return storageController_.importCSV(file_path);
}

std::expected<void, err::Error> MainController::exportCSV() {
    return storageController_.exportCSV();
}