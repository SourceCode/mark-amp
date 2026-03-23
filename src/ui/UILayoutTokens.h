/// @file UILayoutTokens.h
/// @brief V16 Phase 21 — Centralized UI spacing, icon sizing, and visual hierarchy tokens.
///
/// All layout constants for icon/text pairs, panel density, toolbar rhythm,
/// context menu alignment, and interaction states. Theme-agnostic geometry;
/// colors and fonts come from ThemeEngine.

#pragma once

#include <cstdint>

namespace markamp::ui
{

// ============================================================================
// 1. Spacing Scale — 4px base unit progression
// ============================================================================

struct SpacingScale
{
    static constexpr int kUnit = 4;
    static constexpr int kXxs = 2;   // 2px — sub-pixel nudge
    static constexpr int kXs = 4;    // 4px — tight
    static constexpr int kSm = 8;    // 8px — compact
    static constexpr int kMd = 12;   // 12px — standard
    static constexpr int kLg = 16;   // 16px — comfortable
    static constexpr int kXl = 24;   // 24px — spacious
    static constexpr int kXxl = 32;  // 32px — section gap
    static constexpr int kXxxl = 48; // 48px — page section
};

// ============================================================================
// 2. Icon Sizes — consistent across all surfaces
// ============================================================================

struct IconSizeTokens
{
    static constexpr int kMicro = 12;  // inline badges, status dots
    static constexpr int kSmall = 16;  // file tree, breadcrumb, tab close
    static constexpr int kMedium = 20; // activity bar, toolbar, context menu
    static constexpr int kLarge = 24;  // panel headers, command palette
    static constexpr int kXlarge = 32; // splash, empty state
    static constexpr int kHero = 48;   // onboarding, hero illustrations
};

// ============================================================================
// 3. Icon/Text Pair Spacing — gap between icon and adjacent label
// ============================================================================

struct IconTextPairTokens
{
    static constexpr int kGapTight = 4;    // file tree rows
    static constexpr int kGapStandard = 8; // toolbar buttons, context menus
    static constexpr int kGapLoose = 12;   // panel headers, command palette items
};

// ============================================================================
// 4. Dense Panel Layout — file tree, outline, search results
// ============================================================================

struct DensePanelTokens
{
    static constexpr int kRowHeight = 22;         // standard row height
    static constexpr int kRowHeightCompact = 18;  // compact mode
    static constexpr int kIndentWidth = 16;       // tree indentation per level
    static constexpr int kRowPaddingH = 8;        // horizontal padding per row
    static constexpr int kRowPaddingV = 2;        // vertical padding per row
    static constexpr int kNestIndicatorWidth = 1; // tree guide line width
    static constexpr int kMaxNestDepth = 12;      // max visible nesting levels
};

// ============================================================================
// 5. Toolbar & Button Rhythm — consistent touch targets
// ============================================================================

struct ToolbarTokens
{
    static constexpr int kButtonSize = 28;        // square touch target
    static constexpr int kButtonSizeCompact = 24; // compact variant
    static constexpr int kButtonGap = 2;          // between adjacent buttons
    static constexpr int kGroupGap = 8;           // between button groups
    static constexpr int kSeparatorWidth = 1;     // visual separator thickness
    static constexpr int kSeparatorGap = 6;       // padding around separator
    static constexpr int kToolbarPaddingH = 8;    // toolbar horizontal padding
    static constexpr int kToolbarPaddingV = 4;    // toolbar vertical padding
    static constexpr int kToolbarHeight = 36;     // standard toolbar height
};

// ============================================================================
// 6. Context Menu Alignment
// ============================================================================

struct ContextMenuTokens
{
    static constexpr int kItemHeight = 28;        // menu item row height
    static constexpr int kItemPaddingH = 12;      // horizontal padding
    static constexpr int kIconColumnWidth = 24;   // fixed icon column width
    static constexpr int kShortcutGap = 24;       // gap before shortcut text
    static constexpr int kSubmenuArrowWidth = 16; // submenu indicator width
    static constexpr int kSeparatorHeight =
        9; // separator row height (1px line + 4px padding each side)
    static constexpr int kMenuMinWidth = 180; // minimum menu width
    static constexpr int kMenuMaxWidth = 400; // maximum menu width
    static constexpr int kBorderRadius = 6;   // menu corner radius
};

// ============================================================================
// 7. Interaction Timing — hover, press, focus (milliseconds)
// ============================================================================

struct InteractionTimingTokens
{
    static constexpr int kHoverDelayMs = 0;           // immediate hover response
    static constexpr int kTooltipDelayMs = 600;       // tooltip show delay
    static constexpr int kTooltipFadeMs = 150;        // tooltip fade-in
    static constexpr int kPressScaleMs = 80;          // press-down scale duration
    static constexpr int kReleaseMs = 120;            // release spring-back
    static constexpr int kFocusRingFadeMs = 150;      // focus ring appearance
    static constexpr int kTransitionStandardMs = 200; // standard transition
    static constexpr int kTransitionFastMs = 100;     // fast micro-interaction
    static constexpr int kTransitionSlowMs = 350;     // deliberate motion
    static constexpr int kReducedMotionMs = 0;        // instant for prefers-reduced-motion
};

// ============================================================================
// 8. Status & Notification Layout
// ============================================================================

struct StatusBarTokens
{
    static constexpr int kHeight = 22;                // status bar height
    static constexpr int kItemPaddingH = 6;           // horizontal padding per item
    static constexpr int kItemGap = 2;                // gap between status items
    static constexpr int kIconSize = 14;              // status bar icon size
    static constexpr int kNotificationBadgeSize = 16; // badge diameter
    static constexpr int kMaxVisibleItems = 10;       // max items before overflow
};

// ============================================================================
// 9. Empty / Loading / Error State Layout
// ============================================================================

struct EmptyStateTokens
{
    static constexpr int kIconSize = 48;      // hero icon size
    static constexpr int kHeadingGap = 16;    // gap below icon
    static constexpr int kBodyGap = 8;        // gap below heading
    static constexpr int kActionGap = 24;     // gap before action button
    static constexpr int kMaxWidth = 320;     // max content width
    static constexpr int kSpinnerSize = 24;   // loading spinner diameter
    static constexpr int kShimmerHeight = 16; // skeleton shimmer line height
    static constexpr int kShimmerGap = 8;     // gap between shimmer lines
};

// ============================================================================
// 10. Tab Bar Layout
// ============================================================================

struct TabBarTokens
{
    static constexpr int kTabHeight = 35;         // tab row height
    static constexpr int kTabMinWidth = 80;       // minimum tab width
    static constexpr int kTabMaxWidth = 240;      // maximum tab width
    static constexpr int kTabPaddingH = 12;       // internal horizontal padding
    static constexpr int kCloseButtonSize = 16;   // close icon touch target
    static constexpr int kCloseButtonGap = 4;     // gap before close button
    static constexpr int kDirtyIndicatorSize = 8; // unsaved dot diameter
    static constexpr int kTabIconGap = 6;         // gap between icon and label
};

// ============================================================================
// 11. V26 Phase 01: Dialog Layout Tokens
// ============================================================================

struct DialogTokens
{
    static constexpr int kMinWidth = 360;          // minimum dialog width
    static constexpr int kMaxWidth = 600;          // maximum dialog width
    static constexpr int kPaddingH = 24;           // horizontal content padding
    static constexpr int kPaddingV = 20;           // vertical content padding
    static constexpr int kTitleGap = 12;           // gap below title
    static constexpr int kBodyGap = 16;            // gap below body
    static constexpr int kFooterGap = 20;          // gap above footer actions
    static constexpr int kFooterButtonGap = 8;     // gap between footer buttons
    static constexpr int kBorderRadius = 8;        // dialog corner radius
    static constexpr int kBackdropOpacity = 128;   // backdrop alpha (0-255)
};

// ============================================================================
// 12. V26 Phase 01: Panel Header Tokens
// ============================================================================

struct PanelHeaderTokens
{
    static constexpr int kHeight = 32;             // standard panel header height
    static constexpr int kPaddingH = 12;           // horizontal padding
    static constexpr int kTitleIconGap = 6;        // gap between icon and title
    static constexpr int kActionGap = 4;           // gap between header actions
    static constexpr int kActionSize = 20;         // header action button size
    static constexpr int kDividerHeight = 1;       // bottom divider thickness
};

// ============================================================================
// 13. V26 Phase 01: Settings Row Tokens
// ============================================================================

struct SettingsRowTokens
{
    static constexpr int kRowMinHeight = 36;       // minimum settings row height
    static constexpr int kRowPaddingH = 16;        // horizontal padding
    static constexpr int kRowPaddingV = 8;         // vertical padding
    static constexpr int kLabelWidth = 220;        // label column width (preferred)
    static constexpr int kControlWidth = 200;      // control column width (preferred)
    static constexpr int kHelpTextGap = 2;         // gap between control and help
    static constexpr int kCategoryGap = 24;        // gap between setting categories
    static constexpr int kGroupGap = 16;           // gap between setting groups
    static constexpr int kIconSize = 16;           // category icon size
    static constexpr int kIconGap = 8;             // gap between icon and label
};

// ============================================================================
// 14. V26 Phase 01: Common Radius Aliases
// ============================================================================

struct RadiusTokens
{
    static constexpr int kNone = 0;                // sharp corners
    static constexpr int kSm = 2;                  // subtle rounding
    static constexpr int kMd = 4;                  // standard control
    static constexpr int kLg = 6;                  // cards, previews
    static constexpr int kXl = 8;                  // panels, dialogs
    static constexpr int kPill = 9999;             // fully rounded
};

} // namespace markamp::ui
