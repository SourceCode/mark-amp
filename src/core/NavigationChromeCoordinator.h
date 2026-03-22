#pragma once

#include "VisualLanguageTokens.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// V22 Phase 05: Navigation chrome coordination service.
///
/// Provides standardized metrics for tabs, breadcrumbs, navigation history,
/// and wayfinding parity checking.
class NavigationChromeCoordinator
{
public:
    /// Tab state for per-state metrics resolution.
    enum class TabState : uint8_t
    {
        kActive,
        kInactive,
        kModified,
        kPinned,
        kPreview,
        kDragging
    };

    /// Per-state tab dimensions and styling.
    struct TabMetrics
    {
        int min_width{80};
        int max_width{200};
        int height{36};
        int close_button_size{16};
        int close_button_margin{4};
        int icon_size{16};
        int icon_gap{6};
        int padding_h{12};
        TypeScaleToken label_font{TypeScaleToken::kBody};
        CornerRadiusToken corner{CornerRadiusToken::kNone};
        BorderWeightToken active_indicator{BorderWeightToken::kMedium};
    };

    /// Breadcrumb segment dimensions.
    struct BreadcrumbMetrics
    {
        int height{22};
        int segment_padding_h{6};
        int separator_width{16};  /// Width of the separator character/icon
        int icon_size{14};
        int icon_gap{4};
        TypeScaleToken font{TypeScaleToken::kCaption};
    };

    /// Navigation history button config.
    struct NavigationHistoryConfig
    {
        int button_size{24};
        int button_gap{2};
        CornerRadiusToken corner{CornerRadiusToken::kSm};
        IconMetricToken icon_size{IconMetricToken::kSmall};
    };

    /// Wayfinding parity finding.
    struct ParityFinding
    {
        std::string surface;      /// "notebook" | "canvas" | "editor"
        std::string description;  /// What is inconsistent
        std::string suggestion;   /// How to fix
    };

    NavigationChromeCoordinator() = default;

    /// Tab metrics for a specific state.
    [[nodiscard]] auto tab_metrics(TabState state) const -> TabMetrics;

    /// Breadcrumb metrics.
    [[nodiscard]] auto breadcrumb_metrics() const -> BreadcrumbMetrics
    {
        return breadcrumb_metrics_;
    }

    /// Navigation history config.
    [[nodiscard]] auto navigation_history_config() const -> NavigationHistoryConfig
    {
        return nav_history_;
    }

    /// Check wayfinding parity across surfaces.
    [[nodiscard]] auto wayfinding_parity_check() const -> std::vector<ParityFinding>;

private:
    BreadcrumbMetrics breadcrumb_metrics_;
    NavigationHistoryConfig nav_history_;
};

} // namespace markamp::core
