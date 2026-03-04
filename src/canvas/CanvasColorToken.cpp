#include "CanvasColorToken.h"

#include <algorithm>
#include <cmath>

namespace markamp::canvas
{

auto CanvasColorToken::role_name() const -> std::string
{
    switch (role)
    {
        case ColorRole::kFill:
            return "fill";
        case ColorRole::kStroke:
            return "stroke";
        case ColorRole::kText:
            return "text";
        case ColorRole::kAccent:
            return "accent";
        case ColorRole::kBackground:
            return "background";
    }
    return "unknown";
}

auto CanvasColorToken::red() const -> int
{
    return static_cast<int>((rgba >> 24) & 0xFF);
}

auto CanvasColorToken::green() const -> int
{
    return static_cast<int>((rgba >> 16) & 0xFF);
}

auto CanvasColorToken::blue() const -> int
{
    return static_cast<int>((rgba >> 8) & 0xFF);
}

auto CanvasColorToken::alpha() const -> int
{
    return static_cast<int>(rgba & 0xFF);
}

auto CanvasColorToken::luminance() const -> double
{
    // WCAG 2.0 relative luminance
    auto srgb_to_linear = [](double channel) -> double
    {
        channel /= 255.0;
        if (channel <= 0.04045)
        {
            return channel / 12.92;
        }
        return std::pow((channel + 0.055) / 1.055, 2.4);
    };

    double linear_r = srgb_to_linear(static_cast<double>(red()));
    double linear_g = srgb_to_linear(static_cast<double>(green()));
    double linear_b = srgb_to_linear(static_cast<double>(blue()));

    return 0.2126 * linear_r + 0.7152 * linear_g + 0.0722 * linear_b;
}

auto CanvasColorToken::contrast_ratio(const CanvasColorToken& foreground,
                                      const CanvasColorToken& background) -> double
{
    double lum_fg = foreground.luminance();
    double lum_bg = background.luminance();

    double lighter = std::max(lum_fg, lum_bg);
    double darker = std::min(lum_fg, lum_bg);

    return (lighter + 0.05) / (darker + 0.05);
}

auto CanvasColorToken::meets_wcag_aa(const CanvasColorToken& foreground,
                                     const CanvasColorToken& background) -> bool
{
    return contrast_ratio(foreground, background) >= 4.5;
}

} // namespace markamp::canvas
