/// MimeValidator.cpp — V7 Phase 13: MIME validation and markdown parsing limits

#include "MimeValidator.h"

#include <algorithm>
#include <array>

namespace markamp::core
{

namespace
{

/// List of known safe MIME types for the editor.
constexpr std::array<std::string_view, 12> kSafeEditorMimes = {
    "text/plain",
    "text/markdown",
    "text/html",
    "text/css",
    "text/csv",
    "text/xml",
    "application/json",
    "application/xml",
    "application/yaml",
    "application/x-yaml",
    "text/x-yaml",
    "text/javascript",
};

} // namespace

auto validate_mime_type(std::string_view mime) -> Result<std::string>
{
    if (mime.empty())
    {
        return std::unexpected(
            make_validation_error("MIME type is empty", ErrorCode::InvalidMimeType));
    }
    // Basic structure check: must contain exactly one '/'
    auto slash_pos = mime.find('/');
    if (slash_pos == std::string_view::npos)
    {
        return std::unexpected(make_validation_error("MIME type missing /: " + std::string(mime),
                                                     ErrorCode::InvalidMimeType));
    }
    if (mime.find('/', slash_pos + 1) != std::string_view::npos)
    {
        return std::unexpected(make_validation_error(
            "MIME type has multiple /: " + std::string(mime), ErrorCode::InvalidMimeType));
    }
    // Type and subtype must be non-empty
    auto type = mime.substr(0, slash_pos);
    auto subtype = mime.substr(slash_pos + 1);
    if (type.empty() || subtype.empty())
    {
        return std::unexpected(make_validation_error(
            "MIME type or subtype is empty: " + std::string(mime), ErrorCode::InvalidMimeType));
    }
    // Characters: [a-zA-Z0-9._+-] only
    auto valid_char = [](char character) -> bool
    {
        return (character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z') ||
               (character >= '0' && character <= '9') || character == '.' || character == '_' ||
               character == '+' || character == '-' || character == 'x'; // for x- prefixes
    };
    for (const char character : type)
    {
        if (!valid_char(character))
        {
            return std::unexpected(
                make_validation_error("Invalid character in MIME type: " + std::string(mime),
                                      ErrorCode::InvalidMimeType));
        }
    }
    for (const char character : subtype)
    {
        if (!valid_char(character))
        {
            return std::unexpected(
                make_validation_error("Invalid character in MIME subtype: " + std::string(mime),
                                      ErrorCode::InvalidMimeType));
        }
    }
    return std::string(mime);
}

auto is_text_mime(std::string_view mime) -> bool
{
    return mime.starts_with("text/");
}

auto is_safe_editor_mime(std::string_view mime) -> bool
{
    return std::ranges::find(kSafeEditorMimes, mime) != kSafeEditorMimes.end();
}

} // namespace markamp::core
