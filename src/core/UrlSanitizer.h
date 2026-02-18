// ============================================================================
// File: src/core/UrlSanitizer.h
// Phase 29: Security & Input Validation — URL Sanitization
// ============================================================================
#pragma once

#include "Result.h"

#include <set>
#include <string>
#include <string_view>

namespace markamp::core
{

/// URL sanitizer that validates, normalizes, and blocks dangerous URLs.
///
/// Defense-in-depth: applied to all user-provided and markdown-embedded URLs
/// before rendering or navigation. Blocks dangerous schemes (javascript:,
/// vbscript:, data:text/html), protocol-relative URLs, and non-standard
/// encodings.
class UrlSanitizer
{
public:
    UrlSanitizer();

    /// Sanitize a URL, returning the normalized safe URL or an error.
    [[nodiscard]] auto sanitize(std::string_view url) const -> Result<std::string>;

    /// Check if a URL uses a safe scheme.
    [[nodiscard]] auto is_safe_scheme(std::string_view url) const -> bool;

    /// Combined safety check: scheme + encoding + structure.
    [[nodiscard]] auto is_safe_url(std::string_view url) const -> bool;

    /// Normalize a URL: lowercase scheme, percent-encoding normalization.
    [[nodiscard]] static auto normalize(std::string_view url) -> std::string;

    /// Extract the scheme from a URL (e.g., "https" from "https://example.com").
    [[nodiscard]] static auto extract_scheme(std::string_view url) -> std::string;

    /// Check if a URL references localhost or local file.
    [[nodiscard]] static auto is_local_url(std::string_view url) -> bool;

    /// Check if a URL is protocol-relative (starts with //).
    [[nodiscard]] static auto is_protocol_relative(std::string_view url) -> bool;

    /// Add a scheme to the allowed set.
    auto allow_scheme(const std::string& scheme) -> void;

    /// Remove a scheme from the allowed set.
    auto disallow_scheme(const std::string& scheme) -> void;

    /// Get the set of allowed schemes.
    [[nodiscard]] auto allowed_schemes() const -> std::vector<std::string>;

    /// Get the set of blocked schemes (always blocked regardless of allowlist).
    [[nodiscard]] static auto blocked_schemes() -> std::vector<std::string>;

private:
    /// Initialize default allowed schemes.
    void initialize_defaults();

    /// Check for encoded dangerous content (e.g., %6A%61%76%61%73%63%72%69%70%74:).
    [[nodiscard]] static auto has_encoded_danger(std::string_view url) -> bool;

    /// Decode percent-encoded characters for analysis.
    [[nodiscard]] static auto percent_decode(std::string_view input) -> std::string;

    std::set<std::string, std::less<>> allowed_schemes_;
};

} // namespace markamp::core
