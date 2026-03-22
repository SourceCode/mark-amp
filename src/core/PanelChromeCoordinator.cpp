#include "PanelChromeCoordinator.h"

namespace markamp::core
{

auto PanelChromeCoordinator::panel_header_config(const std::string& /*panel_id*/) const
    -> PanelHeaderConfig
{
    // V22 Phase 03: All panel headers share a unified configuration.
    // Future: allow per-panel overrides via a registry map.
    PanelHeaderConfig config;
    config.height = 30;
    config.title_font = TypeScaleToken::kSubtitle;
    config.corner = CornerRadiusToken::kNone;
    config.border = BorderWeightToken::kThin;
    config.action_button_size = 20;
    config.action_button_gap = 4;
    config.title_padding_left = 12;
    return config;
}

auto PanelChromeCoordinator::loading_state_config(const std::string& /*panel_id*/) const
    -> LoadingStateConfig
{
    LoadingStateConfig config;
    config.skeleton_style = "shimmer";
    config.skeleton_rows = 3;
    config.row_height = 20;
    config.animation_duration_ms = 1200;
    return config;
}

auto PanelChromeCoordinator::empty_state_config(const std::string& panel_id) const
    -> EmptyStateConfig
{
    EmptyStateConfig config;
    config.icon_size = IconMetricToken::kHero;
    config.title_font = TypeScaleToken::kSubtitle;
    config.subtitle_font = TypeScaleToken::kBody;

    if (panel_id == "explorer")
    {
        config.icon_name = "activity-explorer";
        config.title = "No Folder Open";
        config.subtitle = "Open a folder to start editing";
        config.action_label = "Open Folder";
    }
    else if (panel_id == "search")
    {
        config.icon_name = "activity-search";
        config.title = "Search";
        config.subtitle = "Type to search across your workspace";
        config.action_label = "";
    }
    else if (panel_id == "graph")
    {
        config.icon_name = "activity-graph";
        config.title = "Knowledge Graph";
        config.subtitle = "Open a workspace with linked notes to visualize connections";
        config.action_label = "Open Folder";
    }
    else
    {
        config.icon_name = "activity-explorer";
        config.title = "No Content";
        config.subtitle = "Nothing to display";
        config.action_label = "";
    }

    return config;
}

} // namespace markamp::core
