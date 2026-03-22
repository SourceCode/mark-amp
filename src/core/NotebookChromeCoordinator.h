#pragma once

#include "VisualLanguageTokens.h"

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// V22 Phase 09: Notebook chrome coordination service.
///
/// Provides standardized metrics for notebook shell, cell containers,
/// execution chrome, output blocks, and notebook navigation.
class NotebookChromeCoordinator
{
public:
    /// Cell state for per-state styling.
    enum class CellState : uint8_t
    {
        kIdle,
        kSelected,
        kRunning,
        kSuccess,
        kError,
        kQueued,
    };

    /// Notebook shell metrics.
    struct NotebookShellMetrics
    {
        int toolbar_height{36};
        int cell_gap{8};                  /// Gap between cells
        int content_max_width{900};       /// Max content width for readability
        int margin_h{40};                 /// Horizontal margin
        TypeScaleToken toolbar_font{TypeScaleToken::kBody};
    };

    /// Cell container metrics.
    struct CellContainerMetrics
    {
        int prompt_gutter_width{40};      /// Left gutter with execution count
        int cell_padding_h{16};
        int cell_padding_v{8};
        int add_cell_button_height{24};
        int execution_count_width{32};
        CornerRadiusToken corner{CornerRadiusToken::kMd};
        BorderWeightToken border{BorderWeightToken::kThin};
        BorderWeightToken selected_border{BorderWeightToken::kMedium};
        TypeScaleToken prompt_font{TypeScaleToken::kCaption};
    };

    /// Execution chrome metrics.
    struct ExecutionChromeMetrics
    {
        int run_button_size{20};
        int status_indicator_size{12};
        int progress_bar_height{2};
        int elapsed_time_width{60};
        TypeScaleToken status_font{TypeScaleToken::kCaption};
    };

    /// Output block metrics.
    struct OutputBlockMetrics
    {
        int max_height{400};              /// Max output height before scroll
        int padding_h{16};
        int padding_v{8};
        int collapse_button_size{16};
        int error_padding_left{12};
        CornerRadiusToken corner{CornerRadiusToken::kSm};
        TypeScaleToken output_font{TypeScaleToken::kBody};
        TypeScaleToken error_font{TypeScaleToken::kBody};
    };

    NotebookChromeCoordinator() = default;

    /// Notebook shell metrics.
    [[nodiscard]] auto shell_metrics() const -> NotebookShellMetrics
    {
        return shell_;
    }

    /// Cell container metrics.
    [[nodiscard]] auto cell_container_metrics() const -> CellContainerMetrics
    {
        return cell_;
    }

    /// Execution chrome metrics.
    [[nodiscard]] auto execution_chrome_metrics() const -> ExecutionChromeMetrics
    {
        return execution_;
    }

    /// Output block metrics.
    [[nodiscard]] auto output_block_metrics() const -> OutputBlockMetrics
    {
        return output_;
    }

    /// Get border weight for a given cell state.
    [[nodiscard]] auto cell_border_for_state(CellState state) const -> BorderWeightToken;

    /// Get corner radius for a given cell state.
    [[nodiscard]] auto cell_corner_for_state(CellState state) const -> CornerRadiusToken;

private:
    NotebookShellMetrics shell_;
    CellContainerMetrics cell_;
    ExecutionChromeMetrics execution_;
    OutputBlockMetrics output_;
};

} // namespace markamp::core
