#pragma once
#include <expected>
#include <vector>

#include <crypto/helper/sodium_info.hpp>

namespace crypto
{
    class Sodium final
    {
        public:
        Sodium() = default;
        ~Sodium();

        Sodium(const Sodium&) = delete;
        Sodium& operator=(const Sodium&) = delete;

        Sodium(const Sodium&&) = delete;
        Sodium& operator=(const Sodium&&) = delete;

        std::expected<SodiumInfo,err::Error> encrypt(const std::string& plaintext);
        std::expected<std::string,err::Error> decrypt(const SodiumInfo& sodium_info);
    private:
        std::expected<void,err::Error> keyGeneration(const std::vector<uint8_t>& salt);

        std::vector<uint8_t> salt_;
        std::vector<uint8_t> key_;
        std::string master_password_;
    };


}
