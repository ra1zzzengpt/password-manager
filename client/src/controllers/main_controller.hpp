#pragma once
#include <memory>

#include "storage_controller.hpp"

class Logs;


class MainController final
{

public:
    explicit MainController(Logs& logs);
    ~MainController() = default;
    MainController(const MainController&) = delete;
    MainController(MainController&&) = delete;
    MainController& operator=(const MainController&) = delete;
    MainController& operator=(MainController&&) = delete;

    const std::vector<Service>& getServices();

    std::expected<void, err::Error> setMasterPassword(const std::string& password);
    std::expected<void, err::Error> changeMasterPassword(const std::string& old_password, const std::string& password);
    std::expected<void, err::Error> loadStorage();
    std::expected<void, err::Error> deleteStorage();

    std::expected<void, err::Error> addService(const Service& service);
    std::expected<void, err::Error> removeService(const std::size_t& index);
    std::expected<void, err::Error> rewriteService(const std::string& name, const std::string& login, const std::string& password, const std::size_t& index);
private:
    /*
     * Before adding tests (if they will?)
     * We can store original version of object
     * we currently use :)
    */
    StorageController storageController_;
};
