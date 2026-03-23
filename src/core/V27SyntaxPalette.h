/// @file V27SyntaxPalette.h
/// @brief V27 Phase 08 — Syntax highlighting palette and code presentation.
#pragma once
#include <cstdint>
namespace markamp::core
{
/// V27 syntax saturation tiers for long-session comfort.
enum class V27SyntaxSaturation { kMuted, kStandard, kVivid };
[[nodiscard]] constexpr auto v27_syntax_saturation_count() noexcept -> int { return 3; }
/// V27 syntax weight for scope importance.
enum class V27SyntaxWeight { kSubtle, kNormal, kStrong };
/// V27 code context — where code is displayed.
enum class V27CodeContext { kEditor, kNotebook, kPreview, kInline, kDiff };
[[nodiscard]] constexpr auto v27_code_context_count() noexcept -> int { return 5; }
struct V27SyntaxPaletteTokens {
    static constexpr int kEditorFontSize = 13;
    static constexpr int kNotebookFontSize = 12;
    static constexpr int kPreviewFontSize = 12;
    static constexpr int kInlineFontSize = 12;
    static constexpr int kLineNumberOpacityPct = 50;  ///< 50% opacity
    static constexpr int kCommentOpacityPct = 60;     ///< 60% opacity
    static constexpr int kBracketMatchRadius = 2;
    static constexpr int kIndentGuideOpacityPct = 15; ///< 15% opacity
};
} // namespace markamp::core
