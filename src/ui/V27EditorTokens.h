/// @file V27EditorTokens.h
/// @brief V27 Phase 07 — Editor chrome, gutter, overlay, readability.
#pragma once
#include <cstdint>
namespace markamp::ui
{
struct V27EditorChromeTokens {
    static constexpr int kGutterWidth = 64;
    static constexpr int kGutterPaddingH = 8;
    static constexpr int kLineNumberWidth = 40;
    static constexpr int kFoldingMarkerSize = 14;
    static constexpr int kGitGutterWidth = 3;
    static constexpr int kMinimapWidth = 80;
    static constexpr int kScrollbarWidth = 10;
    static constexpr int kScrollbarMinThumb = 24;
    static constexpr int kRulerWidth = 1;
    static constexpr int kContentPaddingH = 16;
    static constexpr int kContentPaddingV = 8;
};
struct V27EditorOverlayTokens {
    static constexpr int kSuggestionWidth = 400;
    static constexpr int kSuggestionRowHeight = 22;
    static constexpr int kSuggestionMaxRows = 12;
    static constexpr int kSuggestionIconSize = 16;
    static constexpr int kSuggestionRadius = 4;
    static constexpr int kHoverMaxWidth = 480;
    static constexpr int kHoverPadding = 12;
    static constexpr int kHoverRadius = 4;
};
struct V27EditorReadabilityTokens {
    static constexpr int kLineHeightMultiplier150 = 150; ///< 1.50x base (percentage)
    static constexpr int kLineHeightMultiplier160 = 160; ///< 1.60x base
    static constexpr int kLetterSpacing = 0;             ///< Default, in 0.01em units
    static constexpr int kMaxLineWidth = 80;             ///< Chars before soft-wrap
    static constexpr int kSelectionRadius = 2;           ///< Selection corner radius
    static constexpr int kCaretWidth = 2;                ///< Caret thickness
    static constexpr int kSearchHighlightRadius = 2;     ///< Search match rounding
};
} // namespace markamp::ui
