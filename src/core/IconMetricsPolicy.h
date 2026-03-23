/// @file IconMetricsPolicy.h
/// @brief P10-T04 + V27-P03-T02: Standardized icon sizing, states, and theme compatibility.
///
/// Shared icon size tokens by component family, state color rules,
/// and high-DPI scaling factor management. V27 adds families for notebook,
/// canvas, settings, and empty state surfaces.
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
    // V27 additions
    kNotebook,         ///< V27: notebook cell and toolbar icons
    kCanvas,           ///< V27: canvas tool palette and object icons
    kSettings,         ///< V27: settings category and row icons
    kEmptyState,       ///< V27: empty state hero icons
    kBreadcrumb,       ///< V27: breadcrumb path icons
    kCommandPalette,   ///< V27: command palette result icons
    kNotification,     ///< V27: notification severity icons
    kStartup,          ///< V27: startup/welcome surface icons
};

/// Total number of icon component families.
[[nodiscard]] constexpr auto icon_component_family_count() noexcept -> int { return 16; }

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

    // ── V27 additions ─────────────────────────────────────────────────────

    /// V27: Get metrics for V27-specific component families.
    [[nodiscard]] auto v27_metrics_for(IconComponentFamily family) const -> IconMetrics;

private:
    double scale_factor_{1.0};
};

} // namespace markamp::core
