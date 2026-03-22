#pragma once

#include "VisualLanguageTokens.h"

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// V22 Phase 11: Sidebar panel coordination service.
///
/// Provides standardized metrics for explorer, search, outline, backlinks,
/// graph, and inspector sidebar panels.
class SidebarPanelCoordinator
{
public:
    /// Explorer section metrics.
    struct ExplorerSectionMetrics
    {
        int section_header_height{24};
        int file_row_height{22};
        int indent_per_level{16};
        int icon_size{16};
        int icon_gap{6};
        int badge_size{18};
        int badge_gap{4};
        TypeScaleToken file_font{TypeScaleToken::kBody};
        TypeScaleToken section_font{TypeScaleToken::kCaption};
    };

    /// Search sidebar metrics.
    struct SearchSidebarMetrics
    {
        int input_height{28};
        int result_file_height{24};
        int result_match_height{20};
        int match_indent{16};
        int replace_row_height{28};
        int action_bar_height{28};
        TypeScaleToken input_font{TypeScaleToken::kBody};
        TypeScaleToken file_font{TypeScaleToken::kBody};
        TypeScaleToken match_font{TypeScaleToken::kCaption};
        CornerRadiusToken input_corner{CornerRadiusToken::kSm};
    };

    /// Metadata / inspector card metrics.
    struct MetadataCardMetrics
    {
        int card_padding{8};
        int chip_height{22};
        int chip_padding_h{8};
        int chip_gap{4};
        int label_width{80};
        int value_gap{8};
        CornerRadiusToken chip_corner{CornerRadiusToken::kSm};
        CornerRadiusToken card_corner{CornerRadiusToken::kMd};
        TypeScaleToken label_font{TypeScaleToken::kCaption};
        TypeScaleToken value_font{TypeScaleToken::kBody};
    };

    /// Panel action bar metrics (inline actions in panel headers/sections).
    struct PanelActionMetrics
    {
        int action_button_size{20};
        int action_button_gap{2};
        int badge_min_width{16};
        int badge_dot_size{6};
        CornerRadiusToken badge_corner{CornerRadiusToken::kPill};
        TypeScaleToken badge_font{TypeScaleToken::kCaption};
    };

    SidebarPanelCoordinator() = default;

    [[nodiscard]] auto explorer_section_metrics() const -> ExplorerSectionMetrics
    {
        return explorer_;
    }

    [[nodiscard]] auto search_sidebar_metrics() const -> SearchSidebarMetrics
    {
        return search_;
    }

    [[nodiscard]] auto metadata_card_metrics() const -> MetadataCardMetrics
    {
        return metadata_;
    }

    [[nodiscard]] auto panel_action_metrics() const -> PanelActionMetrics
    {
        return actions_;
    }

private:
    ExplorerSectionMetrics explorer_;
    SearchSidebarMetrics search_;
    MetadataCardMetrics metadata_;
    PanelActionMetrics actions_;
};

} // namespace markamp::core
