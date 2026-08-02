#include <crypto/sodium.hpp>
#include <logs/logs.hpp>

#include "sodium/core.h"
#include "sodium/crypto_pwhash.h"
#include "sodium/crypto_secretbox.h"
#include "sodium/randombytes.h"
#include "sodium/utils.h"

namespace
{
    constexpr uint32_t kMinPasswordLength = 8;
}


namespace crypto
{
    Sodium::Sodium(Logs& logs) : logs_(logs)
    {
        logs_.info_log("Initializing cryptographic subsystem");
        key_.resize(crypto_secretbox_KEYBYTES);

        if (sodium_init() < 0)
        {
            logs_.error_log("Cryptographic subsystem initialization failed");
            throw err::Error{err::SodiumError::SodiumInitError, "sodium init error."};
        }
        logs_.info_log("Cryptographic subsystem initialized");
    }

    Sodium::~Sodium()
    {
        logs_.info_log("Clearing cryptographic material");
        if (!key_.empty())
        {
            sodium_memzero(key_.data(), key_.size());
        }
        if (!salt_.empty())
        {
            sodium_memzero(salt_.data(), salt_.size());
        }
        if (!master_password_.empty())
        {
            sodium_memzero(master_password_.data(), master_password_.size());
        }
    }

    std::expected<void, err::Error> Sodium::keyGeneration(const std::vector<uint8_t> &salt)
    {
        if (salt.size() != crypto_pwhash_SALTBYTES)
        {
            logs_.warning_log("Key derivation rejected an invalid salt");
            return std::unexpected{err::Error{err::SodiumError::BrokenCryptedData,"salt is broken."}};
        }

        logs_.info_log("Deriving encryption key");
        const int32_t rc = crypto_pwhash(
            key_.data(),
            key_.size(),
            master_password_.c_str(),
            master_password_.size(),
            salt.data(),
            crypto_pwhash_OPSLIMIT_INTERACTIVE,
            crypto_pwhash_MEMLIMIT_INTERACTIVE,
            crypto_pwhash_ALG_ARGON2ID13);
        if (rc != 0)
        {
            logs_.error_log("Key derivation failed");
            return std::unexpected{err::Error{err::SodiumError::OutOfMemory,"Argon can't generate your key."}};
        }
        salt_ = salt;
        logs_.info_log("Encryption key derived");
        return {};
    }

    std::expected<SodiumInfo, err::Error> Sodium::encrypt(const std::string &plaintext)
    {
        logs_.info_log("Encrypting storage payload");
        if (salt_.empty() || key_.empty() || salt_.size() != crypto_pwhash_SALTBYTES)
        {
            std::vector<uint8_t> new_salt(crypto_pwhash_SALTBYTES);
            randombytes_buf(new_salt.data(), new_salt.size());
            if (const std::expected<void,err::Error> gen_result = keyGeneration(new_salt); !gen_result.has_value())
            {
                return std::unexpected{gen_result.error()};
            }
        } else
        {
            if (const std::expected<void,err::Error> gen_result = keyGeneration(salt_); !gen_result.has_value())
            {
                return std::unexpected{gen_result.error()};
            }
        }
        SodiumInfo sodium_info;
        sodium_info.salt = salt_;
        sodium_info.nonce.resize(crypto_secretbox_NONCEBYTES);
        randombytes_buf(sodium_info.nonce.data(), sodium_info.nonce.size());
        sodium_info.ciphertext.resize(plaintext.size() + crypto_secretbox_MACBYTES);

        crypto_secretbox_easy(
            sodium_info.ciphertext.data(),
            reinterpret_cast<const uint8_t*>(plaintext.data()),
            plaintext.size(),
            sodium_info.nonce.data(),
            key_.data());

        logs_.info_log("Storage payload encrypted");
        return sodium_info;
    }

    std::expected<std::string, err::Error> Sodium::decrypt(const SodiumInfo& sodium_info)
    {
        logs_.info_log("Decrypting storage payload");
        if (sodium_info.nonce.size() != crypto_secretbox_NONCEBYTES || sodium_info.ciphertext.size() <
            crypto_secretbox_MACBYTES || sodium_info.salt.size() != crypto_pwhash_SALTBYTES)
        {
            logs_.warning_log("Encrypted payload validation failed");
            return std::unexpected{err::Error{err::SodiumError::BrokenCryptedData,"crypted data is broken."}};
        }
        if (const std::expected<void,err::Error> gen_result = keyGeneration(sodium_info.salt); !gen_result.has_value())
        {
            return std::unexpected{gen_result.error()};
        }

        std::vector<uint8_t> plaintext(sodium_info.ciphertext.size() - crypto_secretbox_MACBYTES);

        const int32_t rc = crypto_secretbox_open_easy(
        plaintext.data(),
        sodium_info.ciphertext.data(),
            sodium_info.ciphertext.size(),
            sodium_info.nonce.data(),
            key_.data());

        if (rc != 0)
        {
            logs_.warning_log("Storage payload authentication failed");
            return std::unexpected{err::Error{err::SodiumError::SecretBoxOpenFailed,"Secret box open failed."}};
        }
        logs_.info_log("Storage payload decrypted");
        return std::string(reinterpret_cast<char*>(plaintext.data()), plaintext.size());
    }

    std::expected<void,err::Error> Sodium::setMasterPassword(const std::string &password)
    {
        if (password.length() < kMinPasswordLength)
        {
            logs_.warning_log("Master password validation failed");
            return std::unexpected{err::Error{err::SodiumError::PasswordIsTooShort,"Password is too short."}};
        }
        master_password_ = password;
        logs_.info_log("Master password accepted");
        return {};
    }
}

const std::string &crypto::Sodium::getMasterPassword()
{
    return master_password_;
}
