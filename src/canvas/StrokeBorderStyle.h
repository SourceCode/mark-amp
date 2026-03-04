#pragma once

/**
 * @file StrokeBorderStyle.h
 * @brief Phase 47 Task 1: Stroke dash patterns, cap/join styles, border sides.
 */

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Dash pattern type.
enum class DashPattern : uint8_t
{
    kSolid,
    kDashed,
    kDotted,
    kDashDot,
};

/// Line cap style.
enum class LineCap : uint8_t
{
    kButt,
    kRound,
    kSquare,
};

/// Line join style.
enum class LineJoin : uint8_t
{
    kMiter,
    kRound,
    kBevel,
};

/// Border side flags.
enum class BorderSide : uint8_t
{
    kNone = 0,
    kTop = 1 << 0,
    kBottom = 1 << 1,
    kLeft = 1 << 2,
    kRight = 1 << 3,
    kAll = kTop | kBottom | kLeft | kRight,
};

/// Allow bitwise | and & on BorderSide.
inline auto operator|(BorderSide lhs, BorderSide rhs) -> BorderSide
{
    return static_cast<BorderSide>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}

inline auto operator&(BorderSide lhs, BorderSide rhs) -> BorderSide
{
    return static_cast<BorderSide>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}

/// Complete stroke/border style.
struct StrokeBorderStyle
{
    double width{2.0};
    uint32_t color_rgba{0x000000FF};
    DashPattern dash{DashPattern::kSolid};
    LineCap cap{LineCap::kButt};
    LineJoin join{LineJoin::kMiter};
    BorderSide sides{BorderSide::kAll};

    /// Get dash pattern as string.
    [[nodiscard]] auto dash_name() const -> std::string;

    /// Get cap style as string.
    [[nodiscard]] auto cap_name() const -> std::string;

    /// Get join style as string.
    [[nodiscard]] auto join_name() const -> std::string;

    /// Check if a specific side is enabled.
    [[nodiscard]] auto has_side(BorderSide side) const -> bool;

    /// Get dash array for rendering (lengths in px).
    [[nodiscard]] auto dash_array() const -> std::vector<double>;
};

} // namespace markamp::canvas
