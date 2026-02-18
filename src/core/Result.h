/// Result.h — V7 Phase 01: Result types and error taxonomy
///
/// Defines Result<T> as an alias for std::expected<T, Error>, along with
/// ErrorCode, SubsystemId, and the Error struct. Replaces raw std::string
/// error types with structured, machine-parseable errors.
///
/// All validators, I/O operations, and parsing utilities return Result<T>
/// to propagate errors without exceptions.

#pragma once

#include <cstdint>
#include <expected>
#include <source_location>
#include <string>
#include <string_view>
#include <utility>

namespace markamp::core
{

// ══════════════════════════════════════════════════════════════════════════════
// Error Codes
// ══════════════════════════════════════════════════════════════════════════════

/// Categorized error codes for the entire application.
enum class ErrorCode : uint16_t
{
    // Generic (0–99)
    Unknown = 0,
    InvalidArgument = 1,
    NotFound = 2,
    AlreadyExists = 3,
    NotSupported = 4,
    PermissionDenied = 5,
    Timeout = 6,
    Cancelled = 7,
    ResourceExhausted = 8,

    // Validation (100–199)
    ValidationFailed = 100,
    StringTooLong = 101,
    StringTooShort = 102,
    InvalidFormat = 103,
    InvalidRange = 104,
    InvalidEnum = 105,
    InvalidPath = 106,
    InvalidYamlKey = 107,
    InvalidMimeType = 108,
    NullByte = 109,

    // I/O (200–299)
    IoError = 200,
    FileNotFound = 201,
    FileReadError = 202,
    FileWriteError = 203,
    FileTooLarge = 204,
    DirectoryTraversal = 205,
    SymlinkEscape = 206,
    CorruptedFile = 207,
    ChecksumMismatch = 208,

    // Parsing (300–399)
    ParseError = 300,
    JsonParseError = 301,
    YamlParseError = 302,
    NestingTooDeep = 303,
    TooManyKeys = 304,
    ValueTooLong = 305,
    RegexError = 306,
    RegexTimeout = 307,
    ManifestInvalid = 308,

    // Threading (400–499)
    ThreadingError = 400,
    DeadlockDetected = 401,
    WrongThread = 402,
    TaskCancelled = 403,
    PanicEscalation = 404,

    // Plugin (500–599)
    PluginError = 500,
    PluginLoadFailed = 501,
    PluginActivateFailed = 502,
    PluginDisabled = 503,
    PluginQuarantined = 504,
    PluginSandboxViolation = 505,
    PluginApiVersionMismatch = 506,
    PluginNetworkDenied = 507,

    // Rendering (600–699)
    RenderError = 600,
    ThemeLoadFailed = 601,
    ThemeTokenMissing = 602,
    FrameBudgetExceeded = 603,

    // Config (700–799)
    ConfigError = 700,
    ConfigCorrupted = 701,
    ConfigKeyNotFound = 702,
    ConfigTypeMismatch = 703,
    ConfigImmutable = 704,

    // Telemetry (800–899)
    TelemetryError = 800,
    ExportFailed = 801,

    // Overflow (900–999)
    IntegerOverflow = 900,
    IntegerUnderflow = 901,
    AllocationFailed = 902,
};

// ══════════════════════════════════════════════════════════════════════════════
// Subsystem Identification
// ══════════════════════════════════════════════════════════════════════════════

/// Identifies which subsystem generated an error.
enum class SubsystemId : uint8_t
{
    Core = 0,
    Rendering = 1,
    ExtensionHost = 2,
    MarkdownEngine = 3,
    YAMLParsing = 4,
    Workspace = 5,
    Network = 6,
    Editor = 7,
    Search = 8,
    Config = 9,
    Telemetry = 10,
    UI = 11,
    Canvas = 12,
    Sync = 13,
};

/// Get a human-readable name for a subsystem.
[[nodiscard]] constexpr auto subsystem_name(SubsystemId id) noexcept -> std::string_view
{
    switch (id)
    {
        case SubsystemId::Core:
            return "Core";
        case SubsystemId::Rendering:
            return "Rendering";
        case SubsystemId::ExtensionHost:
            return "ExtensionHost";
        case SubsystemId::MarkdownEngine:
            return "MarkdownEngine";
        case SubsystemId::YAMLParsing:
            return "YAMLParsing";
        case SubsystemId::Workspace:
            return "Workspace";
        case SubsystemId::Network:
            return "Network";
        case SubsystemId::Editor:
            return "Editor";
        case SubsystemId::Search:
            return "Search";
        case SubsystemId::Config:
            return "Config";
        case SubsystemId::Telemetry:
            return "Telemetry";
        case SubsystemId::UI:
            return "UI";
        case SubsystemId::Canvas:
            return "Canvas";
        case SubsystemId::Sync:
            return "Sync";
    }
    return "Unknown";
}

/// Get a human-readable label for an error code.
[[nodiscard]] constexpr auto error_code_name(ErrorCode code) noexcept -> std::string_view
{
    switch (code)
    {
        case ErrorCode::Unknown:
            return "Unknown";
        case ErrorCode::InvalidArgument:
            return "InvalidArgument";
        case ErrorCode::NotFound:
            return "NotFound";
        case ErrorCode::AlreadyExists:
            return "AlreadyExists";
        case ErrorCode::NotSupported:
            return "NotSupported";
        case ErrorCode::PermissionDenied:
            return "PermissionDenied";
        case ErrorCode::Timeout:
            return "Timeout";
        case ErrorCode::Cancelled:
            return "Cancelled";
        case ErrorCode::ResourceExhausted:
            return "ResourceExhausted";
        case ErrorCode::ValidationFailed:
            return "ValidationFailed";
        case ErrorCode::StringTooLong:
            return "StringTooLong";
        case ErrorCode::StringTooShort:
            return "StringTooShort";
        case ErrorCode::InvalidFormat:
            return "InvalidFormat";
        case ErrorCode::InvalidRange:
            return "InvalidRange";
        case ErrorCode::InvalidEnum:
            return "InvalidEnum";
        case ErrorCode::InvalidPath:
            return "InvalidPath";
        case ErrorCode::InvalidYamlKey:
            return "InvalidYamlKey";
        case ErrorCode::InvalidMimeType:
            return "InvalidMimeType";
        case ErrorCode::NullByte:
            return "NullByte";
        case ErrorCode::IoError:
            return "IoError";
        case ErrorCode::FileNotFound:
            return "FileNotFound";
        case ErrorCode::FileReadError:
            return "FileReadError";
        case ErrorCode::FileWriteError:
            return "FileWriteError";
        case ErrorCode::FileTooLarge:
            return "FileTooLarge";
        case ErrorCode::DirectoryTraversal:
            return "DirectoryTraversal";
        case ErrorCode::SymlinkEscape:
            return "SymlinkEscape";
        case ErrorCode::CorruptedFile:
            return "CorruptedFile";
        case ErrorCode::ChecksumMismatch:
            return "ChecksumMismatch";
        case ErrorCode::ParseError:
            return "ParseError";
        case ErrorCode::JsonParseError:
            return "JsonParseError";
        case ErrorCode::YamlParseError:
            return "YamlParseError";
        case ErrorCode::NestingTooDeep:
            return "NestingTooDeep";
        case ErrorCode::TooManyKeys:
            return "TooManyKeys";
        case ErrorCode::ValueTooLong:
            return "ValueTooLong";
        case ErrorCode::RegexError:
            return "RegexError";
        case ErrorCode::RegexTimeout:
            return "RegexTimeout";
        case ErrorCode::ManifestInvalid:
            return "ManifestInvalid";
        case ErrorCode::ThreadingError:
            return "ThreadingError";
        case ErrorCode::DeadlockDetected:
            return "DeadlockDetected";
        case ErrorCode::WrongThread:
            return "WrongThread";
        case ErrorCode::TaskCancelled:
            return "TaskCancelled";
        case ErrorCode::PanicEscalation:
            return "PanicEscalation";
        case ErrorCode::PluginError:
            return "PluginError";
        case ErrorCode::PluginLoadFailed:
            return "PluginLoadFailed";
        case ErrorCode::PluginActivateFailed:
            return "PluginActivateFailed";
        case ErrorCode::PluginDisabled:
            return "PluginDisabled";
        case ErrorCode::PluginQuarantined:
            return "PluginQuarantined";
        case ErrorCode::PluginSandboxViolation:
            return "PluginSandboxViolation";
        case ErrorCode::PluginApiVersionMismatch:
            return "PluginApiVersionMismatch";
        case ErrorCode::PluginNetworkDenied:
            return "PluginNetworkDenied";
        case ErrorCode::RenderError:
            return "RenderError";
        case ErrorCode::ThemeLoadFailed:
            return "ThemeLoadFailed";
        case ErrorCode::ThemeTokenMissing:
            return "ThemeTokenMissing";
        case ErrorCode::FrameBudgetExceeded:
            return "FrameBudgetExceeded";
        case ErrorCode::ConfigError:
            return "ConfigError";
        case ErrorCode::ConfigCorrupted:
            return "ConfigCorrupted";
        case ErrorCode::ConfigKeyNotFound:
            return "ConfigKeyNotFound";
        case ErrorCode::ConfigTypeMismatch:
            return "ConfigTypeMismatch";
        case ErrorCode::ConfigImmutable:
            return "ConfigImmutable";
        case ErrorCode::TelemetryError:
            return "TelemetryError";
        case ErrorCode::ExportFailed:
            return "ExportFailed";
        case ErrorCode::IntegerOverflow:
            return "IntegerOverflow";
        case ErrorCode::IntegerUnderflow:
            return "IntegerUnderflow";
        case ErrorCode::AllocationFailed:
            return "AllocationFailed";
    }
    return "Unknown";
}

// ══════════════════════════════════════════════════════════════════════════════
// Error Struct
// ══════════════════════════════════════════════════════════════════════════════

/// Structured error with machine-parseable code, subsystem, and human message.
struct Error
{
    ErrorCode code{ErrorCode::Unknown};
    SubsystemId subsystem{SubsystemId::Core};
    std::string message;
    std::string file;
    int line{0};

    /// Format the error for display / logging.
    [[nodiscard]] auto format() const -> std::string
    {
        std::string result;
        result.reserve(128);
        result += '[';
        result += subsystem_name(subsystem);
        result += "] ";
        result += error_code_name(code);
        result += ": ";
        result += message;
        if (!file.empty())
        {
            result += " (";
            result += file;
            result += ':';
            result += std::to_string(line);
            result += ')';
        }
        return result;
    }
};

// ══════════════════════════════════════════════════════════════════════════════
// Result<T> Alias
// ══════════════════════════════════════════════════════════════════════════════

/// The primary result type used throughout V7.
/// Success: holds a T value.
/// Failure: holds an Error struct.
template <typename T>
using Result = std::expected<T, Error>;

// ══════════════════════════════════════════════════════════════════════════════
// Factory Functions
// ══════════════════════════════════════════════════════════════════════════════

/// Create a generic Error.
[[nodiscard]] inline auto make_error(ErrorCode code,
                                     SubsystemId subsystem,
                                     std::string message,
                                     std::source_location loc = std::source_location::current())
    -> Error
{
    return Error{
        .code = code,
        .subsystem = subsystem,
        .message = std::move(message),
        .file = std::string(loc.file_name()),
        .line = static_cast<int>(loc.line()),
    };
}

/// Create a validation error (SubsystemId::Core by default).
[[nodiscard]] inline auto
make_validation_error(std::string message,
                      ErrorCode code = ErrorCode::ValidationFailed,
                      std::source_location loc = std::source_location::current()) -> Error
{
    return make_error(code, SubsystemId::Core, std::move(message), loc);
}

/// Create an I/O error (SubsystemId::Workspace by default).
[[nodiscard]] inline auto make_io_error(std::string message,
                                        ErrorCode code = ErrorCode::IoError,
                                        std::source_location loc = std::source_location::current())
    -> Error
{
    return make_error(code, SubsystemId::Workspace, std::move(message), loc);
}

/// Create a parse error.
[[nodiscard]] inline auto
make_parse_error(std::string message,
                 ErrorCode code = ErrorCode::ParseError,
                 std::source_location loc = std::source_location::current()) -> Error
{
    return make_error(code, SubsystemId::Core, std::move(message), loc);
}

/// Create a plugin error.
[[nodiscard]] inline auto
make_plugin_error(std::string message,
                  ErrorCode code = ErrorCode::PluginError,
                  std::source_location loc = std::source_location::current()) -> Error
{
    return make_error(code, SubsystemId::ExtensionHost, std::move(message), loc);
}

/// Create a config error.
[[nodiscard]] inline auto
make_config_error(std::string message,
                  ErrorCode code = ErrorCode::ConfigError,
                  std::source_location loc = std::source_location::current()) -> Error
{
    return make_error(code, SubsystemId::Config, std::move(message), loc);
}

/// Create a rendering error.
[[nodiscard]] inline auto
make_render_error(std::string message,
                  ErrorCode code = ErrorCode::RenderError,
                  std::source_location loc = std::source_location::current()) -> Error
{
    return make_error(code, SubsystemId::Rendering, std::move(message), loc);
}

// ══════════════════════════════════════════════════════════════════════════════
// Guard Macros
// ══════════════════════════════════════════════════════════════════════════════

/// Return an unexpected error if expression evaluates to false.
/// Usage: MARKAMP_GUARD(ptr != nullptr, make_error(...));
#define MARKAMP_GUARD(expr, error_val)                                                             \
    do                                                                                             \
    {                                                                                              \
        if (!(expr)) [[unlikely]]                                                                  \
        {                                                                                          \
            return std::unexpected(error_val);                                                     \
        }                                                                                          \
    } while (false)

/// Return the error from a Result if it failed.
/// Usage: MARKAMP_TRY(result);
#define MARKAMP_TRY(result_expr)                                                                   \
    do                                                                                             \
    {                                                                                              \
        auto&& _markamp_try_result = (result_expr);                                                \
        if (!_markamp_try_result.has_value()) [[unlikely]]                                         \
        {                                                                                          \
            return std::unexpected(std::move(_markamp_try_result.error()));                        \
        }                                                                                          \
    } while (false)

/// Unwrap a Result: return the value or propagate the error.
/// Usage: auto val = MARKAMP_UNWRAP(some_result_expr);
#define MARKAMP_UNWRAP(result_expr)                                                                \
    ({                                                                                             \
        auto&& _markamp_unwrap_res = (result_expr);                                                \
        if (!_markamp_unwrap_res.has_value()) [[unlikely]]                                         \
        {                                                                                          \
            return std::unexpected(std::move(_markamp_unwrap_res.error()));                        \
        }                                                                                          \
        std::move(*_markamp_unwrap_res);                                                           \
    })

} // namespace markamp::core
