/// @file IconMetricsPolicy.cpp
/// @brief P10-T04: Standardized icon sizing, states, and theme compatibility.

#include "IconMetricsPolicy.h"

namespace markamp::core
{

IconMetricsPolicy::IconMetricsPolicy() = default;

auto IconMetricsPolicy::metrics_for(IconComponentFamily family) const -> IconMetrics
{
    switch (family)
    {
    case IconComponentFamily::kActivityBar:
        return {24, 8, 1.0, 0.4, 0.8};
    case IconComponentFamily::kToolbar:
        return {20, 4, 1.0, 0.4, 0.8};
    case IconComponentFamily::kTabBar:
        return {14, 2, 1.0, 0.4, 0.8};
    case IconComponentFamily::kFileTree:
        return {16, 2, 1.0, 0.4, 0.8};
    case IconComponentFamily::kPanelHeader:
        return {16, 4, 1.0, 0.4, 0.8};
    case IconComponentFamily::kStatusBar:
        return {14, 2, 1.0, 0.4, 0.8};
    case IconComponentFamily::kContextMenu:
        return {16, 4, 1.0, 0.4, 0.8};
    case IconComponentFamily::kDialog:
        return {24, 8, 1.0, 0.4, 0.8};
    // V27 surface-specific families
    case IconComponentFamily::kNotebook:
        return {16, 4, 1.0, 0.4, 0.8};
    case IconComponentFamily::kCanvas:
        return {18, 6, 1.0, 0.4, 0.8};
    case IconComponentFamily::kSettings:
        return {16, 4, 1.0, 0.4, 0.8};
    case IconComponentFamily::kEmptyState:
        return {48, 16, 1.0, 0.4, 0.8};
    case IconComponentFamily::kBreadcrumb:
        return {14, 2, 1.0, 0.4, 0.8};
    case IconComponentFamily::kCommandPalette:
        return {16, 4, 1.0, 0.4, 0.8};
    case IconComponentFamily::kNotification:
        return {16, 4, 1.0, 0.4, 0.8};
    case IconComponentFamily::kStartup:
        return {48, 16, 1.0, 0.4, 0.8};
    }
    return {16, 4, 1.0, 0.4, 0.8};
}

auto IconMetricsPolicy::effective_size(IconComponentFamily family) const -> int
{
    const auto m = metrics_for(family);
    return static_cast<int>(static_cast<double>(m.size_px) * scale_factor_);
}

} // namespace markamp::core
