#pragma once
namespace err
{
    enum class StorageError
    {
        OpenFileFailed,
        CreateDirectoryFailed,
        ParseFailed,
        FileStreamError,
    };

    enum class SodiumError
    {
        InfoDataFailed,
        OutOfMemory,
        BrokenCryptedData,
        SecretBoxOpenFailed,
    };
}