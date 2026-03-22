#pragma once

#include "VisualLanguageTokens.h"

#include <string>

namespace markamp::core
{

/// V22 Phase 04: Command surface coordination service.
///
/// Provides standardized dimensions and styling for menus, command palette,
/// context menus, tooltips, and shortcut chips.
class CommandSurfaceCoordinator
{
public:
    /// Menu item dimensions and presentation rules.
    struct MenuItemMetrics
    {
        int height{28};
        int icon_size{16};
        int icon_gap{8};          /// Gap between icon and label
        int padding_h{12};        /// Horizontal content padding
        int separator_height{1};  /// Divider line height
        int separator_margin{4};  /// Vertical margin above/below divider
        int shortcut_gap{24};     /// Gap between label and shortcut
        CornerRadiusToken hover_radius{CornerRadiusToken::kSm};
    };

    /// Command palette result row dimensions.
    struct PaletteResultMetrics
    {
        int height{32};
        int icon_size{16};
        int icon_gap{8};
        int padding_h{12};
        int match_highlight_padding{2};  /// Padding around match highlights
        TypeScaleToken label_font{TypeScaleToken::kBody};
        TypeScaleToken detail_font{TypeScaleToken::kCaption};
        CornerRadiusToken container_radius{CornerRadiusToken::kLg};
        ElevationToken container_elevation{ElevationToken::kHigh};
    };

    /// Context menu dimensions.
    struct ContextMenuMetrics
    {
        int min_width{180};
        int max_width{320};
        int item_height{28};
        int padding_v{4};         /// Vertical padding inside menu container
        CornerRadiusToken corner{CornerRadiusToken::kMd};
        ElevationToken elevation{ElevationToken::kMedium};
        BorderWeightToken border{BorderWeightToken::kThin};
    };

    /// Tooltip dimensions and presentation.
    struct TooltipMetrics
    {
        int max_width{260};
        int padding_h{8};
        int padding_v{4};
        int delay_ms{500};          /// Show delay
        int fade_duration_ms{120};  /// Fade-in duration
        CornerRadiusToken corner{CornerRadiusToken::kSm};
        ElevationToken elevation{ElevationToken::kMedium};
        TypeScaleToken font{TypeScaleToken::kCaption};
    };

    /// Shortcut chip styling tokens.
    struct ShortcutChipStyle
    {
        int padding_h{4};
        int padding_v{2};
        int gap{2};               /// Gap between key segments (e.g., Cmd + S)
        CornerRadiusToken corner{CornerRadiusToken::kSm};
        BorderWeightToken border{BorderWeightToken::kThin};
        TypeScaleToken font{TypeScaleToken::kCaption};
    };

    CommandSurfaceCoordinator() = default;

    /// Menu item metrics.
    [[nodiscard]] auto menu_item_metrics() const -> MenuItemMetrics
    {
        return menu_metrics_;
    }

    /// Command palette result metrics.
    [[nodiscard]] auto palette_result_metrics() const -> PaletteResultMetrics
    {
        return palette_metrics_;
    }

    /// Context menu metrics.
    [[nodiscard]] auto context_menu_metrics() const -> ContextMenuMetrics
    {
        return context_metrics_;
    }

    /// Tooltip metrics.
    [[nodiscard]] auto tooltip_metrics() const -> TooltipMetrics
    {
        return tooltip_metrics_;
    }

    /// Shortcut chip style.
    [[nodiscard]] auto shortcut_chip_style() const -> ShortcutChipStyle
    {
        return shortcut_style_;
    }

private:
    MenuItemMetrics menu_metrics_;
    PaletteResultMetrics palette_metrics_;
    ContextMenuMetrics context_metrics_;
    TooltipMetrics tooltip_metrics_;
    ShortcutChipStyle shortcut_style_;
};

} // namespace markamp::core
