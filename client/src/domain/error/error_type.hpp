#pragma once
namespace err
{
    enum class StorageError
    {
        OpenFileFailed,
        CreateDirectoryFailed,
        ParseFailed,
        FileStreamError,
        DeleteFailed,
    };

    enum class SodiumError
    {
        InfoDataFailed,
        OutOfMemory,
        BrokenCryptedData,
        SecretBoxOpenFailed,
        PasswordIsTooShort,
    };

    enum class TransformError
    {
        TransformFailed,
    };
}