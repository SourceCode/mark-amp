#pragma once

#include "VisualLanguageTokens.h"

#include <string>

namespace markamp::core
{

/// V22 Phase 03: Panel chrome coordination service.
///
/// Provides standardized layout and styling configuration for panel headers,
/// sidebar sections, and loading/empty state visuals.
class PanelChromeCoordinator
{
public:
    /// Panel header layout/style configuration.
    struct PanelHeaderConfig
    {
        int height{30};
        TypeScaleToken title_font{TypeScaleToken::kSubtitle};
        CornerRadiusToken corner{CornerRadiusToken::kNone};
        BorderWeightToken border{BorderWeightToken::kThin};
        int action_button_size{20};
        int action_button_gap{4};
        int title_padding_left{12};
    };

    /// Sidebar section rhythm rules.
    struct SidebarRhythmConfig
    {
        int section_gap{12};         /// Gap between sidebar sections
        int section_header_height{24}; /// Section header row height
        int section_indent{8};       /// Left indent for section content
        TypeScaleToken section_font{TypeScaleToken::kCaption};
        bool show_section_separator{true};
    };

    /// Loading state visual configuration.
    struct LoadingStateConfig
    {
        std::string skeleton_style; /// "pulse" | "shimmer" | "spinner"
        int skeleton_rows{3};
        int row_height{20};
        int animation_duration_ms{1200};
    };

    /// Empty state visual configuration.
    struct EmptyStateConfig
    {
        std::string icon_name;
        std::string title;
        std::string subtitle;
        std::string action_label;
        TypeScaleToken title_font{TypeScaleToken::kSubtitle};
        TypeScaleToken subtitle_font{TypeScaleToken::kBody};
        IconMetricToken icon_size{IconMetricToken::kHero};
    };

    PanelChromeCoordinator() = default;

    /// Get panel header config for a given panel.
    [[nodiscard]] auto panel_header_config(const std::string& panel_id) const -> PanelHeaderConfig;

    /// Get sidebar section rhythm rules.
    [[nodiscard]] auto sidebar_section_rhythm() const -> SidebarRhythmConfig
    {
        return rhythm_;
    }

    /// Get loading state config for a panel.
    [[nodiscard]] auto loading_state_config(const std::string& panel_id) const -> LoadingStateConfig;

    /// Get empty state config for a panel.
    [[nodiscard]] auto empty_state_config(const std::string& panel_id) const -> EmptyStateConfig;

    /// Set sidebar rhythm configuration.
    void set_sidebar_rhythm(const SidebarRhythmConfig& config)
    {
        rhythm_ = config;
    }

private:
    SidebarRhythmConfig rhythm_;
};

} // namespace markamp::core
