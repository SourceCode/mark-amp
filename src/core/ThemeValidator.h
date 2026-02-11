#pragma once

#include "Color.h"
#include "Theme.h"

#include <nlohmann/json_fwd.hpp>

#include <array>
#include <string>
#include <vector>

namespace markamp::core
{

/// Result of theme validation with structured errors and warnings.
struct ValidationResult
{
    bool is_valid{false};
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
};

/// Validates theme JSON and Theme structs against the MarkAmp schema.
///
/// Three validation levels:
/// 1. Structural (errors): required fields, correct types
/// 2. Color validity (errors): parseable color values
/// 3. Contrast (warnings): WCAG AA contrast ratios
class ThemeValidator
{
public:
    /// Validate raw JSON before deserialization.
    [[nodiscard]] auto validate_json(const nlohmann::json& json) const -> ValidationResult;

    /// Validate a deserialized Theme (contrast checks).
    [[nodiscard]] auto validate_theme(const Theme& theme) const -> ValidationResult;

    /// The 10 required CSS color keys in theme JSON.
    static constexpr std::array<const char*, 10> kRequiredColorKeys = {"--bg-app",
                                                                       "--bg-panel",
                                                                       "--bg-header",
                                                                       "--bg-input",
                                                                       "--text-main",
                                                                       "--text-muted",
                                                                       "--accent-primary",
                                                                       "--accent-secondary",
                                                                       "--border-light",
                                                                       "--border-dark"};

    /// Minimum WCAG AA contrast ratio for normal text.
    static constexpr float kMinContrastRatio = 4.5f;

    /// Maximum allowed theme name length.
    static constexpr size_t kMaxNameLength = 100;

    /// Maximum allowed theme ID length.
    static constexpr size_t kMaxIdLength = 64;

    /// Check if a string contains null bytes.
    [[nodiscard]] static auto contains_null_bytes(const std::string& str) -> bool;

    /// Check if a string contains control characters (< 0x20, excluding tab/newline).
    [[nodiscard]] static auto contains_control_chars(const std::string& str) -> bool;

private:
    void check_required_fields(const nlohmann::json& json, ValidationResult& result) const;
    void check_color_values(const nlohmann::json& colors, ValidationResult& result) const;
    void check_contrast_ratios(const Theme& theme, ValidationResult& result) const;
};

} // namespace markamp::core
