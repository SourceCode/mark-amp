#include "Color.h"

#include <wx/colour.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fmt/format.h>

namespace markamp::core
{

// --- Hex parsing helpers ---

static auto hex_char_to_nibble(char c) -> std::expected<uint8_t, std::string>
{
    if (c >= '0' && c <= '9')
    {
        return static_cast<uint8_t>(c - '0');
    }
    if (c >= 'a' && c <= 'f')
    {
        return static_cast<uint8_t>(c - 'a' + 10);
    }
    if (c >= 'A' && c <= 'F')
    {
        return static_cast<uint8_t>(c - 'A' + 10);
    }
    return std::unexpected(fmt::format("Invalid hex character: '{}'", c));
}

static auto parse_hex_byte(char hi, char lo) -> std::expected<uint8_t, std::string>
{
    auto h = hex_char_to_nibble(hi);
    if (!h)
    {
        return std::unexpected(h.error());
    }
    auto l = hex_char_to_nibble(lo);
    if (!l)
    {
        return std::unexpected(l.error());
    }
    return static_cast<uint8_t>((*h << 4) | *l);
}

auto Color::from_hex(std::string_view hex) -> std::expected<Color, std::string>
{
    // Strip leading '#'
    if (!hex.empty() && hex[0] == '#')
    {
        hex = hex.substr(1);
    }

    if (hex.empty())
    {
        return std::unexpected(std::string("Empty hex color string"));
    }

    Color result;

    if (hex.size() == 3)
    {
        // #RGB → #RRGGBB
        auto rv = hex_char_to_nibble(hex[0]);
        auto gv = hex_char_to_nibble(hex[1]);
        auto bv = hex_char_to_nibble(hex[2]);
        if (!rv || !gv || !bv)
        {
            return std::unexpected(std::string("Invalid shorthand hex color"));
        }
        result.r = static_cast<uint8_t>((*rv << 4) | *rv);
        result.g = static_cast<uint8_t>((*gv << 4) | *gv);
        result.b = static_cast<uint8_t>((*bv << 4) | *bv);
        result.a = 255;
    }
    else if (hex.size() == 6)
    {
        // #RRGGBB
        auto rv = parse_hex_byte(hex[0], hex[1]);
        auto gv = parse_hex_byte(hex[2], hex[3]);
        auto bv = parse_hex_byte(hex[4], hex[5]);
        if (!rv || !gv || !bv)
        {
            return std::unexpected(std::string("Invalid hex color"));
        }
        result.r = *rv;
        result.g = *gv;
        result.b = *bv;
        result.a = 255;
    }
    else if (hex.size() == 8)
    {
        // #RRGGBBAA
        auto rv = parse_hex_byte(hex[0], hex[1]);
        auto gv = parse_hex_byte(hex[2], hex[3]);
        auto bv = parse_hex_byte(hex[4], hex[5]);
        auto av = parse_hex_byte(hex[6], hex[7]);
        if (!rv || !gv || !bv || !av)
        {
            return std::unexpected(std::string("Invalid hex+alpha color"));
        }
        result.r = *rv;
        result.g = *gv;
        result.b = *bv;
        result.a = *av;
    }
    else
    {
        return std::unexpected(
            fmt::format("Invalid hex length {}: expected 3, 6, or 8 characters", hex.size()));
    }

    return result;
}

auto Color::from_rgba_string(std::string_view rgba) -> std::expected<Color, std::string>
{
    // Format: "rgba(R, G, B, A)" where A is 0.0–1.0
    // Also accept "rgb(R, G, B)"
    auto prefix_end = rgba.find('(');
    if (prefix_end == std::string_view::npos)
    {
        return std::unexpected(std::string("Missing '(' in rgba/rgb string"));
    }
    auto close = rgba.find(')', prefix_end);
    if (close == std::string_view::npos)
    {
        return std::unexpected(std::string("Missing ')' in rgba/rgb string"));
    }

    auto content = rgba.substr(prefix_end + 1, close - prefix_end - 1);

    // Parse comma-separated values using strtof (AppleClang compatible)
    std::array<float, 4> values{0.0f, 0.0f, 0.0f, 1.0f};
    int count = 0;

    // Make a null-terminated copy for strtof
    std::string contentStr(content);
    const char* ptr = contentStr.c_str();
    char* end = nullptr;

    while (*ptr != '\0' && count < 4)
    {
        // Skip whitespace and commas
        while (*ptr == ' ' || *ptr == ',')
        {
            ++ptr;
        }
        if (*ptr == '\0')
        {
            break;
        }

        float val = std::strtof(ptr, &end);
        if (end == ptr)
        {
            return std::unexpected(std::string("Invalid number in rgba string"));
        }

        values[static_cast<size_t>(count)] = val;
        ++count;
        ptr = end;
    }

    if (count < 3)
    {
        return std::unexpected(std::string("rgba/rgb requires at least 3 values"));
    }

    Color result;
    result.r = static_cast<uint8_t>(std::clamp(values[0], 0.0f, 255.0f));
    result.g = static_cast<uint8_t>(std::clamp(values[1], 0.0f, 255.0f));
    result.b = static_cast<uint8_t>(std::clamp(values[2], 0.0f, 255.0f));

    // Alpha: if count == 4, the 4th value is 0.0–1.0
    if (count == 4)
    {
        result.a = static_cast<uint8_t>(std::clamp(values[3] * 255.0f, 0.0f, 255.0f));
    }
    else
    {
        result.a = 255;
    }

    return result;
}

auto Color::from_string(std::string_view str) -> std::expected<Color, std::string>
{
    // Trim whitespace
    while (!str.empty() && str.front() == ' ')
    {
        str = str.substr(1);
    }
    while (!str.empty() && str.back() == ' ')
    {
        str = str.substr(0, str.size() - 1);
    }

    if (str.empty())
    {
        return std::unexpected(std::string("Empty color string"));
    }

    // Detect format
    if (str.starts_with("rgba(") || str.starts_with("rgb("))
    {
        return from_rgba_string(str);
    }

    return from_hex(str);
}

// --- Conversion ---

auto Color::to_hex() const -> std::string
{
    if (a == 255)
    {
        return fmt::format("#{:02x}{:02x}{:02x}", r, g, b);
    }
    return fmt::format("#{:02x}{:02x}{:02x}{:02x}", r, g, b, a);
}

auto Color::to_wx_colour() const -> wxColour
{
    return {r, g, b, a};
}

auto Color::to_rgba_string() const -> std::string
{
    float alpha = static_cast<float>(a) / 255.0f;
    // Round alpha to 1 decimal for clean output
    alpha = std::round(alpha * 10.0f) / 10.0f;
    return fmt::format("rgba({}, {}, {}, {})", r, g, b, alpha);
}

auto Color::to_theme_string() const -> std::string
{
    if (a == 255)
    {
        return to_hex();
    }
    return to_rgba_string();
}

// --- Operations ---

auto Color::with_alpha(float alpha_01) const -> Color
{
    auto clamped = std::clamp(alpha_01, 0.0f, 1.0f);
    return {r, g, b, static_cast<uint8_t>(clamped * 255.0f)};
}

auto Color::blend(const Color& other, float factor) const -> Color
{
    auto f = std::clamp(factor, 0.0f, 1.0f);
    auto inv = 1.0f - f;
    return {static_cast<uint8_t>(static_cast<float>(r) * inv + static_cast<float>(other.r) * f),
            static_cast<uint8_t>(static_cast<float>(g) * inv + static_cast<float>(other.g) * f),
            static_cast<uint8_t>(static_cast<float>(b) * inv + static_cast<float>(other.b) * f),
            static_cast<uint8_t>(static_cast<float>(a) * inv + static_cast<float>(other.a) * f)};
}

auto Color::lighten(float factor) const -> Color
{
    // Lighten by blending with white
    return blend(Color{255, 255, 255, a}, factor);
}

// WCAG 2.1 relative luminance
static auto srgb_to_linear(float channel) -> float
{
    if (channel <= 0.03928f)
    {
        return channel / 12.92f;
    }
    return std::pow((channel + 0.055f) / 1.055f, 2.4f);
}

auto Color::luminance() const -> float
{
    float rLin = srgb_to_linear(static_cast<float>(r) / 255.0f);
    float gLin = srgb_to_linear(static_cast<float>(g) / 255.0f);
    float bLin = srgb_to_linear(static_cast<float>(b) / 255.0f);
    return 0.2126f * rLin + 0.7152f * gLin + 0.0722f * bLin;
}

auto Color::contrast_ratio(const Color& other) const -> float
{
    float l1 = luminance();
    float l2 = other.luminance();
    if (l1 < l2)
    {
        std::swap(l1, l2);
    }
    return (l1 + 0.05f) / (l2 + 0.05f);
}

} // namespace markamp::core
