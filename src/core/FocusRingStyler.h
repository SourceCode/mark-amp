// ============================================================================
// File: src/core/FocusRingStyler.h
// Phase 31: Accessibility System — Focus ring appearance coordinator
// ============================================================================
#pragma once

#include <cstdint>
#include <string>

namespace markamp::core
{

/// Visual style for a focus ring drawn around focused elements.
struct FocusRingStyle
{
    int width_px{2};          ///< Ring width in pixels
    int offset_px{2};         ///< Offset from element boundary
    uint32_t color{0x0078D4}; ///< Ring color (RGB, default: blue accent)
    int corner_radius_px{2};  ///< Corner radius for rounded rectangles
    float opacity{1.0F};      ///< Ring opacity (0.0–1.0)
};

/// Minimum touch/click target sizes per accessibility guidelines.
struct TouchTargetSize
{
    int min_width_px{44};  ///< Minimum interactive width
    int min_height_px{44}; ///< Minimum interactive height
};

/// FocusRingStyler — coordinates focus ring appearance across the application.
///
/// Phase 31 Tasks 6, 12: Focus ring styling with keyboard-only visibility,
/// high contrast override, and minimum touch target enforcement.
class FocusRingStyler
{
public:
    FocusRingStyler();

    // ── Style computation ──────────────────────────────────────────

    /// Compute the appropriate focus ring style given the current state.
    /// When keyboard_mode is false, the ring is invisible (width=0).
    /// When high_contrast is true, the ring is thicker (3px) and higher contrast.
    [[nodiscard]] auto compute_ring_style(bool high_contrast, bool keyboard_mode) const
        -> FocusRingStyle;

    /// Get the default (normal) focus ring style.
    [[nodiscard]] auto default_style() const -> FocusRingStyle;

    /// Get the high contrast focus ring style.
    [[nodiscard]] auto high_contrast_style() const -> FocusRingStyle;

    /// Get the invisible (mouse-mode) style.
    [[nodiscard]] static auto invisible_style() -> FocusRingStyle;

    // ── Customization ──────────────────────────────────────────────

    /// Set the accent color for normal focus rings.
    void set_accent_color(uint32_t rgb);

    /// Set the high contrast override color.
    void set_high_contrast_color(uint32_t rgb);

    /// Set the normal ring width.
    void set_ring_width(int width_px);

    /// Set the high contrast ring width.
    void set_high_contrast_width(int width_px);

    /// Set the ring offset from element boundary.
    void set_ring_offset(int offset_px);

    /// Set the corner radius.
    void set_corner_radius(int radius_px);

    // ── Touch target enforcement ───────────────────────────────────

    /// Check if a given size meets the minimum touch target requirements.
    [[nodiscard]] auto meets_touch_target(int width_px, int height_px) const -> bool;

    /// Get the minimum touch target size.
    [[nodiscard]] auto min_touch_target() const -> TouchTargetSize;

    /// Set custom minimum touch target size.
    void set_min_touch_target(int width_px, int height_px);

    // ── Contrast helpers ───────────────────────────────────────────

    /// Compute the WCAG contrast ratio between two sRGB colors (0x00RRGGBB).
    [[nodiscard]] static auto contrast_ratio(uint32_t color_a, uint32_t color_b) -> double;

    /// Check if a focus ring color has sufficient contrast (≥ 3:1) against a background.
    [[nodiscard]] static auto has_sufficient_contrast(uint32_t ring_color, uint32_t bg_color)
        -> bool;

    // ── Constants ──────────────────────────────────────────────────

    static constexpr int kDefaultWidth = 2;
    static constexpr int kHighContrastWidth = 3;
    static constexpr int kDefaultOffset = 2;
    static constexpr int kDefaultCornerRadius = 2;
    static constexpr uint32_t kDefaultAccentColor = 0x0078D4;       // Blue
    static constexpr uint32_t kHighContrastDefaultColor = 0xFFFFFF; // White
    static constexpr double kMinContrastRatio = 3.0;

private:
    uint32_t accent_color_{kDefaultAccentColor};
    uint32_t high_contrast_color_{kHighContrastDefaultColor};
    int ring_width_{kDefaultWidth};
    int high_contrast_width_{kHighContrastWidth};
    int ring_offset_{kDefaultOffset};
    int corner_radius_{kDefaultCornerRadius};
    TouchTargetSize min_touch_target_;
};

} // namespace markamp::core
