#pragma once
#include <wx/colour.h>

namespace markamp::ui
{

struct HSLColor
{
    float h{0.0f}; // [0, 360)
    float s{0.0f}; // [0, 1]
    float l{0.0f}; // [0, 1]
    float a{1.0f}; // [0, 1]
};

// Converts wxColour (RGB) to HSL space
auto rgb_to_hsl(const wxColour& c) -> HSLColor;

// Converts HSL space back to wxColour (RGB)
auto hsl_to_rgb(const HSLColor& hsl) -> wxColour;

// Lightens a color by increasing its lightness (amount is 0.0 to 1.0)
auto lighten(const wxColour& c, float amount) -> wxColour;

// Darkens a color by decreasing its lightness (amount is 0.0 to 1.0)
auto darken(const wxColour& c, float amount) -> wxColour;

// Desaturates a color by decreasing its saturation (amount is 0.0 to 1.0)
auto desaturate(const wxColour& c, float amount) -> wxColour;

// Returns a new colour with the specified alpha [0.0 to 1.0] applied
auto with_alpha(const wxColour& c, float alpha) -> wxColour;

// Calculates WCAG 2.1 contrast ratio between foreground and background. returns [1, 21]
auto contrast_ratio(const wxColour& fg, const wxColour& bg) -> float;

// Returns true if contrast ratio >= 4.5
auto meets_wcag_aa(const wxColour& fg, const wxColour& bg) -> bool;

// Returns true if contrast ratio >= 7.0
auto meets_wcag_aaa(const wxColour& fg, const wxColour& bg) -> bool;

} // namespace markamp::ui
