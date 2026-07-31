#pragma once
#include <expected>
#include <qstring.h>
#include <vector>

#include "crypto/sodium.hpp"
#include "domain/service.hpp"
#include "domain/error/error.hpp"


class StorageController final
{
public:
    // ----------------- OBJ --------------------------
    StorageController() = default;

    ~StorageController() = default;

    StorageController(const StorageController &) = delete;

    StorageController &operator=(const StorageController &) = delete;

    StorageController(StorageController &&) = delete;

    StorageController &operator=(StorageController &&) = delete;

    // --------------- STORAGE -----------------------
    std::expected<void, err::Error> load();

    std::expected<void, err::Error> save();

    static std::expected<void, err::Error> del();

    // --------------- SERVICES ----------------------
    std::expected<void, err::Error> addService(const Service& service);

    std::expected<void, err::Error> removeService(const std::size_t& index);

    std::expected<void, err::Error> rewriteService(const std::string& name, const std::string& login, const std::string& password, const std::size_t& index);

    const std::vector<Service>& services();

    // --------------- SODIUM -----------------------
    std::expected<void,err::Error> setMasterPassword(const std::string& password);

private:
    std::vector<Service> services_;
    crypto::Sodium sodium_;
};
