#pragma once
#include <expected>
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

    // --------------- SERVICES ----------------------
    std::expected<void, err::Error> addService(const Service& service);

    std::expected<void, err::Error> removeService(/* todo index or service? */);

    const std::vector<Service>& services();

private:
    std::vector<Service> services_;
    crypto::Sodium sodium_;
};
