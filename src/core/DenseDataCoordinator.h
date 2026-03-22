#pragma once

#include "VisualLanguageTokens.h"

#include <cstdint>
#include <string>

namespace markamp::core
{

/// V22 Phase 15: Dense data coordination service.
///
/// Provides standardized metrics for trees, lists, tables, AV surfaces,
/// and data-dense row systems with unified selection/hover behaviors.
class DenseDataCoordinator
{
public:
    /// Unified dense-row system metrics.
    struct DenseRowMetrics
    {
        int tree_row_height{22};
        int list_row_height{24};
        int table_row_height{28};
        int indent_per_level{16};
        int checkbox_size{16};
        int checkbox_gap{6};
        int expand_icon_size{12};
        float hover_bg_opacity{0.08F};
        float selection_bg_opacity{0.15F};
        float focus_bg_opacity{0.12F};
        CornerRadiusToken row_hover_corner{CornerRadiusToken::kSm};
    };

    /// Table / AV surface metrics.
    struct TableSurfaceMetrics
    {
        int header_height{32};
        int cell_padding_h{8};
        int cell_padding_v{4};
        int resize_handle_width{4};
        int sort_icon_size{12};
        int filter_bar_height{32};
        int column_min_width{60};
        BorderWeightToken header_border{BorderWeightToken::kThin};
        BorderWeightToken cell_border{BorderWeightToken::kThin};
        TypeScaleToken header_font{TypeScaleToken::kCaption};
        TypeScaleToken cell_font{TypeScaleToken::kBody};
    };

    /// AV gallery card metrics.
    struct GalleryCardMetrics
    {
        int card_min_width{180};
        int card_max_width{280};
        int card_gap{12};
        int cover_height{120};
        int content_padding{8};
        int property_row_height{20};
        CornerRadiusToken card_corner{CornerRadiusToken::kMd};
        ElevationToken card_elevation{ElevationToken::kLow};
        BorderWeightToken card_border{BorderWeightToken::kThin};
    };

    /// AV kanban lane metrics.
    struct KanbanLaneMetrics
    {
        int lane_min_width{260};
        int lane_max_width{320};
        int lane_header_height{36};
        int lane_gap{8};
        int card_gap{6};
        int card_padding{8};
        CornerRadiusToken lane_corner{CornerRadiusToken::kMd};
        CornerRadiusToken card_corner{CornerRadiusToken::kSm};
        TypeScaleToken header_font{TypeScaleToken::kSubtitle};
        TypeScaleToken card_font{TypeScaleToken::kBody};
    };

    /// Table overlay / filter surface metrics.
    struct TableOverlayMetrics
    {
        int filter_chip_height{24};
        int filter_chip_padding_h{8};
        int filter_chip_gap{4};
        int sort_menu_width{200};
        int column_picker_width{240};
        CornerRadiusToken chip_corner{CornerRadiusToken::kSm};
        TypeScaleToken chip_font{TypeScaleToken::kCaption};
    };

    DenseDataCoordinator() = default;

    [[nodiscard]] auto dense_row_metrics() const -> DenseRowMetrics
    {
        return rows_;
    }

    [[nodiscard]] auto table_surface_metrics() const -> TableSurfaceMetrics
    {
        return table_;
    }

    [[nodiscard]] auto gallery_card_metrics() const -> GalleryCardMetrics
    {
        return gallery_;
    }

    [[nodiscard]] auto kanban_lane_metrics() const -> KanbanLaneMetrics
    {
        return kanban_;
    }

    [[nodiscard]] auto table_overlay_metrics() const -> TableOverlayMetrics
    {
        return overlay_;
    }

private:
    DenseRowMetrics rows_;
    TableSurfaceMetrics table_;
    GalleryCardMetrics gallery_;
    KanbanLaneMetrics kanban_;
    TableOverlayMetrics overlay_;
};

} // namespace markamp::core
