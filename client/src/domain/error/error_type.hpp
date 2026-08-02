#pragma once
namespace err
{
    enum class StorageError
    {
        OpenFileFailed,
        CreateDirectoryFailed,
        ParseFailed,
        FileStreamError,
        RenameFailed,
        DeleteFailed,
    };

    enum class SodiumError
    {
        SodiumInitError,
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

    enum class LogsError
    {
        InitError,
        CantCreateDirectory,
    };

    enum class SettingsError
    {
        PasswordsNotEqual,
    };
}