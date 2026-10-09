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
        VaultVersionOverflow,
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

    enum class NetworkError
    {
        CantLoadSertificate,
        CantConnectToServer,
        CantReadResponse,
        CantShutdownConnection,
        InformationalResponse,
        Redirection,
        BadRequest,
        Unauthorized,
        Forbidden,
        NotFound,
        MethodNotAllowed,
        RequestTimeout,
        Conflict,
        PayloadTooLarge,
        UnsupportedMediaType,
        UnprocessableEntity,
        TooManyRequests,
        ClientError,
        InternalServerError,
        NotImplemented,
        BadGateway,
        ServiceUnavailable,
        GatewayTimeout,
        ServerError,
        UnexpectedHttpStatus,
    };
}
