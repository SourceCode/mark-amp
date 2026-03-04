#pragma once

/**
 * @file DpiScalingController.h
 * @brief Phase 36 Task 1: DPI scale factor management and metric scaling.
 *
 * Manages display scale factor (100%-300%), scales control metrics
 * (sizes, padding, icons, fonts), and handles live DPI changes.
 */

#include <cstdint>
#include <string>

namespace markamp::ui
{

/// Platform identifier for default scaling.
enum class Platform : uint8_t
{
    kMacOS,
    kWindows,
    kLinux,
};

/// Scaled metric result.
struct ScaledMetric
{
    int base_value{0};
    double scale_factor{1.0};
    int scaled_value{0};

    /// Compute the scaled value from base and factor.
    void compute();
};

/**
 * @brief DPI scale factor management and metric scaling.
 */
class DpiScalingController
{
public:
    DpiScalingController() = default;

    // ── Scale factor ───────────────────────────────────────────────

    /// Set the DPI scale factor (1.0 = 100%).
    void set_scale_factor(double factor);

    /// Get the current scale factor.
    [[nodiscard]] auto scale_factor() const -> double;

    /// Set platform for default scale behavior.
    void set_platform(Platform platform);

    /// Get the current platform.
    [[nodiscard]] auto platform() const -> Platform;

    /// Get the default scale factor for the current platform.
    [[nodiscard]] auto platform_default_scale() const -> double;

    // ── Metric scaling ─────────────────────────────────────────────

    /// Scale a pixel value.
    [[nodiscard]] auto scale_px(int base_px) const -> int;

    /// Scale a font size.
    [[nodiscard]] auto scale_font(double base_pt) const -> double;

    /// Scale padding.
    [[nodiscard]] auto scale_padding(int base_px) const -> int;

    /// Scale an icon size.
    [[nodiscard]] auto scale_icon(int base_px) const -> int;

    /// Get a ScaledMetric for a base value.
    [[nodiscard]] auto metric(int base_value) const -> ScaledMetric;

    // ── Live DPI change ────────────────────────────────────────────

    /// Notify of a DPI change. Returns true if scale changed.
    auto on_dpi_changed(double new_factor) -> bool;

    /// Get the number of DPI changes handled.
    [[nodiscard]] auto dpi_change_count() const -> int;

    // ── Bounds ─────────────────────────────────────────────────────

    /// Minimum supported scale factor.
    static constexpr double kMinScale = 1.0;

    /// Maximum supported scale factor.
    static constexpr double kMaxScale = 3.0;

private:
    double scale_factor_{1.0};
    Platform platform_{Platform::kMacOS};
    int dpi_change_count_{0};
};

} // namespace markamp::ui
