#pragma once

#include <string>
#include <cstdint>
enum class GenerationLevel
{
    Low,
    Medium,
    High,
    Maximum,
};

class Generator final
{
    public:
    // todo generator level for easy normal and hard password
    static std::string generate_random_password(uint32_t length);
};