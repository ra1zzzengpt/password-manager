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
