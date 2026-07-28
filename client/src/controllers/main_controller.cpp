#include <controllers/main_controller.hpp>

MainController::MainController() : storageController_(StorageController{}) {}

const std::vector<Service>& MainController::getServices()
{
    return storageController_.services();
}
