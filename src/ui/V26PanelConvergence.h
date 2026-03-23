/// @file V26PanelConvergence.h
/// @brief V26 Phase 05: Panel, Inspector & Dense Information Chrome convergence tokens.
///
/// Defines premium layout/typography/spacing roles that every MarkAmp panel
/// must adopt to achieve visual parity. Covers:
///   - Panel header system (title, subtitle, action row)
///   - Dense row metrics for list/tree panels
///   - Inspector row layout (label/value/action)
///   - Empty-state templates
#pragma once

#include <cstdint>
#include <string_view>

namespace markamp::ui
{

// ═══════════════════════════════════════════════════════════════════════════════
// Phase 05: Panel Chrome System
// ═══════════════════════════════════════════════════════════════════════════════

/// Standard panel header layout — all panels must conform to these metrics.
struct PanelHeaderV26
{
    static constexpr int kHeight           = 32;   ///< Total header height
    static constexpr int kPaddingH         = 12;   ///< Horizontal padding
    static constexpr int kTitleFontPt      = 11;   ///< Title font size
    static constexpr int kSubtitleFontPt   = 10;   ///< Subtitle / count badge font
    static constexpr int kActionSize       = 20;   ///< Action icon hit target
    static constexpr int kActionGap        = 4;    ///< Gap between action icons
    static constexpr int kDividerH         = 1;    ///< Bottom divider height
    static constexpr int kCollapseIconSize = 12;   ///< Collapse chevron size
};

/// Dense row metrics for file trees, search results, problems lists, etc.
struct DenseRowMetrics
{
    static constexpr int kRowHeightDefault  = 24;
    static constexpr int kRowHeightCompact  = 20;
    static constexpr int kRowHeightComfort  = 28;
    static constexpr int kIndentStep        = 16;   ///< Per-level indent for trees
    static constexpr int kIconSize          = 16;
    static constexpr int kIconTextGap       = 6;    ///< Gap between icon and text
    static constexpr int kRowPaddingH       = 12;   ///< Horizontal padding
    static constexpr int kBadgeHeight       = 16;   ///< Inline badge height
    static constexpr int kBadgePaddingH     = 6;    ///< Badge horizontal padding
    static constexpr int kBadgeRadius       = 8;    ///< Pill badge radius
    static constexpr int kChevronSize       = 12;   ///< Expand/collapse chevron
    static constexpr int kDecoratorDotSize  = 8;    ///< Status dot (modified, error)

    /// Resolve row height for a given density profile index (0=comfort, 1=default, 2=compact).
    [[nodiscard]] static constexpr auto row_height_for_density(int density_idx) noexcept -> int
    {
        switch (density_idx)
        {
            case 0: return kRowHeightComfort;
            case 2: return kRowHeightCompact;
            default: return kRowHeightDefault;
        }
    }
};

/// Inspector row layout (e.g. for property inspectors, settings rows, AV cells).
struct InspectorRowMetrics
{
    static constexpr int kRowMinHeight     = 36;
    static constexpr int kLabelWidth       = 140;   ///< Label column width
    static constexpr int kLabelPaddingR    = 12;    ///< Right padding after label
    static constexpr int kValuePaddingH    = 8;     ///< Value field padding
    static constexpr int kActionWidth      = 28;    ///< Inline action button width
    static constexpr int kHelpTextFontPt   = 10;    ///< Help text font size
    static constexpr int kGroupHeaderH     = 28;    ///< Collapsible group header height
    static constexpr int kGroupGap         = 8;     ///< Gap between groups
    static constexpr int kCategoryGap      = 16;    ///< Gap between categories
};

/// Empty state template roles.
enum class EmptyStateRole : uint8_t
{
    kFullPage  = 0,   ///< Full-page welcome / startup
    kPanel     = 1,   ///< Panel-level "No results" / "Nothing open"
    kInline    = 2,   ///< Inline compact "No items match"
};

/// Empty state rendering metrics.
struct EmptyStateMetrics
{
    static constexpr int kFullPageIconSize     = 48;
    static constexpr int kFullPageTitlePt      = 18;
    static constexpr int kFullPageBodyPt       = 13;
    static constexpr int kFullPageActionGap    = 16;

    static constexpr int kPanelIconSize        = 32;
    static constexpr int kPanelTitlePt         = 13;
    static constexpr int kPanelBodyPt          = 11;
    static constexpr int kPanelPaddingV        = 24;

    static constexpr int kInlinePaddingV       = 8;
    static constexpr int kInlineFontPt         = 11;

    /// Number of empty-state role variants.
    [[nodiscard]] static constexpr auto role_count() noexcept -> int { return 3; }
};

/// Panel action bar — shared action row at the top of panels.
struct PanelActionBarMetrics
{
    static constexpr int kHeight           = 28;
    static constexpr int kPaddingH         = 8;
    static constexpr int kButtonSize       = 20;
    static constexpr int kButtonGap        = 4;
    static constexpr int kSearchInputH     = 24;
    static constexpr int kFilterChipH      = 20;
    static constexpr int kFilterChipRadius = 10;
    static constexpr int kFilterChipPadH   = 8;
};

} // namespace markamp::ui
