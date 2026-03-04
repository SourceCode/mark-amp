#pragma once

/**
 * @file CanvasColorToken.h
 * @brief Phase 46 Task 1,4: Semantic color tokens and contrast checking.
 *
 * Semantic roles (fill/stroke/text/accent/background), color tokens
 * with RGBA and semantic reference, WCAG contrast ratio calculation.
 */

#include <cstdint>
#include <string>

namespace markamp::canvas
{

/// Semantic color role.
enum class ColorRole : uint8_t
{
    kFill,
    kStroke,
    kText,
    kAccent,
    kBackground,
};

/// A color token with RGBA and optional semantic role.
struct CanvasColorToken
{
    std::string name;
    uint32_t rgba{0x000000FF};
    ColorRole role{ColorRole::kFill};

    /// Get role as string.
    [[nodiscard]] auto role_name() const -> std::string;

    /// Extract red channel (0-255).
    [[nodiscard]] auto red() const -> int;
    /// Extract green channel (0-255).
    [[nodiscard]] auto green() const -> int;
    /// Extract blue channel (0-255).
    [[nodiscard]] auto blue() const -> int;
    /// Extract alpha channel (0-255).
    [[nodiscard]] auto alpha() const -> int;

    /// Compute relative luminance (0.0-1.0) per WCAG 2.0.
    [[nodiscard]] auto luminance() const -> double;

    /// Compute WCAG contrast ratio between two colors (1.0-21.0).
    [[nodiscard]] static auto contrast_ratio(const CanvasColorToken& foreground,
                                             const CanvasColorToken& background) -> double;

    /// Check if contrast meets WCAG AA threshold (>= 4.5:1).
    [[nodiscard]] static auto meets_wcag_aa(const CanvasColorToken& foreground,
                                            const CanvasColorToken& background) -> bool;
};

} // namespace markamp::canvas
