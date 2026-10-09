#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <pqxx/pqxx>

struct Account
{
    std::uint32_t id;
    std::uint32_t password_hash;
    std::uint32_t vault_version;
    std::string vault_path;
};

class Database final
{
public:
    explicit Database(const std::string& database_info);

    Account createAccount(std::uint32_t password_hash);
    std::optional<Account> account(std::uint32_t id, std::uint32_t password_hash);
    bool updateVaultVersion(std::uint32_t id, std::uint32_t password_hash,
                            std::uint32_t vault_version);

private:
    pqxx::connection connection_;
};
