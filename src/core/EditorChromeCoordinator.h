#pragma once

#include "VisualLanguageTokens.h"

#include <cstdint>
#include <string>

namespace markamp::core
{

/// V22 Phase 06: Editor chrome coordination service.
///
/// Provides standardized metrics for editor frame, gutter, minimap,
/// caret/selection styling, and inline furniture (brackets, guides, rulers).
class EditorChromeCoordinator
{
public:
    /// Editor frame and margin metrics.
    struct EditorFrameMetrics
    {
        int gutter_width{48};            /// Total gutter width including line numbers
        int line_number_width{36};       /// Line number column width
        int fold_marker_width{12};       /// Fold gutter column width
        int decoration_gutter_width{8};  /// Breakpoint/decoration gutter
        int content_left_margin{4};      /// Left margin between gutter and content
        int content_right_margin{16};    /// Right margin after content
        TypeScaleToken line_number_font{TypeScaleToken::kCaption};
        CornerRadiusToken fold_marker_corner{CornerRadiusToken::kSm};
    };

    /// Caret and selection styling tokens.
    struct CaretSelectionStyle
    {
        int caret_width{2};
        int caret_blink_rate_ms{530};
        int selection_corner_radius{0};  /// 0 = sharp selection rectangles
        float selection_opacity{0.35F};  /// Selection background alpha
        float line_highlight_opacity{0.07F}; /// Current line highlight alpha
        bool show_indent_guides{true};
        bool show_bracket_pairs{true};
    };

    /// Minimap presentation metrics.
    struct MinimapMetrics
    {
        int width{60};
        int slider_min_height{20};
        float char_width{1.4F};          /// Minimap character width scale
        float char_height{2.0F};         /// Minimap character height scale
        float slider_opacity{0.2F};      /// Slider background opacity
        float slider_hover_opacity{0.4F};
        CornerRadiusToken slider_corner{CornerRadiusToken::kSm};
        BorderWeightToken slider_border{BorderWeightToken::kThin};
    };

    /// Overview ruler metrics.
    struct OverviewRulerMetrics
    {
        int width{14};
        int marker_height{4};
        int marker_min_gap{1};
        CornerRadiusToken marker_corner{CornerRadiusToken::kSm};
    };

    /// Inline furniture metrics (brackets, indent guides, column rulers).
    struct InlineFurnitureMetrics
    {
        int indent_guide_width{1};
        int bracket_pair_width{1};
        int ruler_width{1};
        float indent_guide_opacity{0.15F};
        float bracket_highlight_opacity{0.25F};
        int bracket_padding{2};
        CornerRadiusToken bracket_corner{CornerRadiusToken::kSm};
    };

    EditorChromeCoordinator() = default;

    [[nodiscard]] auto editor_frame_metrics() const -> EditorFrameMetrics
    {
        return frame_;
    }

    [[nodiscard]] auto caret_selection_style() const -> CaretSelectionStyle
    {
        return caret_;
    }

    [[nodiscard]] auto minimap_metrics() const -> MinimapMetrics
    {
        return minimap_;
    }

    [[nodiscard]] auto overview_ruler_metrics() const -> OverviewRulerMetrics
    {
        return overview_;
    }

    [[nodiscard]] auto inline_furniture_metrics() const -> InlineFurnitureMetrics
    {
        return furniture_;
    }

private:
    EditorFrameMetrics frame_;
    CaretSelectionStyle caret_;
    MinimapMetrics minimap_;
    OverviewRulerMetrics overview_;
    InlineFurnitureMetrics furniture_;
};

} // namespace markamp::core
