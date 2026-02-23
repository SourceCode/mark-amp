#include "ContrastRatioValidator.h"

#include <algorithm>
#include <cmath>

namespace markamp::core
{

auto ContrastRatioValidator::calculate_ratio(const wxColour& foreground, const wxColour& background)
    -> double
{
    double lum1 = get_relative_luminance(foreground);
    double lum2 = get_relative_luminance(background);

    double lighter = std::max(lum1, lum2);
    double darker = std::min(lum1, lum2);

    return (lighter + 0.05) / (darker + 0.05);
}

auto ContrastRatioValidator::validate(const wxColour& foreground, const wxColour& background)
    -> ValidationResult
{
    ValidationResult result;
    result.ratio = calculate_ratio(foreground, background);

    // Normal text thresholds: AA = 4.5, AAA = 7.0
    if (result.ratio >= 7.0)
    {
        result.normal_text = ComplianceLevel::AAA;
    }
    else if (result.ratio >= 4.5)
    {
        result.normal_text = ComplianceLevel::AA;
    }
    else
    {
        result.normal_text = ComplianceLevel::Fail;
    }

    // Large text thresholds: AA = 3.0, AAA = 4.5
    if (result.ratio >= 4.5)
    {
        result.large_text = ComplianceLevel::AAA;
    }
    else if (result.ratio >= 3.0)
    {
        result.large_text = ComplianceLevel::AA;
    }
    else
    {
        result.large_text = ComplianceLevel::Fail;
    }

    return result;
}

auto ContrastRatioValidator::get_relative_luminance(const wxColour& color) -> double
{
    double r = srgb_to_linear(static_cast<double>(color.Red()) / 255.0);
    double g = srgb_to_linear(static_cast<double>(color.Green()) / 255.0);
    double b = srgb_to_linear(static_cast<double>(color.Blue()) / 255.0);

    return 0.2126 * r + 0.7152 * g + 0.0722 * b;
}

auto ContrastRatioValidator::srgb_to_linear(double channel) -> double
{
    if (channel <= 0.03928)
    {
        return channel / 12.92;
    }
    return std::pow((channel + 0.055) / 1.055, 2.4);
}

} // namespace markamp::core
