#pragma once
#include <string>
#include <variant>
#include <domain/error/error_type.hpp>

namespace err
{
    struct Error
    {
        std::variant<StorageError,SoundError,SodiumError,TransformError,SettingsError,LogsError,NetworkError> type;
        std::string message;
    };
}
