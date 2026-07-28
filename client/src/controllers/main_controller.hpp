#pragma once
#include <memory>

#include "storage_controller.hpp"


class MainController
{

public:
    MainController();
    ~MainController() = default;
    MainController(const MainController&) = delete;
    MainController(MainController&&) = delete;
    MainController& operator=(const MainController&) = delete;
    MainController& operator=(MainController&&) = delete;

    const std::vector<Service>& getServices();



private:
    /*
     * Before adding tests (if they will?)
     * We can store original version of object
     * we currently use :)
    */
    StorageController storageController_;
};
