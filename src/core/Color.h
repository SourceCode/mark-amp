#pragma once

#include <cstdint>
#include <expected>
#include <string>
#include <string_view>

// Forward-declare wxColour to avoid wx dependency in the header
class wxColour;

namespace markamp::core
{

/// RGBA color type with parsing, conversion, and WCAG accessibility utilities.
struct Color
{
    uint8_t r{0};
    uint8_t g{0};
    uint8_t b{0};
    uint8_t a{255};

    // Constructors
    constexpr Color() = default;
    constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        : r(r)
        , g(g)
        , b(b)
        , a(a)
    {
    }

    // Factory functions
    static auto from_hex(std::string_view hex) -> std::expected<Color, std::string>;
    static auto from_rgba_string(std::string_view rgba) -> std::expected<Color, std::string>;
    static constexpr auto from_rgb(uint8_t r, uint8_t g, uint8_t b) -> Color
    {
        return {r, g, b, 255};
    }

    /// Parse either hex or rgba() format automatically.
    static auto from_string(std::string_view str) -> std::expected<Color, std::string>;

    // Conversion
    [[nodiscard]] auto to_hex() const -> std::string;
    [[nodiscard]] auto to_wx_colour() const -> wxColour;
    [[nodiscard]] auto to_rgba_string() const -> std::string;

    /// Serialize back to the most compact representation.
    /// Uses hex for fully opaque (a == 255) or 3-char-able colors,
    /// rgba() for colours with non-trivial alpha.
    [[nodiscard]] auto to_theme_string() const -> std::string;

    // Operations
    [[nodiscard]] constexpr auto with_alpha(uint8_t alpha) const -> Color
    {
        return {r, g, b, alpha};
    }
    [[nodiscard]] auto with_alpha(float alpha_01) const -> Color;
    [[nodiscard]] auto blend(const Color& other, float factor) const -> Color;
    [[nodiscard]] auto lighten(float factor) const -> Color;

    /// Relative luminance per WCAG 2.1 (0.0 = black, 1.0 = white).
    [[nodiscard]] auto luminance() const -> float;

    /// WCAG contrast ratio (1.0 – 21.0).
    [[nodiscard]] auto contrast_ratio(const Color& other) const -> float;

    // Comparison
    auto operator<=>(const Color&) const = default;
    auto operator==(const Color&) const -> bool = default;
};

} // namespace markamp::core
