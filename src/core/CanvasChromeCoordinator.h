#pragma once

#include "VisualLanguageTokens.h"

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// V22 Phase 10: Canvas chrome coordination service.
///
/// Provides standardized metrics for canvas workspace frame, tool palettes,
/// object selection visuals, object chrome, and collaboration overlays.
class CanvasChromeCoordinator
{
public:
    /// Canvas tool state.
    enum class ToolState : uint8_t
    {
        kIdle,
        kActive,
        kHover,
        kDisabled,
    };

    /// Canvas workspace frame metrics.
    struct WorkspaceFrameMetrics
    {
        int toolbar_height{40};
        int tool_palette_width{44};
        int mini_map_size{120};
        int zoom_control_width{100};
        int zoom_button_size{24};
        int zoom_label_width{48};
        CornerRadiusToken toolbar_corner{CornerRadiusToken::kNone};
        CornerRadiusToken zoom_corner{CornerRadiusToken::kMd};
        ElevationToken tool_palette_elevation{ElevationToken::kLow};
    };

    /// Tool palette metrics.
    struct ToolPaletteMetrics
    {
        int tool_button_size{32};
        int tool_button_gap{4};
        int separator_height{1};
        int separator_margin{4};
        int group_gap{8};
        CornerRadiusToken button_corner{CornerRadiusToken::kSm};
        IconMetricToken icon_size{IconMetricToken::kMedium};
    };

    /// Object selection visual metrics.
    struct SelectionVisualMetrics
    {
        int handle_size{8};
        int rotation_handle_offset{24};  /// Distance above object for rotation handle
        int snap_guide_thickness{1};
        int selection_border_width{2};
        float snap_guide_opacity{0.6F};
        float multi_select_fill_opacity{0.05F};
        CornerRadiusToken handle_corner{CornerRadiusToken::kSm};
    };

    /// Canvas object chrome metrics (inspector, inline editors).
    struct ObjectChromeMetrics
    {
        int inspector_width{260};
        int inspector_section_gap{12};
        int property_row_height{28};
        int property_label_width{80};
        int color_swatch_size{20};
        TypeScaleToken section_font{TypeScaleToken::kSubtitle};
        TypeScaleToken property_font{TypeScaleToken::kBody};
        TypeScaleToken value_font{TypeScaleToken::kBody};
        CornerRadiusToken inspector_corner{CornerRadiusToken::kMd};
    };

    /// Collaboration overlay metrics.
    struct CollabOverlayMetrics
    {
        int cursor_label_height{18};
        int cursor_label_max_width{120};
        int cursor_caret_width{2};
        int cursor_caret_height{20};
        int selection_border_width{2};
        float selection_fill_opacity{0.1F};
        CornerRadiusToken label_corner{CornerRadiusToken::kSm};
        TypeScaleToken label_font{TypeScaleToken::kCaption};
    };

    /// Empty board state config.
    struct EmptyBoardConfig
    {
        std::string title;
        std::string subtitle;
        std::string icon_name;
        IconMetricToken icon_size{IconMetricToken::kHero};
        TypeScaleToken title_font{TypeScaleToken::kTitle};
        TypeScaleToken subtitle_font{TypeScaleToken::kBody};
    };

    CanvasChromeCoordinator() = default;

    [[nodiscard]] auto workspace_frame_metrics() const -> WorkspaceFrameMetrics
    {
        return frame_;
    }

    [[nodiscard]] auto tool_palette_metrics() const -> ToolPaletteMetrics
    {
        return palette_;
    }

    [[nodiscard]] auto selection_visual_metrics() const -> SelectionVisualMetrics
    {
        return selection_;
    }

    [[nodiscard]] auto object_chrome_metrics() const -> ObjectChromeMetrics
    {
        return object_;
    }

    [[nodiscard]] auto collab_overlay_metrics() const -> CollabOverlayMetrics
    {
        return collab_;
    }

    [[nodiscard]] auto empty_board_config() const -> EmptyBoardConfig
    {
        return empty_board_;
    }

    /// Get tool button corner radius for a given state.
    [[nodiscard]] auto tool_corner_for_state(ToolState state) const -> CornerRadiusToken;

private:
    WorkspaceFrameMetrics frame_;
    ToolPaletteMetrics palette_;
    SelectionVisualMetrics selection_;
    ObjectChromeMetrics object_;
    CollabOverlayMetrics collab_;
    EmptyBoardConfig empty_board_{
        "Start Creating",
        "Use the tools on the left to add objects to your canvas",
        "canvas-blank",
        IconMetricToken::kHero,
        TypeScaleToken::kTitle,
        TypeScaleToken::kBody};
};

} // namespace markamp::core
