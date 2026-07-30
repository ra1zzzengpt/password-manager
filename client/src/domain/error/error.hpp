#pragma once
#include <string>
#include <variant>
#include <domain/error/error_type.hpp>

namespace err
{
    struct Error
    {
        std::variant<StorageError,SodiumError,TransformError> type;
        std::string message;
    };
}
