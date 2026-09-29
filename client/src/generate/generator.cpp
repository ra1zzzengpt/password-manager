#include <generate/generator.hpp>
#include <constants/symbols.hpp>
#include <sodium.h>
#include <cmath>
#include <random>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>

#include "constants/paths.hpp"

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

std::expected<std::string,err::Error> Generator::generate_random_seed_phrase(uint32_t length, const std::string &separator)
{
    std::ifstream nouns(cnt::nounsPath());
    std::ifstream verbs(cnt::verbsPath());

    if (!nouns.is_open() || !verbs.is_open())
    {
        return std::unexpected{err::Error{.type = err::StorageError::OpenFileFailed, .message = "can't find wordlist files"}};
    }
    std::uint32_t nouns_count = 0;
    std::uint32_t verbs_count = 0;

    std::string line;

    while (std::getline(nouns, line))
    {
        nouns_count++;
    }
    nouns.clear();

    while (std::getline(verbs, line))
    {
        verbs_count++;
    }
    verbs.clear();

    std::vector<std::string> strings;

    nouns.seekg(std::ios_base::beg);
    verbs.seekg(std::ios_base::beg);

    for (uint32_t i = 1; i <= length; ++i)
    {
        std::string word;
        if (i % 2 != 0)
        {
            std::uint32_t word_index = randombytes_uniform(nouns_count);
            std::uint32_t current_word_index = 0;
            while (std::getline(nouns, word))
            {
                if (current_word_index == word_index)
                {
                    strings.push_back(word);
                    break;
                }
                current_word_index++;
            }
            nouns.clear();
            nouns.seekg(std::ios_base::beg);
        } else
        {
            std::uint32_t word_index = randombytes_uniform(verbs_count);
            std::uint32_t current_word_index = 0;
            while (std::getline(verbs, word))
            {
                if (current_word_index == word_index)
                {
                    strings.push_back(word);
                    break;
                }
                current_word_index++;
            }
            verbs.clear();
            verbs.seekg(std::ios_base::beg);
        }
    }

    std::string result;
    for (std::uint32_t i = 0; i < strings.size(); ++i)
    {
        result += strings[i];
        if (i < strings.size() - 1)
        {
            result += separator;
        }
    }
    return result;
}
