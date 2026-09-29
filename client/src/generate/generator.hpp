#pragma once

#include <string>
#include <cstdint>
#include <expected>

#include "domain/error/error.hpp"

enum class GenerationLevel
{
    Low,
    Medium,
    High,
};

enum class EntropyLevel
{
    Low,
    Medium,
    High,
};

class Generator final
{
public:
    static std::string generate_random_password(uint32_t length, const GenerationLevel& level);

    static std::expected<std::string,err::Error> generate_random_seed_phrase(uint32_t length, const std::string& separator);

    static EntropyLevel Entropy(const std::string& password);

private:
    static std::string generate_low(uint32_t length);
    static std::string generate_medium(uint32_t length);
    static std::string generate_high(uint32_t length);
};