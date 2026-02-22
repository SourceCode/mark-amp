#include "ColorUtils.h"

#include <algorithm>
#include <cmath>

namespace markamp::ui
{

namespace
{

float srgb_to_linear(float c)
{
    if (c <= 0.03928f)
        return c / 12.92f;
    return std::pow((c + 0.055f) / 1.055f, 2.4f);
}

// Calculate relative luminance for contrast ratio
float luminance(const wxColour& c)
{
    float r = srgb_to_linear(c.Red() / 255.0f);
    float g = srgb_to_linear(c.Green() / 255.0f);
    float b = srgb_to_linear(c.Blue() / 255.0f);
    return 0.2126f * r + 0.7152f * g + 0.0722f * b;
}

float hue_to_rgb(float p, float q, float t)
{
    if (t < 0.0f)
        t += 1.0f;
    if (t > 1.0f)
        t -= 1.0f;
    if (t < 1.0f / 6.0f)
        return p + (q - p) * 6.0f * t;
    if (t < 1.0f / 2.0f)
        return q;
    if (t < 2.0f / 3.0f)
        return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
    return p;
}

} // namespace

auto rgb_to_hsl(const wxColour& c) -> HSLColor
{
    float r = c.Red() / 255.0f;
    float g = c.Green() / 255.0f;
    float b = c.Blue() / 255.0f;

    float max_val = std::max({r, g, b});
    float min_val = std::min({r, g, b});

    HSLColor hsl;
    hsl.a = c.Alpha() / 255.0f;
    hsl.l = (max_val + min_val) / 2.0f;

    if (max_val == min_val)
    {
        hsl.h = 0.0f; // achromatic
        hsl.s = 0.0f;
    }
    else
    {
        float d = max_val - min_val;
        hsl.s = hsl.l > 0.5f ? d / (2.0f - max_val - min_val) : d / (max_val + min_val);

        if (max_val == r)
        {
            hsl.h = (g - b) / d + (g < b ? 6.0f : 0.0f);
        }
        else if (max_val == g)
        {
            hsl.h = (b - r) / d + 2.0f;
        }
        else if (max_val == b)
        {
            hsl.h = (r - g) / d + 4.0f;
        }
        hsl.h *= 60.0f;
    }

    return hsl;
}

auto hsl_to_rgb(const HSLColor& hsl) -> wxColour
{
    float r, g, b;

    if (hsl.s == 0.0f)
    {
        r = g = b = hsl.l; // achromatic
    }
    else
    {
        float q = hsl.l < 0.5f ? hsl.l * (1.0f + hsl.s) : hsl.l + hsl.s - hsl.l * hsl.s;
        float p = 2.0f * hsl.l - q;
        float hk = hsl.h / 360.0f;

        r = hue_to_rgb(p, q, hk + 1.0f / 3.0f);
        g = hue_to_rgb(p, q, hk);
        b = hue_to_rgb(p, q, hk - 1.0f / 3.0f);
    }

    return wxColour(static_cast<unsigned char>(std::round(r * 255.0f)),
                    static_cast<unsigned char>(std::round(g * 255.0f)),
                    static_cast<unsigned char>(std::round(b * 255.0f)),
                    static_cast<unsigned char>(std::round(hsl.a * 255.0f)));
}

auto lighten(const wxColour& c, float amount) -> wxColour
{
    HSLColor hsl = rgb_to_hsl(c);
    hsl.l = std::clamp(hsl.l + amount, 0.0f, 1.0f);
    return hsl_to_rgb(hsl);
}

auto darken(const wxColour& c, float amount) -> wxColour
{
    HSLColor hsl = rgb_to_hsl(c);
    hsl.l = std::clamp(hsl.l - amount, 0.0f, 1.0f);
    return hsl_to_rgb(hsl);
}

auto desaturate(const wxColour& c, float amount) -> wxColour
{
    HSLColor hsl = rgb_to_hsl(c);
    hsl.s = std::clamp(hsl.s - amount, 0.0f, 1.0f);
    return hsl_to_rgb(hsl);
}

auto with_alpha(const wxColour& c, float alpha) -> wxColour
{
    wxColour res = c;
    res.Set(c.Red(),
            c.Green(),
            c.Blue(),
            static_cast<unsigned char>(std::clamp(alpha * 255.0f, 0.0f, 255.0f)));
    return res;
}

auto contrast_ratio(const wxColour& fg, const wxColour& bg) -> float
{
    float l1 = luminance(fg);
    float l2 = luminance(bg);
    if (l1 < l2)
        std::swap(l1, l2);
    return (l1 + 0.05f) / (l2 + 0.05f);
}

auto meets_wcag_aa(const wxColour& fg, const wxColour& bg) -> bool
{
    return contrast_ratio(fg, bg) >= 4.5f;
}

auto meets_wcag_aaa(const wxColour& fg, const wxColour& bg) -> bool
{
    return contrast_ratio(fg, bg) >= 7.0f;
}

} // namespace markamp::ui
