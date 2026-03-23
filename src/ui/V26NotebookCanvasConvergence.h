/// @file V26NotebookCanvasConvergence.h
/// @brief V26 Phases 08–09: Notebook shell, cell chrome, canvas workspace convergence tokens.
///
/// Extends the existing NotebookChromeCoordinator with V26-specific premium
/// presentation metrics and defines canvas workspace premium layout rules.
#pragma once

#include <cstdint>

namespace markamp::ui
{

// ═══════════════════════════════════════════════════════════════════════════════
// Phase 08: Notebook Shell, Cell Chrome, Output & Mixed Content
// ═══════════════════════════════════════════════════════════════════════════════

/// V26 notebook presentation convergence metrics.
struct NotebookV26Metrics
{
    // ── Shell ────────────────────────────────────────────────────────
    static constexpr int kToolbarHeight       = 36;
    static constexpr int kCellGap             = 8;
    static constexpr int kContentMaxWidth     = 900;
    static constexpr int kMarginH             = 40;

    // ── Cell container ───────────────────────────────────────────────
    static constexpr int kPromptGutterWidth   = 40;
    static constexpr int kCellPaddingH        = 16;
    static constexpr int kCellPaddingV        = 8;
    static constexpr int kAddCellButtonH      = 24;
    static constexpr int kExecCountWidth      = 32;

    // ── Execution chrome ─────────────────────────────────────────────
    static constexpr int kRunButtonSize       = 20;
    static constexpr int kStatusIndicatorSize = 12;
    static constexpr int kProgressBarH        = 2;
    static constexpr int kElapsedTimeWidth    = 60;

    // ── Output blocks ────────────────────────────────────────────────
    static constexpr int kOutputMaxHeight     = 400;
    static constexpr int kOutputPaddingH      = 16;
    static constexpr int kOutputPaddingV      = 8;
    static constexpr int kCollapseButtonSize  = 16;
    static constexpr int kErrorPaddingLeft    = 12;

    // ── Cell state borders ───────────────────────────────────────────
    static constexpr int kIdleBorderW         = 1;
    static constexpr int kSelectedBorderW     = 2;
    static constexpr int kRunningBorderW      = 2;
    static constexpr int kErrorBorderW        = 2;

    // ── V26-specific metadata chrome ─────────────────────────────────
    static constexpr int kCellExecutionBadgeH = 16;
    static constexpr int kCellMenuIconSize    = 14;
    static constexpr int kCellDividerH        = 1;
    static constexpr float kIdleCellOpacity   = 0.8F;
    static constexpr float kSelectedCellOpacity = 1.0F;
};


// ═══════════════════════════════════════════════════════════════════════════════
// Phase 09: Canvas Shell, Tools, Overlays & Interaction Visuals
// ═══════════════════════════════════════════════════════════════════════════════

/// Canvas workspace premium layout tokens.
struct CanvasV26Metrics
{
    // ── Shell dimensions ─────────────────────────────────────────────
    static constexpr int kToolRailWidth       = 40;
    static constexpr int kInspectorWidth      = 240;
    static constexpr int kMinimapHeight       = 120;
    static constexpr int kContextBarHeight    = 36;

    // ── Tool palette ─────────────────────────────────────────────────
    static constexpr int kToolButtonSize      = 32;
    static constexpr int kToolButtonGap       = 4;
    static constexpr int kToolIconSize        = 20;
    static constexpr int kToolGroupGap        = 8;
    static constexpr int kToolTooltipDelayMs  = 600;

    // ── Selection handles ────────────────────────────────────────────
    static constexpr int kHandleSize          = 8;
    static constexpr int kHandleBorderW       = 1;
    static constexpr int kRotationHandleGap   = 16;
    static constexpr float kHandleFillAlpha   = 0.9F;

    // ── Grid & guides ────────────────────────────────────────────────
    static constexpr int kGridMinSize         = 8;
    static constexpr int kGridDefaultSize     = 20;
    static constexpr float kGridLineAlpha     = 0.08F;
    static constexpr float kGridDotAlpha      = 0.12F;
    static constexpr float kGuideLineAlpha    = 0.40F;
    static constexpr int kSnapDistance        = 8;

    // ── Zoom controls ────────────────────────────────────────────────
    static constexpr int kZoomBarWidth        = 140;
    static constexpr int kZoomStepPercent     = 10;
    static constexpr int kZoomMin             = 10;
    static constexpr int kZoomMax             = 400;
    static constexpr int kZoomDefault         = 100;

    // ── Inline editing ───────────────────────────────────────────────
    static constexpr int kInlineEditPaddingH  = 8;
    static constexpr int kInlineEditPaddingV  = 4;
    static constexpr int kInlineEditBorderW   = 2;
    static constexpr int kInlineEditMinW      = 100;

    /// True if a zoom level is within the valid range.
    [[nodiscard]] static constexpr auto is_zoom_valid(int zoom) noexcept -> bool
    {
        return zoom >= kZoomMin && zoom <= kZoomMax;
    }

    /// Clamp zoom to valid range.
    [[nodiscard]] static constexpr auto clamp_zoom(int zoom) noexcept -> int
    {
        return zoom < kZoomMin ? kZoomMin
             : zoom > kZoomMax ? kZoomMax
             : zoom;
    }
};

} // namespace markamp::ui
