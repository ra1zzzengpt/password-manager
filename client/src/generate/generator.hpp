#pragma once

#include <string>
#include <cstdint>

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

    static EntropyLevel Entropy(const std::string& password);

private:
    static std::string generate_low(uint32_t length);
    static std::string generate_medium(uint32_t length);
    static std::string generate_high(uint32_t length);
};