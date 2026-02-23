#pragma once

#include <wx/colour.h>

namespace markamp::core
{

/// Simulates how colors appear to individuals with different types of color vision deficiencies.
class ColorBlindnessSimulator
{
public:
    enum class VisionType
    {
        Normal,       // No color vision deficiency
        Protanopia,   // Red-blind (missing red cones)
        Deuteranopia, // Green-blind (missing green cones - most common)
        Tritanopia,   // Blue-blind (missing blue cones - rare)
        Achromatopsia // Total color blindness (monochromacy)
    };

    /// Simulates a color vision deficiency on a single color.
    [[nodiscard]] static auto simulate(const wxColour& color, VisionType type) -> wxColour;
};

} // namespace markamp::core
