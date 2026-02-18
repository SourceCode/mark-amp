// ============================================================================
// File: src/core/UrlSanitizer.cpp
// Phase 29: Security & Input Validation — URL Sanitization
// ============================================================================

#include "UrlSanitizer.h"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <vector>

namespace markamp::core
{

// ── Construction ──

UrlSanitizer::UrlSanitizer()
{
    initialize_defaults();
}

void UrlSanitizer::initialize_defaults()
{
    allowed_schemes_ = {"http", "https", "mailto", "file", "ftp", "tel", "ssh"};
}

// ── Scheme helpers ──

auto UrlSanitizer::extract_scheme(std::string_view url) -> std::string
{
    auto colon_pos = url.find(':');
    if (colon_pos == std::string_view::npos || colon_pos == 0)
    {
        return {};
    }

    std::string scheme(url.substr(0, colon_pos));
    // Lowercase the scheme.
    std::transform(scheme.begin(),
                   scheme.end(),
                   scheme.begin(),
                   [](unsigned char character) { return std::tolower(character); });

    // Scheme must be alphabetic (+ digits/+/-/. after first char) per RFC 3986.
    if (!std::isalpha(static_cast<unsigned char>(scheme[0])))
    {
        return {};
    }
    for (size_t idx = 1; idx < scheme.size(); ++idx)
    {
        const auto chr = static_cast<unsigned char>(scheme[idx]);
        if (!std::isalnum(chr) && chr != '+' && chr != '-' && chr != '.')
        {
            return {};
        }
    }
    return scheme;
}

auto UrlSanitizer::blocked_schemes() -> std::vector<std::string>
{
    return {"javascript", "vbscript", "data"};
}

auto UrlSanitizer::is_protocol_relative(std::string_view url) -> bool
{
    return url.size() >= 2 && url[0] == '/' && url[1] == '/';
}

auto UrlSanitizer::is_local_url(std::string_view url) -> bool
{
    const auto normalized = normalize(url);
    // Check common local patterns.
    if (normalized.find("://localhost") != std::string::npos)
    {
        return true;
    }
    if (normalized.find("://127.0.0.1") != std::string::npos)
    {
        return true;
    }
    if (normalized.find("://[::1]") != std::string::npos)
    {
        return true;
    }
    if (normalized.starts_with("file://"))
    {
        return true;
    }
    return false;
}

// ── Percent encoding ──

auto UrlSanitizer::percent_decode(std::string_view input) -> std::string
{
    std::string result;
    result.reserve(input.size());

    for (size_t idx = 0; idx < input.size(); ++idx)
    {
        if (input[idx] == '%' && idx + 2 < input.size() &&
            std::isxdigit(static_cast<unsigned char>(input[idx + 1])) &&
            std::isxdigit(static_cast<unsigned char>(input[idx + 2])))
        {
            const auto high = input[idx + 1];
            const auto low = input[idx + 2];
            auto hex_to_int = [](char hex_char) -> int
            {
                if (hex_char >= '0' && hex_char <= '9')
                    return hex_char - '0';
                if (hex_char >= 'a' && hex_char <= 'f')
                    return hex_char - 'a' + 10;
                if (hex_char >= 'A' && hex_char <= 'F')
                    return hex_char - 'A' + 10;
                return 0;
            };
            result += static_cast<char>((hex_to_int(high) << 4) | hex_to_int(low));
            idx += 2;
        }
        else
        {
            result += input[idx];
        }
    }
    return result;
}

auto UrlSanitizer::has_encoded_danger(std::string_view url) -> bool
{
    // Decode and check if the result starts with a dangerous scheme.
    const auto decoded = percent_decode(url);
    std::string lower_decoded(decoded.size(), '\0');
    std::transform(decoded.begin(),
                   decoded.end(),
                   lower_decoded.begin(),
                   [](unsigned char character) { return std::tolower(character); });

    // Strip whitespace/control chars that browsers might ignore.
    std::string stripped;
    stripped.reserve(lower_decoded.size());
    for (const auto chr : lower_decoded)
    {
        if (chr != '\t' && chr != '\n' && chr != '\r' && chr != ' ')
        {
            stripped += chr;
        }
    }

    for (const auto& blocked : blocked_schemes())
    {
        if (stripped.starts_with(blocked + ":"))
        {
            return true;
        }
    }
    return false;
}

// ── Normalization ──

auto UrlSanitizer::normalize(std::string_view url) -> std::string
{
    std::string result(url);

    // Trim leading/trailing whitespace.
    while (!result.empty() && std::isspace(static_cast<unsigned char>(result.front())))
    {
        result.erase(result.begin());
    }
    while (!result.empty() && std::isspace(static_cast<unsigned char>(result.back())))
    {
        result.pop_back();
    }

    // Lowercase the scheme portion.
    auto colon_pos = result.find(':');
    if (colon_pos != std::string::npos && colon_pos > 0)
    {
        std::transform(result.begin(),
                       result.begin() + static_cast<std::ptrdiff_t>(colon_pos),
                       result.begin(),
                       [](unsigned char character) { return std::tolower(character); });
    }

    return result;
}

// ── Core validation ──

auto UrlSanitizer::is_safe_scheme(std::string_view url) const -> bool
{
    if (is_protocol_relative(url))
    {
        return false;
    }

    // Check for encoded danger first.
    if (has_encoded_danger(url))
    {
        return false;
    }

    const auto scheme = extract_scheme(url);
    if (scheme.empty())
    {
        // Relative URLs (no scheme) are allowed — they're resolved relative to base.
        return true;
    }

    // Check blocked list first (always blocked).
    for (const auto& blocked : blocked_schemes())
    {
        if (scheme == blocked)
        {
            return false;
        }
    }

    // Check allowed list.
    return allowed_schemes_.contains(scheme);
}

auto UrlSanitizer::is_safe_url(std::string_view url) const -> bool
{
    if (url.empty())
    {
        return false;
    }

    const auto normalized = normalize(url);

    // Protocol-relative URLs are always unsafe.
    if (is_protocol_relative(normalized))
    {
        return false;
    }

    // Check scheme safety.
    if (!is_safe_scheme(normalized))
    {
        return false;
    }

    // Check for null bytes.
    if (normalized.find('\0') != std::string::npos)
    {
        return false;
    }

    return true;
}

auto UrlSanitizer::sanitize(std::string_view url) const -> Result<std::string>
{
    if (url.empty())
    {
        return std::unexpected(make_validation_error("URL is empty", ErrorCode::InvalidArgument));
    }

    auto normalized = normalize(url);

    if (is_protocol_relative(normalized))
    {
        return std::unexpected(make_validation_error("Protocol-relative URLs are not allowed",
                                                     ErrorCode::PermissionDenied));
    }

    if (has_encoded_danger(normalized))
    {
        return std::unexpected(make_validation_error("URL contains encoded dangerous content",
                                                     ErrorCode::PermissionDenied));
    }

    const auto scheme = extract_scheme(normalized);
    if (!scheme.empty())
    {
        for (const auto& blocked : blocked_schemes())
        {
            if (scheme == blocked)
            {
                return std::unexpected(make_validation_error(
                    "URL scheme '" + scheme + "' is blocked", ErrorCode::PermissionDenied));
            }
        }

        if (!allowed_schemes_.contains(scheme))
        {
            return std::unexpected(
                make_validation_error("URL scheme '" + scheme + "' is not in the allowed list",
                                      ErrorCode::PermissionDenied));
        }
    }

    return normalized;
}

// ── Configuration ──

auto UrlSanitizer::allow_scheme(const std::string& scheme) -> void
{
    std::string lower_scheme = scheme;
    std::transform(lower_scheme.begin(),
                   lower_scheme.end(),
                   lower_scheme.begin(),
                   [](unsigned char character) { return std::tolower(character); });
    allowed_schemes_.insert(lower_scheme);
}

auto UrlSanitizer::disallow_scheme(const std::string& scheme) -> void
{
    std::string lower_scheme = scheme;
    std::transform(lower_scheme.begin(),
                   lower_scheme.end(),
                   lower_scheme.begin(),
                   [](unsigned char character) { return std::tolower(character); });
    allowed_schemes_.erase(lower_scheme);
}

auto UrlSanitizer::allowed_schemes() const -> std::vector<std::string>
{
    return {allowed_schemes_.begin(), allowed_schemes_.end()};
}

} // namespace markamp::core
