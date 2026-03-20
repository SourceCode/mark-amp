/// @file IconMetricsPolicy.h
/// @brief P10-T04: Standardized icon sizing, states, and theme compatibility.
///
/// Shared icon size tokens by component family, state color rules,
/// and high-DPI scaling factor management.
#pragma once

#include <string>

namespace markamp::core
{

/// Component family for icon metrics.
enum class IconComponentFamily
{
    kActivityBar,
    kToolbar,
    kTabBar,
    kFileTree,
    kPanelHeader,
    kStatusBar,
    kContextMenu,
    kDialog,
};

/// Icon metrics for a component family.
struct IconMetrics
{
    int size_px{16};
    int padding_px{4};
    double opacity_normal{1.0};
    double opacity_disabled{0.4};
    double opacity_hover{0.8};
};

/// Manages icon metrics across component families.
class IconMetricsPolicy
{
public:
    IconMetricsPolicy();

    /// Get metrics for a component family.
    [[nodiscard]] auto metrics_for(IconComponentFamily family) const -> IconMetrics;

    /// Get the high-DPI scale factor.
    [[nodiscard]] auto scale_factor() const -> double { return scale_factor_; }

    /// Set the high-DPI scale factor.
    void set_scale_factor(double factor) { scale_factor_ = factor; }

    /// Get the effective size for a family (size * scale).
    [[nodiscard]] auto effective_size(IconComponentFamily family) const -> int;

private:
    double scale_factor_{1.0};
};

} // namespace markamp::core
