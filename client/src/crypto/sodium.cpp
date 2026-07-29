#include "sodium.hpp"
#include <cassert>
#include <QDebug>

#include "sodium/core.h"
#include "sodium/crypto_pwhash.h"
#include "sodium/crypto_secretbox.h"
#include "sodium/randombytes.h"
#include "sodium/utils.h"

// todo add choose in settings level of crypt (for sensitive)
namespace
{
    constexpr uint32_t kMinPasswordLength = 8;
}


namespace crypto
{
    Sodium::Sodium()
    {
        key_.resize(crypto_secretbox_KEYBYTES);
        assert(sodium_init() >= 0);
    }

    Sodium::~Sodium()
    {
        if (!key_.empty())
        {
            sodium_memzero(key_.data(), key_.size());
        }
        if (!salt_.empty())
        {
            sodium_memzero(salt_.data(), salt_.size());
        }
    }

    std::expected<void, err::Error> Sodium::keyGeneration(const std::vector<uint8_t> &salt)
    {
        if (salt.size() != crypto_pwhash_SALTBYTES)
        {
            return std::unexpected{err::Error{err::SodiumError::BrokenCryptedData,"salt is broken."}};
        }

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
            return std::unexpected{err::Error{err::SodiumError::OutOfMemory,"Argon can't generate your key."}};
        }
        salt_ = salt;
        return {};
    }

    std::expected<SodiumInfo, err::Error> Sodium::encrypt(const std::string &plaintext)
    {
        if (salt_.empty() || key_.empty() || salt_.size() != crypto_pwhash_SALTBYTES)
        {
            std::vector<uint8_t> new_salt(crypto_pwhash_SALTBYTES);
            randombytes_buf(new_salt.data(), new_salt.size());
            if (const std::expected<void,err::Error> gen_result = keyGeneration(new_salt); !gen_result.has_value())
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

        return sodium_info;
    }

    std::expected<std::string, err::Error> Sodium::decrypt(const SodiumInfo& sodium_info)
    {
        if (sodium_info.nonce.size() != crypto_secretbox_NONCEBYTES || sodium_info.ciphertext.size() <
            crypto_secretbox_MACBYTES || sodium_info.salt.size() != crypto_pwhash_SALTBYTES)
        {
            return std::unexpected{err::Error{err::SodiumError::BrokenCryptedData,"crypted data is broken."}};
        }
        if (key_.empty() || salt_.empty() || salt_ != sodium_info.salt || salt_.size() != crypto_pwhash_SALTBYTES)
        {
            if (const std::expected<void,err::Error> gen_result = keyGeneration(sodium_info.salt); !gen_result.has_value())
            {
                return std::unexpected{gen_result.error()};
            }
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
            return std::unexpected{err::Error{err::SodiumError::SecretBoxOpenFailed,"Secret box open failed."}};
        }
        return std::string(reinterpret_cast<char*>(plaintext.data()), plaintext.size());
    }

    std::expected<void,err::Error> Sodium::setMasterPassword(const std::string &password)
    {
        if (password.length() < kMinPasswordLength)
        {
            return std::unexpected{err::Error{err::SodiumError::PasswordIsTooShort,"Password is too short."}};
        }
        master_password_ = password;
        return {};
    }
}
