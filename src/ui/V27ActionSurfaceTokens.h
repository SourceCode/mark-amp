/// @file V27ActionSurfaceTokens.h
/// @brief V27 Phase 05 — Menus, command surfaces, toolbars, action clusters.
///
/// Premium layout tokens for redesigned toolbars, menus, context menus,
/// command palette, and overflow surfaces.
#pragma once

#include <cstdint>

namespace markamp::ui
{

/// V27 toolbar tokens (supersedes ToolbarTokens for V27 surfaces).
struct V27ToolbarTokens
{
    static constexpr int kHeight = 38;          ///< Toolbar height
    static constexpr int kButtonSize = 28;      ///< Button touch target
    static constexpr int kButtonIconSize = 16;  ///< Icon within button
    static constexpr int kButtonGap = 2;        ///< Gap between buttons
    static constexpr int kGroupGap = 8;         ///< Gap between button groups
    static constexpr int kSeparatorWidth = 1;   ///< Separator thickness
    static constexpr int kPaddingH = 8;         ///< Horizontal padding
    static constexpr int kPaddingV = 4;         ///< Vertical padding
    static constexpr int kOverflowWidth = 28;   ///< Overflow button width
};

/// V27 menu tokens (supersedes ContextMenuTokens for V27 surfaces).
struct V27MenuTokens
{
    static constexpr int kItemHeight = 28;       ///< Menu item row height
    static constexpr int kItemPaddingH = 12;     ///< Horizontal padding
    static constexpr int kIconColumnWidth = 24;  ///< Fixed icon column
    static constexpr int kShortcutGap = 24;      ///< Gap before shortcut text
    static constexpr int kSubmenuArrowWidth = 16;///< Submenu arrow width
    static constexpr int kSeparatorHeight = 9;   ///< Separator thickness + padding
    static constexpr int kMinWidth = 200;        ///< Minimum menu width
    static constexpr int kMaxWidth = 380;        ///< Maximum menu width
    static constexpr int kRadius = 6;            ///< Corner radius
    static constexpr int kShadowBlur = 8;        ///< Drop shadow blur
    static constexpr int kShadowOffsetY = 4;     ///< Drop shadow Y offset
};

/// V27 command palette tokens.
struct V27CommandPaletteTokens
{
    static constexpr int kWidth = 520;             ///< Palette width
    static constexpr int kMaxHeight = 400;         ///< Max palette height
    static constexpr int kInputHeight = 36;        ///< Search input height
    static constexpr int kInputPaddingH = 12;      ///< Input horizontal padding
    static constexpr int kInputIconSize = 16;      ///< Search icon size
    static constexpr int kResultRowHeight = 28;    ///< Result row height
    static constexpr int kResultIconSize = 16;     ///< Result icon size
    static constexpr int kResultIconGap = 8;       ///< Gap between icon and label
    static constexpr int kCategoryHeaderHeight = 24;///< Category header height
    static constexpr int kRadius = 8;              ///< Corner radius
    static constexpr int kShadowBlur = 16;         ///< Elevated shadow
};

} // namespace markamp::ui
