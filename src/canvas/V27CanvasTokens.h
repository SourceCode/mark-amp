/// @file V27CanvasTokens.h
/// @brief V27 Phase 10 — Canvas shell, tooling, overlay, creative surfaces.
#pragma once
#include <cstdint>
namespace markamp::canvas
{
struct V27CanvasToolPaletteTokens {
    static constexpr int kPaletteWidth = 48;
    static constexpr int kToolButtonSize = 36;
    static constexpr int kToolIconSize = 18;
    static constexpr int kToolGap = 2;
    static constexpr int kGroupSeparatorGap = 8;
    static constexpr int kPaletteRadius = 8;
    static constexpr int kPaletteShadowBlur = 8;
};
struct V27CanvasSelectionTokens {
    static constexpr int kHandleSize = 8;
    static constexpr int kHandleBorderWidth = 2;
    static constexpr int kRotationHandleOffset = 24;
    static constexpr int kRotationHandleSize = 12;
    static constexpr int kSelectionBorderWidth = 1;
    static constexpr int kMultiSelectDash = 4;
    static constexpr int kSnapLineWidth = 1;
    static constexpr int kAlignmentGuideWidth = 1;
    static constexpr int kSpacingIndicatorHeight = 12;
};
struct V27CanvasOverlayTokens {
    static constexpr int kPropertyPanelWidth = 280;
    static constexpr int kPropertyPanelRowHeight = 28;
    static constexpr int kMinimapSize = 120;
    static constexpr int kMinimapRadius = 4;
    static constexpr int kContextMenuMinWidth = 180;
    static constexpr int kZoomControlHeight = 28;
    static constexpr int kBoardNavHeight = 32;
};
} // namespace markamp::canvas
