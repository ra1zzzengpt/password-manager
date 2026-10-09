#include "database.hpp"
#include "../logging/logger.hpp"

Database::Database(const std::string& database_info) : connection_(database_info)
{
    server_log::info("database", "PostgreSQL connection established");
}

Account Database::createAccount(const std::uint32_t password_hash)
{
    server_log::debug("database", "creating account");
    pqxx::work transaction{connection_};
    const auto row = transaction.exec(
        "INSERT INTO accounts (password_hash, vault_version, vault_path) "
        "VALUES ($1, 0, '') RETURNING id",
        pqxx::params{password_hash}).one_row();
    const auto id = row[0].as<std::uint32_t>();
    const std::string path = "saved/" + std::to_string(id) + "/file";
    transaction.exec(
        "UPDATE accounts SET vault_path = $1 WHERE id = $2",
        pqxx::params{path, id});
    transaction.commit();
    server_log::info("database", "account created id=", id);
    return Account{id, password_hash, 0, path};
}

std::optional<Account> Database::account(const std::uint32_t id, const std::uint32_t password_hash)
{
    server_log::debug("database", "looking up account id=", id);
    pqxx::read_transaction transaction{connection_};
    // language=SQL
    const auto result = transaction.exec("SELECT id, password_hash, vault_version, vault_path FROM accounts WHERE id = $1 AND password_hash = $2",pqxx::params{id, password_hash});
    if (result.empty())
    {
        server_log::warning("database", "account lookup failed id=", id);
        return std::nullopt;
    }

    const auto row = result.one_row();
    server_log::debug("database", "account found id=", id,
                      " vault_version=", row[2].as<std::uint32_t>());
    return Account{
        row[0].as<std::uint32_t>(),
        row[1].as<std::uint32_t>(),
        row[2].as<std::uint32_t>(),
        row[3].as<std::string>(),
    };
}

bool Database::updateVaultVersion(
    const std::uint32_t id,
    const std::uint32_t password_hash,
    const std::uint32_t vault_version)
{
    server_log::debug("database", "updating vault version id=", id,
                      " version=", vault_version);
    pqxx::work transaction{connection_};
    const auto result = transaction.exec(
        "UPDATE accounts SET vault_version = $1 "
        "WHERE id = $2 AND password_hash = $3",
        pqxx::params{vault_version, id, password_hash});
    transaction.commit();
    const bool updated = result.affected_rows() == 1;
    if (updated)
        server_log::info("database", "vault version updated id=", id,
                         " version=", vault_version);
    else
        server_log::warning("database", "vault version was not updated id=", id);
    return updated;
}
