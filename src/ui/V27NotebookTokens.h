/// @file V27NotebookTokens.h
/// @brief V27 Phase 09 — Notebook shell, cell output, mixed content.
#pragma once
#include <cstdint>
namespace markamp::ui
{
struct V27NotebookCellTokens {
    static constexpr int kCellPaddingH = 16;
    static constexpr int kCellPaddingV = 8;
    static constexpr int kCellGap = 4;
    static constexpr int kToolbarHeight = 28;
    static constexpr int kToolbarIconSize = 14;
    static constexpr int kStatusIconSize = 12;
    static constexpr int kExecutionIndicatorWidth = 4;
    static constexpr int kCellRadius = 4;
    static constexpr int kDragHandleWidth = 16;
    static constexpr int kGutterWidth = 48;
};
struct V27NotebookOutputTokens {
    static constexpr int kOutputPaddingH = 16;
    static constexpr int kOutputPaddingV = 8;
    static constexpr int kOutputMaxHeight = 400;
    static constexpr int kOutputBorderWidth = 1;
    static constexpr int kOutputRadius = 4;
    static constexpr int kScrollInset = 8;
    static constexpr int kErrorBannerHeight = 32;
    static constexpr int kImageMaxWidth = 600;
};
struct V27NotebookEdgeTokens {
    static constexpr int kEmptyCellIconSize = 32;
    static constexpr int kEmptyCellGap = 12;
    static constexpr int kLoadingSpinnerSize = 24;
    static constexpr int kErrorIconSize = 16;
    static constexpr int kAddCellButtonHeight = 28;
};
} // namespace markamp::ui
