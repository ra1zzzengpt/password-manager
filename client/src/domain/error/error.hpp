#pragma once
#include <string>
#include <variant>
#include <domain/error/error_type.hpp>

namespace err
{
    struct Error
    {
        std::variant<StorageError,SodiumError> type;
        std::string message;
    };
}
