#pragma once
#include <cstdint>
#include <vector>
#include <expected>
#include <domain/error/error.hpp>


namespace crypto {

    struct SodiumInfo
    {
        std::vector<uint8_t> ciphertext;
        std::vector<uint8_t> salt;
        std::vector<uint8_t> nonce;
    };

    std::vector<uint8_t> port(const SodiumInfo& sodium_info);
    std::expected<SodiumInfo,err::Error> import(const std::vector<uint8_t>& sodium_info);
}
