#include "ValueInterpolator.h"

#include <algorithm>
#include <cmath>

namespace markamp::ui::animation
{

auto ValueInterpolator::interpolate(float start, float end, float t) -> float
{
    t = std::clamp(t, 0.0f, 1.0f);
    return start + (end - start) * t;
}

auto ValueInterpolator::interpolate(int start, int end, float t) -> int
{
    t = std::clamp(t, 0.0f, 1.0f);
    return start + static_cast<int>(std::round(static_cast<float>(end - start) * t));
}

auto ValueInterpolator::interpolate(const wxColour& start, const wxColour& end, float t) -> wxColour
{
    t = std::clamp(t, 0.0f, 1.0f);
    return wxColour(static_cast<unsigned char>(
                        interpolate(static_cast<int>(start.Red()), static_cast<int>(end.Red()), t)),
                    static_cast<unsigned char>(interpolate(
                        static_cast<int>(start.Green()), static_cast<int>(end.Green()), t)),
                    static_cast<unsigned char>(interpolate(
                        static_cast<int>(start.Blue()), static_cast<int>(end.Blue()), t)),
                    static_cast<unsigned char>(interpolate(
                        static_cast<int>(start.Alpha()), static_cast<int>(end.Alpha()), t)));
}

auto ValueInterpolator::interpolate(const wxPoint& start, const wxPoint& end, float t) -> wxPoint
{
    return wxPoint(interpolate(start.x, end.x, t), interpolate(start.y, end.y, t));
}

auto ValueInterpolator::interpolate(const wxSize& start, const wxSize& end, float t) -> wxSize
{
    return wxSize(interpolate(start.GetWidth(), end.GetWidth(), t),
                  interpolate(start.GetHeight(), end.GetHeight(), t));
}

auto ValueInterpolator::interpolate(const wxRect& start, const wxRect& end, float t) -> wxRect
{
    return wxRect(interpolate(start.GetPosition(), end.GetPosition(), t),
                  interpolate(start.GetSize(), end.GetSize(), t));
}

} // namespace markamp::ui::animation
