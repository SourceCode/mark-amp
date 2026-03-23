/// @file V26DataFeedbackConvergence.h
/// @brief V26 Phases 12–13: Trees, lists, tables, AV data presentation, and
///        feedback / status / progress / inline validation convergence.
///
/// Covers premium metrics for structured data surfaces (tree rows, table cells,
/// AV panels) and all feedback/status UI (severity badges, progress bars,
/// banners, inline alerts).
#pragma once

#include <cstdint>

namespace markamp::ui
{

// ═══════════════════════════════════════════════════════════════════════════════
// Phase 12: Trees, Lists, Tables, AV & Structured Data Presentation
// ═══════════════════════════════════════════════════════════════════════════════

/// Tree/list row convergence (extends DenseRowMetrics with structured-data additions).
struct TreeListV26Metrics
{
    // ── Row dimensions ───────────────────────────────────────────────
    static constexpr int kRowHeight            = 24;
    static constexpr int kIndentStep           = 16;
    static constexpr int kIconSize             = 16;
    static constexpr int kIconTextGap          = 6;

    // ── Hierarchy indicators ─────────────────────────────────────────
    static constexpr int kChevronSize          = 12;
    static constexpr int kGuideLineW           = 1;     ///< Tree indent guide width
    static constexpr float kGuideLineAlpha     = 0.10F;
    static constexpr float kActiveGuideAlpha   = 0.25F;

    // ── Inline actions ───────────────────────────────────────────────
    static constexpr int kInlineActionSize     = 16;
    static constexpr int kInlineActionGap      = 2;
    static constexpr int kInlineActionPad      = 4;
    static constexpr float kInlineActionAlpha  = 0.0F;  ///< Hidden until hover
    static constexpr float kInlineActionHover  = 1.0F;  ///< Fully visible on row hover

    // ── Status decorators ────────────────────────────────────────────
    static constexpr int kStatusDotSize        = 8;
    static constexpr int kBadgeHeight          = 16;
    static constexpr int kBadgePadH            = 6;
    static constexpr int kBadgeRadius          = 8;
};

/// Table cell metrics (for AVTablePanel and structured data tables).
struct TableCellV26Metrics
{
    // ── Cell sizing ──────────────────────────────────────────────────
    static constexpr int kCellMinHeight        = 32;
    static constexpr int kCellPaddingH         = 8;
    static constexpr int kCellPaddingV         = 4;
    static constexpr int kHeaderHeight         = 32;
    static constexpr int kHeaderPaddingH       = 8;

    // ── Column constraints ───────────────────────────────────────────
    static constexpr int kColumnMinW           = 60;
    static constexpr int kColumnDefaultW       = 140;
    static constexpr int kColumnMaxW           = 500;
    static constexpr int kResizeHandleW        = 4;

    // ── Sort/filter chrome ───────────────────────────────────────────
    static constexpr int kSortArrowSize        = 10;
    static constexpr int kFilterIconSize       = 14;
    static constexpr int kHeaderActionGap      = 4;

    // ── Selection ────────────────────────────────────────────────────
    static constexpr float kSelectedRowAlpha   = 0.12F;
    static constexpr float kHoverRowAlpha      = 0.06F;
    static constexpr int kFocusBorderW         = 2;
};


// ═══════════════════════════════════════════════════════════════════════════════
// Phase 13: Feedback, Status, Progress & Inline Validation UI
// ═══════════════════════════════════════════════════════════════════════════════

/// Feedback severity badge metrics.
struct SeverityBadgeMetrics
{
    static constexpr int kHeight               = 20;
    static constexpr int kPaddingH             = 6;
    static constexpr int kRadius               = 10;
    static constexpr int kIconSize             = 12;
    static constexpr int kIconTextGap          = 4;
    static constexpr int kFontPt               = 10;
};

/// Progress indicator metrics.
struct ProgressV26Metrics
{
    // ── Linear progress bar ──────────────────────────────────────────
    static constexpr int kBarHeight            = 4;
    static constexpr int kBarRadius            = 2;
    static constexpr int kBarMinW              = 60;

    // ── Circular spinner ─────────────────────────────────────────────
    static constexpr int kSpinnerSizeSmall     = 16;
    static constexpr int kSpinnerSizeMedium    = 24;
    static constexpr int kSpinnerSizeLarge     = 32;
    static constexpr int kSpinnerStrokeW       = 2;

    // ── Status bar progress ──────────────────────────────────────────
    static constexpr int kStatusBarProgressH   = 2;   ///< Thin bar at top of status bar
};

/// Inline validation & alert metrics.
struct InlineAlertMetrics
{
    // ── Inline field validation ──────────────────────────────────────
    static constexpr int kValidationIconSize   = 14;
    static constexpr int kValidationFontPt     = 11;
    static constexpr int kValidationGap        = 4;
    static constexpr int kValidationPadV       = 4;

    // ── Banner alerts ────────────────────────────────────────────────
    static constexpr int kBannerHeight         = 36;
    static constexpr int kBannerPaddingH       = 12;
    static constexpr int kBannerIconSize       = 16;
    static constexpr int kBannerRadius         = 4;
    static constexpr int kBannerActionGap      = 8;
};

} // namespace markamp::ui
