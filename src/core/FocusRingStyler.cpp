// ============================================================================
// File: src/core/FocusRingStyler.cpp
// Phase 31: Accessibility System — Focus ring appearance coordinator
// ============================================================================

#include "FocusRingStyler.h"

#include <algorithm>
#include <cmath>

namespace markamp::core
{

FocusRingStyler::FocusRingStyler() = default;

// ── Style computation ──────────────────────────────────────────────────────

auto FocusRingStyler::compute_ring_style(bool high_contrast, bool keyboard_mode) const
    -> FocusRingStyle
{
    if (!keyboard_mode)
    {
        return invisible_style();
    }

    if (high_contrast)
    {
        return high_contrast_style();
    }

    return default_style();
}

auto FocusRingStyler::default_style() const -> FocusRingStyle
{
    FocusRingStyle style;
    style.width_px = ring_width_;
    style.offset_px = ring_offset_;
    style.color = accent_color_;
    style.corner_radius_px = corner_radius_;
    style.opacity = 1.0F;
    return style;
}

auto FocusRingStyler::high_contrast_style() const -> FocusRingStyle
{
    FocusRingStyle style;
    style.width_px = high_contrast_width_;
    style.offset_px = ring_offset_;
    style.color = high_contrast_color_;
    style.corner_radius_px = corner_radius_;
    style.opacity = 1.0F;
    return style;
}

auto FocusRingStyler::invisible_style() -> FocusRingStyle
{
    FocusRingStyle style;
    style.width_px = 0;
    style.offset_px = 0;
    style.color = 0x000000;
    style.corner_radius_px = 0;
    style.opacity = 0.0F;
    return style;
}

// ── Customization ──────────────────────────────────────────────────────────

void FocusRingStyler::set_accent_color(uint32_t rgb)
{
    accent_color_ = rgb & 0x00FFFFFF;
}

void FocusRingStyler::set_high_contrast_color(uint32_t rgb)
{
    high_contrast_color_ = rgb & 0x00FFFFFF;
}

void FocusRingStyler::set_ring_width(int width_px)
{
    ring_width_ = std::max(1, width_px);
}

void FocusRingStyler::set_high_contrast_width(int width_px)
{
    high_contrast_width_ = std::max(1, width_px);
}

void FocusRingStyler::set_ring_offset(int offset_px)
{
    ring_offset_ = std::max(0, offset_px);
}

void FocusRingStyler::set_corner_radius(int radius_px)
{
    corner_radius_ = std::max(0, radius_px);
}

// ── Touch target enforcement ───────────────────────────────────────────────

auto FocusRingStyler::meets_touch_target(int width_px, int height_px) const -> bool
{
    return width_px >= min_touch_target_.min_width_px &&
           height_px >= min_touch_target_.min_height_px;
}

auto FocusRingStyler::min_touch_target() const -> TouchTargetSize
{
    return min_touch_target_;
}

void FocusRingStyler::set_min_touch_target(int width_px, int height_px)
{
    min_touch_target_.min_width_px = std::max(1, width_px);
    min_touch_target_.min_height_px = std::max(1, height_px);
}

// ── Contrast helpers ───────────────────────────────────────────────────────

namespace
{

/// Convert an sRGB channel (0–255) to linear luminance component.
auto srgb_to_linear(double channel) -> double
{
    const double normalized = channel / 255.0;
    if (normalized <= 0.03928)
    {
        return normalized / 12.92;
    }
    return std::pow((normalized + 0.055) / 1.055, 2.4);
}

/// Compute relative luminance per WCAG 2.1.
auto relative_luminance(uint32_t rgb) -> double
{
    const double red = srgb_to_linear(static_cast<double>((rgb >> 16) & 0xFF));
    const double green = srgb_to_linear(static_cast<double>((rgb >> 8) & 0xFF));
    const double blue = srgb_to_linear(static_cast<double>(rgb & 0xFF));
    return 0.2126 * red + 0.7152 * green + 0.0722 * blue;
}

} // namespace

auto FocusRingStyler::contrast_ratio(uint32_t color_a, uint32_t color_b) -> double
{
    const double lum_a = relative_luminance(color_a);
    const double lum_b = relative_luminance(color_b);
    const double lighter = std::max(lum_a, lum_b);
    const double darker = std::min(lum_a, lum_b);
    return (lighter + 0.05) / (darker + 0.05);
}

auto FocusRingStyler::has_sufficient_contrast(uint32_t ring_color, uint32_t bg_color) -> bool
{
    return contrast_ratio(ring_color, bg_color) >= kMinContrastRatio;
}

} // namespace markamp::core
