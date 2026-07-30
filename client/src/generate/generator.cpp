#include "generator.hpp"

#include <random>
namespace {
    const std::string ALLOWED_SYMBOLS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()";
}
std::string Generator::generate_random_password(const uint32_t length)
{
    static std::mt19937_64 gen(std::random_device{}());
    std::uniform_int_distribution<size_t> dis(0, ALLOWED_SYMBOLS.size() - 1);

    std::string result;
    result.reserve(length);
    for (uint32_t i = 0; i < length; ++i)
    {
        result.push_back(ALLOWED_SYMBOLS[dis(gen)]);
    }
    return result;
}
