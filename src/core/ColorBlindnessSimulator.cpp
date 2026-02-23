#include "ColorBlindnessSimulator.h"

#include <algorithm>
#include <cmath>

namespace markamp::core
{

// Color transformation matrices based on Machado et al. (2009)
// "A Physiologically-based Model for Simulation of Color Vision Deficiency"

auto ColorBlindnessSimulator::simulate(const wxColour& color, VisionType type) -> wxColour
{
    if (type == VisionType::Normal)
    {
        return color;
    }

    double r = static_cast<double>(color.Red()) / 255.0;
    double g = static_cast<double>(color.Green()) / 255.0;
    double b = static_cast<double>(color.Blue()) / 255.0;

    // Convert sRGB to linear RGB
    r = (r <= 0.04045) ? (r / 12.92) : std::pow((r + 0.055) / 1.055, 2.4);
    g = (g <= 0.04045) ? (g / 12.92) : std::pow((g + 0.055) / 1.055, 2.4);
    b = (b <= 0.04045) ? (b / 12.92) : std::pow((b + 0.055) / 1.055, 2.4);

    double rgb[3] = {r, g, b};
    double out[3] = {0.0, 0.0, 0.0};

    // Severity 1.0 (full dichromacy) transformation matrices
    if (type == VisionType::Protanopia)
    {
        // Missing L (red) cones
        out[0] = rgb[0] * 0.152286 + rgb[1] * 1.052583 + rgb[2] * -0.204868;
        out[1] = rgb[0] * 0.114503 + rgb[1] * 0.786281 + rgb[2] * 0.099216;
        out[2] = rgb[0] * -0.003882 + rgb[1] * -0.048116 + rgb[2] * 1.051998;
    }
    else if (type == VisionType::Deuteranopia)
    {
        // Missing M (green) cones
        out[0] = rgb[0] * 0.367322 + rgb[1] * 0.860646 + rgb[2] * -0.227968;
        out[1] = rgb[0] * 0.280085 + rgb[1] * 0.672501 + rgb[2] * 0.047413;
        out[2] = rgb[0] * -0.011820 + rgb[1] * 0.042940 + rgb[2] * 0.968881;
    }
    else if (type == VisionType::Tritanopia)
    {
        // Missing S (blue) cones
        out[0] = rgb[0] * 1.255528 + rgb[1] * -0.076749 + rgb[2] * -0.178779;
        out[1] = rgb[0] * -0.078411 + rgb[1] * 0.930809 + rgb[2] * 0.147602;
        out[2] = rgb[0] * 0.004733 + rgb[1] * 0.691367 + rgb[2] * 0.303900;
    }
    else if (type == VisionType::Achromatopsia)
    {
        // Monochromacy (black & white, luminance only)
        double lum = 0.2126 * rgb[0] + 0.7152 * rgb[1] + 0.0722 * rgb[2];
        out[0] = lum;
        out[1] = lum;
        out[2] = lum;
    }

    // Convert back from linear RGB to sRGB
    for (int i = 0; i < 3; ++i)
    {
        out[i] = std::clamp(out[i], 0.0, 1.0);
        out[i] = (out[i] <= 0.0031308) ? (out[i] * 12.92)
                                       : (1.055 * std::pow(out[i], 1.0 / 2.4) - 0.055);
    }

    auto to_byte = [](double v) -> unsigned char
    { return static_cast<unsigned char>(std::clamp(std::round(v * 255.0), 0.0, 255.0)); };

    return {to_byte(out[0]), to_byte(out[1]), to_byte(out[2]), color.Alpha()};
}

} // namespace markamp::core
