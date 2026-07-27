#include <crypto/helper/sodium_info.hpp>

#include <sodium.h>

namespace crypto
{
    std::vector<uint8_t> port(const SodiumInfo &sodium_info)
    {
        std::vector<uint8_t> result;
        result.insert(result.begin(), sodium_info.nonce.begin(), sodium_info.nonce.end());
        result.insert(result.begin() + static_cast<long>(sodium_info.nonce.size()), sodium_info.salt.begin(),
                      sodium_info.salt.end());
        result.insert(
            result.begin() + static_cast<long>(sodium_info.nonce.size()) + static_cast<long>(sodium_info.salt.size()),
            sodium_info.ciphertext.begin(), sodium_info.ciphertext.end());
        return result;
    }

    std::expected<SodiumInfo,err::Error> import(const std::vector<uint8_t>& raw_crypted_bytes)
    {
        if (raw_crypted_bytes.size() < crypto_secretbox_NONCEBYTES + crypto_pwhash_SALTBYTES)
        {
            return std::unexpected{err::Error{err::SodiumError::InfoDataFailed,"hasn't needed data"}};
        }
        SodiumInfo sodium_info;
        sodium_info.nonce.resize(crypto_secretbox_NONCEBYTES);
        for (size_t i = 0; i < crypto_secretbox_NONCEBYTES; i++)
        {
            sodium_info.nonce[i] = raw_crypted_bytes[i];
        }
        sodium_info.salt.resize(crypto_pwhash_SALTBYTES);
        for (size_t i = crypto_secretbox_NONCEBYTES; i < crypto_pwhash_SALTBYTES + crypto_secretbox_NONCEBYTES; i++)
        {
            sodium_info.salt[i] = raw_crypted_bytes[i];
        }
        sodium_info.ciphertext.resize(raw_crypted_bytes.size() - crypto_secretbox_NONCEBYTES - crypto_pwhash_SALTBYTES);
        for (size_t i = crypto_pwhash_SALTBYTES + crypto_secretbox_NONCEBYTES; i < raw_crypted_bytes.size(); i++)
        {
            sodium_info.ciphertext[i] = raw_crypted_bytes[i];
        }
        return sodium_info;
    }
}