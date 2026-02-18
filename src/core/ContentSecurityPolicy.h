// ============================================================================
// File: src/core/ContentSecurityPolicy.h
// Phase 29: Security & Input Validation — CSP Header Generation
// ============================================================================
#pragma once

#include <string>
#include <vector>

namespace markamp::core
{

/// Configuration for a Content Security Policy.
/// Each field maps to a CSP directive.
struct CspConfig
{
    std::vector<std::string> default_src{"'self'"};
    std::vector<std::string> script_src{"'none'"};
    std::vector<std::string> style_src{"'self'"};
    std::vector<std::string> img_src{"'self'", "data:", "https:"};
    std::vector<std::string> connect_src{"'self'"};
    std::vector<std::string> font_src{"'self'"};
    std::vector<std::string> frame_src{"'none'"};
    std::vector<std::string> object_src{"'none'"};
    std::vector<std::string> media_src{"'self'"};
    std::vector<std::string> base_uri{"'self'"};
    bool upgrade_insecure_requests{true};
};

/// Content Security Policy generator and validator.
///
/// Generates CSP header strings from configuration. Provides strict
/// and relaxed presets for different rendering contexts.
class ContentSecurityPolicy
{
public:
    ContentSecurityPolicy() = default;

    /// Generate a CSP header string from a configuration.
    [[nodiscard]] static auto generate_policy(const CspConfig& config) -> std::string;

    /// Get a strict production CSP (no inline scripts or styles).
    [[nodiscard]] static auto default_strict_policy() -> CspConfig;

    /// Get a relaxed preview CSP (allows inline styles for markdown preview).
    [[nodiscard]] static auto default_preview_policy() -> CspConfig;

    /// Get a permissive development CSP (allows more sources).
    [[nodiscard]] static auto default_dev_policy() -> CspConfig;

    /// Validate that a directive value is well-formed.
    [[nodiscard]] static auto validate_directive(const std::string& directive) -> bool;

    /// Merge two CSP configs (override values take precedence when non-empty).
    [[nodiscard]] static auto merge_policies(const CspConfig& base,
                                             const CspConfig& override_config) -> CspConfig;

    /// Get the CSP as a <meta> tag suitable for embedding in HTML.
    [[nodiscard]] static auto as_meta_tag(const CspConfig& config) -> std::string;

    /// Count the number of active directives in a policy.
    [[nodiscard]] static auto directive_count(const CspConfig& config) -> int32_t;
};

} // namespace markamp::core
