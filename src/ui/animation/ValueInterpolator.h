#pragma once

#include <wx/colour.h>
#include <wx/gdicmn.h>

namespace markamp::ui::animation
{

/// A standard interpolation utility for tweening values over time.
/// Uses a type-safe overloading mechanism to support various UI metrics.
class ValueInterpolator
{
public:
    [[nodiscard]] static auto interpolate(float start, float end, float t) -> float;
    [[nodiscard]] static auto interpolate(int start, int end, float t) -> int;
    [[nodiscard]] static auto interpolate(const wxColour& start, const wxColour& end, float t)
        -> wxColour;
    [[nodiscard]] static auto interpolate(const wxPoint& start, const wxPoint& end, float t)
        -> wxPoint;
    [[nodiscard]] static auto interpolate(const wxSize& start, const wxSize& end, float t)
        -> wxSize;
    [[nodiscard]] static auto interpolate(const wxRect& start, const wxRect& end, float t)
        -> wxRect;
};

} // namespace markamp::ui::animation
