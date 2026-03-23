/// @file V26EditorConvergence.h
/// @brief V26 Phases 06–07: Editor chrome, gutter, selection, caret, minimap, and syntax convergence.
///
/// Centralizes premium metrics for the editor surface: gutter hierarchy,
/// caret/selection/search-highlight intensities, minimap rendering, and
/// syntax token readability tiers.
#pragma once

#include <cstdint>

namespace markamp::ui
{

// ═══════════════════════════════════════════════════════════════════════════════
// Phase 06: Editor Chrome, Gutter, Selection, Caret & Minimap
// ═══════════════════════════════════════════════════════════════════════════════

/// Editor gutter convergence metrics.
struct GutterConvergenceMetrics
{
    // ── Gutter column widths (pixels) ────────────────────────────────
    static constexpr int kLineNumberMinDigits   = 3;
    static constexpr int kLineNumberCharWidth   = 8;    ///< Per-char width estimate
    static constexpr int kFoldMarginWidth       = 14;
    static constexpr int kBreakpointMarginWidth = 16;
    static constexpr int kGitChangeMarginWidth  = 4;
    static constexpr int kBookmarkMarginWidth   = 14;
    static constexpr int kGutterPaddingR        = 8;    ///< Right padding before content

    // ── Gutter visual hierarchy (opacity) ────────────────────────────
    static constexpr float kLineNumberOpacity      = 0.5F;
    static constexpr float kLineNumberActiveOpacity = 1.0F;
    static constexpr float kFoldMarkerOpacity      = 0.4F;
    static constexpr float kFoldMarkerHoverOpacity = 0.8F;

    /// Total minimum gutter width (line number + fold + breakpoint + git change + padding).
    [[nodiscard]] static constexpr auto min_gutter_width() noexcept -> int
    {
        return (kLineNumberMinDigits * kLineNumberCharWidth) + kFoldMarginWidth
             + kBreakpointMarginWidth + kGitChangeMarginWidth + kGutterPaddingR;
    }
};

/// Editor caret, selection, and highlight intensity tiers.
struct EditorHighlightMetrics
{
    // ── Caret ────────────────────────────────────────────────────────
    static constexpr int   kCaretWidth       = 2;
    static constexpr int   kCaretBlinkMs     = 400;
    static constexpr float kCaretLineAlpha   = 0.06F;   ///< Active line highlight

    // ── Selection ────────────────────────────────────────────────────
    static constexpr float kSelectionAlpha      = 0.20F;
    static constexpr float kSelectionMatchAlpha = 0.10F; ///< Other occurrence highlights

    // ── Search highlights ────────────────────────────────────────────
    static constexpr float kSearchMatchAlpha       = 0.35F;
    static constexpr float kSearchCurrentAlpha     = 0.50F;
    static constexpr int   kSearchHighlightBorderW = 1;

    // ── Bracket match ────────────────────────────────────────────────
    static constexpr float kBracketMatchAlpha      = 0.15F;
    static constexpr int   kBracketMatchBorderW    = 1;

    // ── Diagnostics ──────────────────────────────────────────────────
    static constexpr int   kErrorSquiggleThickness  = 2;
    static constexpr int   kWarnSquiggleThickness   = 1;
    static constexpr float kDiagnosticBgAlpha       = 0.08F;

    /// Validate that intensity layers form a readable hierarchy.
    [[nodiscard]] static constexpr auto is_hierarchy_valid() noexcept -> bool
    {
        return kCaretLineAlpha < kSelectionMatchAlpha
            && kSelectionMatchAlpha < kSelectionAlpha
            && kSelectionAlpha < kSearchMatchAlpha
            && kSearchMatchAlpha < kSearchCurrentAlpha;
    }
};

/// Minimap rendering metrics.
struct MinimapConvergenceMetrics
{
    static constexpr int kMinimapWidth         = 80;
    static constexpr int kMinimapCharWidth     = 1;    ///< Simplified char width
    static constexpr int kMinimapLineHeight    = 2;    ///< Simplified line height
    static constexpr float kViewportAlpha      = 0.15F;
    static constexpr float kSelectionAlpha     = 0.30F;
    static constexpr float kSearchMatchAlpha   = 0.50F;
    static constexpr int kSliderMinHeight      = 20;   ///< Viewport slider min height
};


// ═══════════════════════════════════════════════════════════════════════════════
// Phase 07: Syntax Highlighting Presentation & Code Readability
// ═══════════════════════════════════════════════════════════════════════════════

/// Syntax token readability tiers — relative opacity/weight for visual hierarchy.
enum class SyntaxReadabilityTier : uint8_t
{
    kPrimary   = 0,   ///< Keywords, headings — full brightness
    kSecondary = 1,   ///< Strings, types — slightly muted
    kTertiary  = 2,   ///< Comments, punctuation — noticeably muted
    kSubdued   = 3,   ///< Whitespace markers, guides — very muted
};

/// Syntax presentation metrics.
struct SyntaxPresentationMetrics
{
    // ── Tier opacity values ──────────────────────────────────────────
    static constexpr float kPrimaryOpacity   = 1.0F;
    static constexpr float kSecondaryOpacity = 0.85F;
    static constexpr float kTertiaryOpacity  = 0.55F;
    static constexpr float kSubduedOpacity   = 0.30F;

    // ── Code block presentation ──────────────────────────────────────
    static constexpr int kCodeBlockPaddingH   = 12;
    static constexpr int kCodeBlockPaddingV   = 8;
    static constexpr int kCodeBlockRadius     = 4;
    static constexpr int kCodeBlockBorderW    = 1;

    // ── Inline code presentation ─────────────────────────────────────
    static constexpr int kInlineCodePaddingH  = 4;
    static constexpr int kInlineCodePaddingV  = 1;
    static constexpr int kInlineCodeRadius    = 3;

    /// Number of readability tiers.
    [[nodiscard]] static constexpr auto tier_count() noexcept -> int { return 4; }

    /// Get opacity for a readability tier.
    [[nodiscard]] static constexpr auto opacity_for_tier(SyntaxReadabilityTier tier) noexcept -> float
    {
        switch (tier)
        {
            case SyntaxReadabilityTier::kPrimary:   return kPrimaryOpacity;
            case SyntaxReadabilityTier::kSecondary: return kSecondaryOpacity;
            case SyntaxReadabilityTier::kTertiary:  return kTertiaryOpacity;
            case SyntaxReadabilityTier::kSubdued:   return kSubduedOpacity;
        }
        return kPrimaryOpacity;
    }
};

} // namespace markamp::ui
