/// ValidationUtils.cpp — V7 Phase 02: Centralized input validation utilities

#include "ValidationUtils.h"

#include <algorithm>

namespace markamp::core
{

// ──────────────────────────────────────────────────────────────────────────────
// String Validation
// ──────────────────────────────────────────────────────────────────────────────

auto validate_string(std::string_view input,
                     size_t min_length,
                     size_t max_length,
                     std::string_view field_name) -> Result<std::string>
{
    if (input.size() < min_length)
    {
        return std::unexpected(make_validation_error(std::string(field_name) + " is too short (" +
                                                         std::to_string(input.size()) + " < " +
                                                         std::to_string(min_length) + ")",
                                                     ErrorCode::StringTooShort));
    }
    if (input.size() > max_length)
    {
        return std::unexpected(make_validation_error(std::string(field_name) + " is too long (" +
                                                         std::to_string(input.size()) + " > " +
                                                         std::to_string(max_length) + ")",
                                                     ErrorCode::StringTooLong));
    }
    return std::string(input);
}

auto validate_no_null_bytes(std::string_view input, std::string_view field_name) -> Result<void>
{
    if (input.find('\0') != std::string_view::npos)
    {
        return std::unexpected(make_validation_error(
            std::string(field_name) + " contains null byte", ErrorCode::NullByte));
    }
    return {};
}

auto validate_display_string(std::string_view input, size_t max_length, std::string_view field_name)
    -> Result<std::string>
{
    if (input.empty())
    {
        return std::unexpected(make_validation_error(std::string(field_name) + " is empty",
                                                     ErrorCode::StringTooShort));
    }
    if (input.size() > max_length)
    {
        return std::unexpected(make_validation_error(std::string(field_name) + " is too long (" +
                                                         std::to_string(input.size()) + " > " +
                                                         std::to_string(max_length) + ")",
                                                     ErrorCode::StringTooLong));
    }
    // Check for control characters (allow \n, \t)
    for (size_t idx = 0; idx < input.size(); ++idx)
    {
        auto character = input[idx];
        if (character != '\n' && character != '\t' && character >= 0 && character < 32)
        {
            return std::unexpected(make_validation_error(
                std::string(field_name) + " contains control character at position " +
                    std::to_string(idx),
                ErrorCode::InvalidFormat));
        }
    }
    return std::string(input);
}

// ──────────────────────────────────────────────────────────────────────────────
// YAML Key Validation
// ──────────────────────────────────────────────────────────────────────────────

auto validate_yaml_key(std::string_view key) -> Result<std::string>
{
    if (key.empty())
    {
        return std::unexpected(
            make_validation_error("YAML key is empty", ErrorCode::InvalidYamlKey));
    }
    if (key.size() > kMaxYamlKeyLength)
    {
        return std::unexpected(make_validation_error("YAML key exceeds maximum length of " +
                                                         std::to_string(kMaxYamlKeyLength),
                                                     ErrorCode::InvalidYamlKey));
    }
    for (size_t idx = 0; idx < key.size(); ++idx)
    {
        char character = key[idx];
        bool valid = (character >= 'a' && character <= 'z') ||
                     (character >= 'A' && character <= 'Z') ||
                     (character >= '0' && character <= '9') || character == '_' ||
                     character == '.' || character == '-';
        if (!valid)
        {
            return std::unexpected(
                make_validation_error(std::string("YAML key contains invalid character '") +
                                          character + "' at position " + std::to_string(idx),
                                      ErrorCode::InvalidYamlKey));
        }
    }
    return std::string(key);
}

// ──────────────────────────────────────────────────────────────────────────────
// Path Validation
// ──────────────────────────────────────────────────────────────────────────────

auto validate_path(std::string_view path_str) -> Result<std::filesystem::path>
{
    if (path_str.empty())
    {
        return std::unexpected(make_validation_error("Path is empty", ErrorCode::InvalidPath));
    }
    if (path_str.size() > kMaxPathLength)
    {
        return std::unexpected(make_validation_error("Path exceeds maximum length of " +
                                                         std::to_string(kMaxPathLength),
                                                     ErrorCode::InvalidPath));
    }
    // No null bytes
    auto null_result = validate_no_null_bytes(path_str, "path");
    if (!null_result.has_value())
    {
        return std::unexpected(
            make_validation_error("Path contains null byte", ErrorCode::NullByte));
    }
    // Check for directory traversal
    std::filesystem::path fspath(path_str);
    for (const auto& component : fspath)
    {
        if (component == "..")
        {
            return std::unexpected(make_validation_error(
                "Path contains directory traversal component '..'", ErrorCode::DirectoryTraversal));
        }
    }
    return fspath;
}

auto validate_filename(std::string_view filename) -> Result<std::string>
{
    if (filename.empty())
    {
        return std::unexpected(make_validation_error("Filename is empty", ErrorCode::InvalidPath));
    }
    if (filename.size() > kMaxFilenameLength)
    {
        return std::unexpected(make_validation_error("Filename exceeds maximum length of " +
                                                         std::to_string(kMaxFilenameLength),
                                                     ErrorCode::InvalidPath));
    }
    // No null bytes
    auto null_result = validate_no_null_bytes(filename, "filename");
    if (!null_result.has_value())
    {
        return std::unexpected(
            make_validation_error("Filename contains null byte", ErrorCode::NullByte));
    }
    // No path separators
    if (filename.find('/') != std::string_view::npos ||
        filename.find('\\') != std::string_view::npos)
    {
        return std::unexpected(
            make_validation_error("Filename contains path separator", ErrorCode::InvalidPath));
    }
    return std::string(filename);
}

} // namespace markamp::core
