#pragma once

#include "VisualLanguageTokens.h"

#include <cstdint>
#include <string>

namespace markamp::core
{

/// V22 Phase 13: Settings surface coordination service.
///
/// Provides standardized metrics for settings panels, preference rows,
/// keybinding editor, theme gallery, and settings-state communication.
class SettingsSurfaceCoordinator
{
public:
    /// Settings category navigation metrics.
    struct CategoryNavMetrics
    {
        int nav_width{200};
        int category_row_height{28};
        int category_indent{16};
        int icon_size{16};
        int icon_gap{8};
        TypeScaleToken category_font{TypeScaleToken::kBody};
        TypeScaleToken group_font{TypeScaleToken::kSubtitle};
    };

    /// Settings row / control metrics.
    struct SettingsRowMetrics
    {
        int row_min_height{36};
        int label_width{240};
        int description_max_width{600};
        int control_gap{16};
        int toggle_width{36};
        int toggle_height{20};
        int input_width{200};
        int dropdown_width{200};
        TypeScaleToken label_font{TypeScaleToken::kBody};
        TypeScaleToken description_font{TypeScaleToken::kCaption};
        CornerRadiusToken input_corner{CornerRadiusToken::kSm};
        CornerRadiusToken toggle_corner{CornerRadiusToken::kPill};
    };

    /// Keybinding editor metrics.
    struct KeybindingEditorMetrics
    {
        int row_height{28};
        int key_chip_height{22};
        int key_chip_padding_h{6};
        int key_chip_gap{2};
        int search_input_height{28};
        CornerRadiusToken key_chip_corner{CornerRadiusToken::kSm};
        BorderWeightToken key_chip_border{BorderWeightToken::kThin};
        TypeScaleToken key_font{TypeScaleToken::kCaption};
        TypeScaleToken command_font{TypeScaleToken::kBody};
    };

    /// Theme gallery card metrics.
    struct ThemeGalleryMetrics
    {
        int card_width{200};
        int card_height{140};
        int preview_height{100};
        int label_height{28};
        int card_gap{12};
        CornerRadiusToken card_corner{CornerRadiusToken::kMd};
        ElevationToken card_elevation{ElevationToken::kLow};
        BorderWeightToken card_border{BorderWeightToken::kThin};
        TypeScaleToken label_font{TypeScaleToken::kBody};
    };

    SettingsSurfaceCoordinator() = default;

    [[nodiscard]] auto category_nav_metrics() const -> CategoryNavMetrics
    {
        return nav_;
    }

    [[nodiscard]] auto settings_row_metrics() const -> SettingsRowMetrics
    {
        return row_;
    }

    [[nodiscard]] auto keybinding_editor_metrics() const -> KeybindingEditorMetrics
    {
        return keybinding_;
    }

    [[nodiscard]] auto theme_gallery_metrics() const -> ThemeGalleryMetrics
    {
        return gallery_;
    }

private:
    CategoryNavMetrics nav_;
    SettingsRowMetrics row_;
    KeybindingEditorMetrics keybinding_;
    ThemeGalleryMetrics gallery_;
};

} // namespace markamp::core
