#pragma once
#include <expected>
#include <vector>

#include <crypto/helper/sodium_info.hpp>

class Logs;

namespace crypto
{
    class Sodium final
    {
        public:
        explicit Sodium(Logs& logs);
        ~Sodium();

        Sodium(const Sodium&) = delete;
        Sodium& operator=(const Sodium&) = delete;

        Sodium(const Sodium&&) = delete;
        Sodium& operator=(const Sodium&&) = delete;

        std::expected<SodiumInfo,err::Error> encrypt(const std::string& plaintext);
        std::expected<std::string,err::Error> decrypt(const SodiumInfo& sodium_info);

        std::expected<void,err::Error> setMasterPassword(const std::string& password);

        const std::string& getMasterPassword();
    private:
        std::expected<void,err::Error> keyGeneration(const std::vector<uint8_t>& salt);

        std::vector<uint8_t> salt_;
        std::vector<uint8_t> key_;
        std::string master_password_;
        Logs& logs_;
    };


}
