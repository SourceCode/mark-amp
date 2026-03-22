#pragma once

#include "VisualLanguageTokens.h"

#include <cstdint>
#include <string>

namespace markamp::core
{

/// V22 Phase 08: Editor overlay coordination service.
///
/// Provides standardized metrics for find/replace, search results,
/// diagnostics, peek editors, and editor-adjacent overlays.
class EditorOverlayCoordinator
{
public:
    /// Find/replace bar metrics.
    struct FindReplaceMetrics
    {
        int bar_height{34};
        int input_width{220};
        int button_size{24};
        int button_gap{2};
        int padding_h{8};
        int match_count_width{60};
        CornerRadiusToken input_corner{CornerRadiusToken::kSm};
        CornerRadiusToken bar_corner{CornerRadiusToken::kMd};
        ElevationToken elevation{ElevationToken::kMedium};
        TypeScaleToken input_font{TypeScaleToken::kBody};
        TypeScaleToken count_font{TypeScaleToken::kCaption};
    };

    /// Search result presentation metrics.
    struct SearchResultMetrics
    {
        int file_row_height{24};
        int match_row_height{22};
        int match_indent{20};          /// Indent for match lines under file header
        int context_lines{2};           /// Lines of context around matches
        int preview_highlight_padding{2};
        TypeScaleToken file_font{TypeScaleToken::kBody};
        TypeScaleToken match_font{TypeScaleToken::kBody};
        TypeScaleToken line_number_font{TypeScaleToken::kCaption};
    };

    /// Diagnostics visual metrics (squiggles, inline messages).
    struct DiagnosticsMetrics
    {
        int squiggle_thickness{2};
        int inline_message_padding{4};
        int severity_icon_size{14};
        float error_squiggle_opacity{1.0F};
        float warning_squiggle_opacity{0.85F};
        float info_squiggle_opacity{0.7F};
        float hint_squiggle_opacity{0.5F};
        TypeScaleToken inline_message_font{TypeScaleToken::kCaption};
    };

    /// Peek view metrics.
    struct PeekViewMetrics
    {
        int header_height{26};
        int min_height{120};
        int max_height{300};
        int result_list_width{200};
        CornerRadiusToken corner{CornerRadiusToken::kMd};
        BorderWeightToken border{BorderWeightToken::kMedium};
        ElevationToken elevation{ElevationToken::kMedium};
        TypeScaleToken title_font{TypeScaleToken::kBody};
    };

    /// Autocomplete / hover / parameter hint overlay metrics.
    struct EditorHoverMetrics
    {
        int max_width{480};
        int max_height{300};
        int padding_h{8};
        int padding_v{4};
        int item_height{22};
        int icon_size{16};
        int icon_gap{6};
        CornerRadiusToken corner{CornerRadiusToken::kMd};
        ElevationToken elevation{ElevationToken::kHigh};
        TypeScaleToken body_font{TypeScaleToken::kBody};
        TypeScaleToken detail_font{TypeScaleToken::kCaption};
    };

    EditorOverlayCoordinator() = default;

    [[nodiscard]] auto find_replace_metrics() const -> FindReplaceMetrics
    {
        return find_replace_;
    }

    [[nodiscard]] auto search_result_metrics() const -> SearchResultMetrics
    {
        return search_;
    }

    [[nodiscard]] auto diagnostics_metrics() const -> DiagnosticsMetrics
    {
        return diagnostics_;
    }

    [[nodiscard]] auto peek_view_metrics() const -> PeekViewMetrics
    {
        return peek_;
    }

    [[nodiscard]] auto editor_hover_metrics() const -> EditorHoverMetrics
    {
        return hover_;
    }

private:
    FindReplaceMetrics find_replace_;
    SearchResultMetrics search_;
    DiagnosticsMetrics diagnostics_;
    PeekViewMetrics peek_;
    EditorHoverMetrics hover_;
};

} // namespace markamp::core
