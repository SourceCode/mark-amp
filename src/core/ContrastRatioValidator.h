#pragma once

#include <wx/colour.h>

#include <string>

namespace markamp::core
{

/// Validates contrast ratios between foreground and background colors according to WCAG 2.1
/// guidelines.
class ContrastRatioValidator
{
public:
    enum class ComplianceLevel
    {
        Fail, // Contrast ratio < 4.5:1 (normal text) or < 3.0:1 (large text)
        AA,   // Contrast ratio >= 4.5:1 (normal text) or >= 3.0:1 (large text)
        AAA   // Contrast ratio >= 7.0:1 (normal text) or >= 4.5:1 (large text)
    };

    struct ValidationResult
    {
        double ratio;
        ComplianceLevel normal_text;
        ComplianceLevel large_text;

        [[nodiscard]] auto is_aa_compliant() const -> bool
        {
            return normal_text >= ComplianceLevel::AA;
        }
        [[nodiscard]] auto is_aaa_compliant() const -> bool
        {
            return normal_text >= ComplianceLevel::AAA;
        }
    };

    /// Calculates the contrast ratio between a foreground and background color.
    [[nodiscard]] static auto calculate_ratio(const wxColour& foreground,
                                              const wxColour& background) -> double;

    /// Validates the contrast between a foreground and background color.
    [[nodiscard]] static auto validate(const wxColour& foreground, const wxColour& background)
        -> ValidationResult;

private:
    /// Calculates the relative luminance of a color according to WCAG 2.1.
    [[nodiscard]] static auto get_relative_luminance(const wxColour& color) -> double;

    /// Helper to convert a single sRGB channel to linear luminance.
    [[nodiscard]] static auto srgb_to_linear(double channel) -> double;
};

} // namespace markamp::core
