#pragma once
#include <expected>
#include <qstring.h>
#include <vector>
#include <unordered_map>

#include "crypto/sodium.hpp"
#include "domain/service.hpp"
#include "domain/error/error.hpp"
#include "utils/csv_parser.hpp"

class Logs;


class StorageController final
{
public:
    // ----------------- OBJ --------------------------
    explicit StorageController(Logs& logs);

    ~StorageController() = default;

    StorageController(const StorageController &) = delete;

    StorageController &operator=(const StorageController &) = delete;

    StorageController(StorageController &&) = delete;

    StorageController &operator=(StorageController &&) = delete;

    static std::uint32_t takeNextId();

    // --------------- STORAGE -----------------------
    std::expected<void, err::Error> load();

    std::expected<void, err::Error> save();

    std::expected<void, err::Error> del();

    // --------------- SERVICES ----------------------
    std::expected<std::uint32_t, err::Error> addService(const Service& service);

    std::expected<void, err::Error> removeService(const std::size_t& index);

    std::expected<void, err::Error> rewriteService(const Service& service, const std::size_t& index);

    const std::unordered_map<std::uint32_t, Service>& services();

    // --------------- CSV --------------------------

    std::expected<void, err::Error> importCSV(const std::string& file_path);

    std::expected<void, err::Error> exportCSV();

    // --------------- SODIUM -----------------------
    std::expected<void,err::Error> setMasterPassword(const std::string& password);

    std::expected<void,err::Error> changeMasterPassword(const std::string& old_password, const std::string& password);
private:
    std::unordered_map<std::uint32_t,Service> services_;
    crypto::Sodium sodium_;
    Logs& logs_;
    static inline std::uint32_t id_;
};
