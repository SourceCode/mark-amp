/// @file V27ShellTokens.h
/// @brief V27 Phase 04 — Application shell, workspace chrome, and region tokens.
///
/// Layout tokens for the redesigned application shell: outer region chrome,
/// splitter geometry, activity bar, sidebar, and empty workspace atmosphere.
#pragma once

#include <cstdint>

namespace markamp::ui
{

// ============================================================================
// Shell Region Geometry
// ============================================================================

/// V27 shell region layout tokens.
struct V27ShellRegionTokens
{
    static constexpr int kTitleBarHeight = 30;       ///< Title bar area height
    static constexpr int kActivityBarWidth = 48;     ///< Activity bar width
    static constexpr int kSidebarMinWidth = 180;     ///< Sidebar minimum width
    static constexpr int kSidebarMaxWidth = 480;     ///< Sidebar maximum width
    static constexpr int kSidebarDefaultWidth = 260; ///< Sidebar default width
    static constexpr int kBottomPanelMinHeight = 100;///< Bottom panel minimum height
    static constexpr int kBottomPanelMaxHeight = 600;///< Bottom panel maximum height
    static constexpr int kBottomPanelDefault = 200;  ///< Bottom panel default height
};

/// V27 splitter tokens.
struct V27SplitterTokens
{
    static constexpr int kWidth = 4;           ///< Splitter visual width
    static constexpr int kHitTarget = 8;       ///< Splitter hit target width
    static constexpr int kHandleLength = 32;   ///< Splitter handle indicator length
    static constexpr int kHandleDots = 3;      ///< Number of dots in handle indicator
};

/// V27 activity bar tokens.
struct V27ActivityBarTokens
{
    static constexpr int kIconSize = 20;           ///< Activity bar icon size
    static constexpr int kItemHeight = 48;         ///< Activity bar item height
    static constexpr int kItemPadding = 12;        ///< Padding around each item
    static constexpr int kIndicatorWidth = 2;      ///< Active item indicator width
    static constexpr int kIndicatorHeight = 18;    ///< Active item indicator height
    static constexpr int kBadgeSize = 16;          ///< Badge diameter
    static constexpr int kBadgeOffset = 6;         ///< Badge offset from icon edge
    static constexpr int kSeparatorGap = 8;        ///< Gap around section separator
};

/// V27 workspace atmosphere tokens (empty/sparse states).
struct V27WorkspaceAtmosphereTokens
{
    static constexpr int kHeroIconSize = 48;       ///< Hero icon in empty workspace
    static constexpr int kTitleGap = 16;           ///< Gap below hero icon
    static constexpr int kBodyGap = 8;             ///< Gap below title
    static constexpr int kActionGap = 24;          ///< Gap before action buttons
    static constexpr int kMaxContentWidth = 400;   ///< Max content block width
    static constexpr int kRecentItemHeight = 28;   ///< Height of recent file item
    static constexpr int kRecentItemIconGap = 8;   ///< Gap between icon and label
};

} // namespace markamp::ui
