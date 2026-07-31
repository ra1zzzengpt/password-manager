#include <controllers/main_controller.hpp>

MainController::MainController() : storageController_(StorageController{}) {}

const std::vector<Service>& MainController::getServices()
{
    return storageController_.services();
}

std::expected<void, err::Error> MainController::setMasterPassword(const std::string& password)
{
    return storageController_.setMasterPassword(password);
}

std::expected<void, err::Error> MainController::loadStorage()
{
    return storageController_.load();
}

std::expected<void, err::Error> MainController::deleteStorage()
{
    return StorageController::del();
}

std::expected<void, err::Error> MainController::addService(const Service &service)
{
    return storageController_.addService(service);
}

std::expected<void, err::Error> MainController::removeService(const std::size_t& index)
{
    return storageController_.removeService(index);
}

std::expected<void, err::Error> MainController::rewriteService(const std::string& name, const std::string& login, const std::string& password, const std::size_t& index)
{
    return storageController_.rewriteService(name, login, password, index);
}