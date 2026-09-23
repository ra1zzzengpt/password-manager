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
        FileIsTooBig,
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

    enum class SoundError
    {
        SoundNotExist,
    };
}