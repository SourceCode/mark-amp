#pragma once

#include "VisualLanguageTokens.h"

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// V22 Phase 12: Utility panel coordination service.
///
/// Provides standardized metrics for terminal, output, problems, build,
/// debug, source control, and bottom-panel tab strip.
class UtilityPanelCoordinator
{
public:
    /// Severity level for problems/diagnostics.
    enum class SeverityLevel : uint8_t
    {
        kError,
        kWarning,
        kInfo,
        kHint,
    };

    /// Terminal panel metrics.
    struct TerminalMetrics
    {
        int tab_height{28};
        int shell_padding_h{8};
        int shell_padding_v{4};
        int cursor_width{1};
        int scrollbar_width{10};
        TypeScaleToken terminal_font{TypeScaleToken::kBody};
        TypeScaleToken tab_font{TypeScaleToken::kCaption};
    };

    /// Output / log panel metrics.
    struct OutputPanelMetrics
    {
        int row_height{20};
        int timestamp_width{80};
        int channel_label_width{100};
        int padding_h{8};
        TypeScaleToken log_font{TypeScaleToken::kBody};
        TypeScaleToken timestamp_font{TypeScaleToken::kCaption};
    };

    /// Problems panel metrics.
    struct ProblemsPanelMetrics
    {
        int row_height{24};
        int severity_icon_size{14};
        int file_path_width{200};
        int line_number_width{40};
        int code_width{60};
        TypeScaleToken message_font{TypeScaleToken::kBody};
        TypeScaleToken path_font{TypeScaleToken::kCaption};
    };

    /// Bottom panel tab strip metrics.
    struct BottomTabStripMetrics
    {
        int tab_height{28};
        int tab_min_width{60};
        int tab_padding_h{12};
        int badge_size{16};
        int badge_gap{4};
        CornerRadiusToken badge_corner{CornerRadiusToken::kPill};
        TypeScaleToken tab_font{TypeScaleToken::kCaption};
    };

    UtilityPanelCoordinator() = default;

    [[nodiscard]] auto terminal_metrics() const -> TerminalMetrics
    {
        return terminal_;
    }

    [[nodiscard]] auto output_panel_metrics() const -> OutputPanelMetrics
    {
        return output_;
    }

    [[nodiscard]] auto problems_panel_metrics() const -> ProblemsPanelMetrics
    {
        return problems_;
    }

    [[nodiscard]] auto bottom_tab_strip_metrics() const -> BottomTabStripMetrics
    {
        return tab_strip_;
    }

    /// Get severity icon size for a severity level.
    [[nodiscard]] auto severity_icon_size(SeverityLevel level) const -> int;

private:
    TerminalMetrics terminal_;
    OutputPanelMetrics output_;
    ProblemsPanelMetrics problems_;
    BottomTabStripMetrics tab_strip_;
};

} // namespace markamp::core
