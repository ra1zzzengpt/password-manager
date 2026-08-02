#include <generate/generator.hpp>
#include <constants/symbols.hpp>
#include <sodium.h>
#include <cmath>
#include <random>
#include <algorithm>

std::string Generator::generate_random_password(const uint32_t length, const GenerationLevel& level)
{
    switch (level)
    {
        case GenerationLevel::Low:
            return generate_low(length);
        case GenerationLevel::Medium:
            return generate_medium(length);
        case GenerationLevel::High:
            return generate_high(length);
        default:
            return "";
    }
}

std::string Generator::generate_low(const uint32_t length)
{
    std::string password;
    password.reserve(length);
    for (uint32_t i = 0; i < length; ++i)
    {
        password.push_back(symbols::all_symbols[randombytes_uniform(symbols::all_symbols.size())]);
    }
    return password;
}

std::string Generator::generate_medium(const uint32_t length)
{
    std::string password;
    password.reserve(length);

    password.push_back(symbols::lower_chars[randombytes_uniform(symbols::lower_chars.size())]);
    password.push_back(symbols::upper_chars[randombytes_uniform(symbols::upper_chars.size())]);
    password.push_back(symbols::numbers[randombytes_uniform(symbols::numbers.size())]);
    password.push_back(symbols::special_symbols[randombytes_uniform(symbols::special_symbols.size())]);

    for (uint32_t i = 4; i < length; ++i)
    {
        password.push_back(symbols::all_symbols[randombytes_uniform(symbols::all_symbols.size())]);
    }

    std::ranges::shuffle(password, std::mt19937{std::random_device{}()});

    return password;
}

std::string Generator::generate_high(const uint32_t length)
{
    std::string password;
    password.reserve(length);

    const size_t random_reserve = randombytes_uniform(length / 4);

    for (uint32_t i = 0; i < random_reserve; ++i)
    {
        password.push_back(symbols::lower_chars[randombytes_uniform(symbols::lower_chars.size())]);
    }

    for (uint32_t i = random_reserve; i < random_reserve * 2; ++i)
    {
        password.push_back(symbols::upper_chars[randombytes_uniform(symbols::upper_chars.size())]);
    }

    for (uint32_t i = random_reserve * 2; i < random_reserve * 3; ++i)
    {
        password.push_back(symbols::numbers[randombytes_uniform(symbols::numbers.size())]);
    }

    for (uint32_t i = random_reserve * 3; i < random_reserve * 4; ++i)
    {
        password.push_back(symbols::special_symbols[randombytes_uniform(symbols::special_symbols.size())]);
    }

    for (uint32_t i = random_reserve * 4; i < length; ++i)
    {
        password.push_back(symbols::all_symbols[randombytes_uniform(symbols::all_symbols.size())]);
    }

    std::ranges::shuffle(password, std::mt19937{std::random_device{}()});

    return password;
}

EntropyLevel Generator::Entropy(const std::string& password) {
    if (password.empty()) return EntropyLevel::Low;

    bool hasLower = false, hasUpper = false, hasDigit = false, hasSpecial = false;

    for (const unsigned char c : password) {
        if (std::islower(c)) {
            hasLower = true;
        } else if (std::isupper(c)) {
            hasUpper = true;
        } else if (std::isdigit(c)) {
            hasDigit = true;
        } else if (symbols::special_symbols.find(static_cast<char>(c)) != std::string::npos) {
            hasSpecial = true;
        }
    }

    size_t charsetSize = 0;
    if (hasLower) charsetSize += symbols::lower_chars.size();
    if (hasUpper) charsetSize += symbols::upper_chars.size();
    if (hasDigit) charsetSize += symbols::numbers.size();
    if (hasSpecial) charsetSize += symbols::special_symbols.size();

    if (charsetSize == 0) return EntropyLevel::Low;

    const uint32_t entropy = static_cast<uint32_t>(static_cast<double>(password.length()) * std::log2(static_cast<double>(charsetSize)));

    if (entropy < 50)
    {
        return EntropyLevel::Low;
    }
    if (entropy < 80)
    {
        return EntropyLevel::Medium;
    }
    return EntropyLevel::High;
}
