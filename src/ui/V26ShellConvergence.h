/// @file V26ShellConvergence.h
/// @brief V26 Phases 02–04: Shell frame layout, action surface, and navigation convergence tokens.
///
/// Centralizes the premium UI constraints for the five shell-critical surfaces:
///   Shell frame (LayoutManager), Toolbar, TabBar, BreadcrumbBar, StatusBarPanel.
///
/// These tokens are **constexpr compile-time values** — no runtime overhead.
#pragma once

#include <cstdint>

namespace markamp::ui
{

// ═══════════════════════════════════════════════════════════════════════════════
// Phase 02: Shell Frame Layout Rhythm & Surface Layering
// ═══════════════════════════════════════════════════════════════════════════════

/// Shell proportion policy — defines min/ideal/max for every shell region.
struct ShellLayoutPolicy
{
    // ── Sidebar proportions ──────────────────────────────────────────────
    static constexpr int kSidebarMinWidth      = 180;
    static constexpr int kSidebarDefaultWidth  = 256;
    static constexpr int kSidebarMaxWidth      = 400;
    static constexpr int kSidebarCollapseWidth = 48;   ///< Collapsed to icon-rail

    // ── Activity bar ─────────────────────────────────────────────────────
    static constexpr int kActivityBarWidth     = 48;
    static constexpr int kActivityBarIconSize  = 24;
    static constexpr int kActivityBarSlotH     = 48;
    static constexpr int kActivityBadgeSize    = 16;

    // ── Bottom panel ─────────────────────────────────────────────────────
    static constexpr int kBottomPanelMinH      = 100;
    static constexpr int kBottomPanelDefaultH  = 200;
    static constexpr int kBottomPanelMaxH      = 600;

    // ── Status bar ───────────────────────────────────────────────────────
    static constexpr int kStatusBarHeight      = 24;

    // ── Toolbar ──────────────────────────────────────────────────────────
    static constexpr int kToolbarHeight        = 40;
    static constexpr int kToolbarMinButtonW    = 28;
    static constexpr int kToolbarButtonSpacing = 4;

    // ── Tab bar ──────────────────────────────────────────────────────────
    static constexpr int kTabBarHeight         = 34;
    static constexpr int kTabMinW              = 100;
    static constexpr int kTabMaxW              = 200;
    static constexpr int kTabFixedW            = 160;

    // ── Breadcrumb bar ───────────────────────────────────────────────────
    static constexpr int kBreadcrumbBarHeight   = 24;
    static constexpr int kBreadcrumbSegPadX     = 6;
    static constexpr int kBreadcrumbSepWidth    = 14;

    // ── Splitter ─────────────────────────────────────────────────────────
    static constexpr int kSplitterHitWidth     = 8;
    static constexpr int kSplitterVisualWidth  = 1;

    // ── Layout helpers ───────────────────────────────────────────────────

    /// True if the given width is within the allowable sidebar range.
    [[nodiscard]] static constexpr auto is_sidebar_width_valid(int w) noexcept -> bool
    {
        return w >= kSidebarMinWidth && w <= kSidebarMaxWidth;
    }

    /// Clamp a requested sidebar width to the allowable range.
    [[nodiscard]] static constexpr auto clamp_sidebar_width(int w) noexcept -> int
    {
        return w < kSidebarMinWidth ? kSidebarMinWidth
             : w > kSidebarMaxWidth ? kSidebarMaxWidth
             : w;
    }

    /// Clamp a requested bottom panel height.
    [[nodiscard]] static constexpr auto clamp_bottom_panel_height(int h) noexcept -> int
    {
        return h < kBottomPanelMinH ? kBottomPanelMinH
             : h > kBottomPanelMaxH ? kBottomPanelMaxH
             : h;
    }
};

/// Surface depth tiers — matches V26PremiumTokens::SurfaceTier but specific to shell assignment.
enum class ShellSurfaceAssignment : uint8_t
{
    kShellFrame    = 0,   ///< LayoutManager frame / main background — SurfaceTier::kShell
    kToolbar       = 1,   ///< Toolbar background — SurfaceTier::kWork
    kTabBar        = 2,   ///< TabBar background — SurfaceTier::kWork
    kBreadcrumbBar = 3,   ///< BreadcrumbBar — SurfaceTier::kWork
    kStatusBar     = 4,   ///< StatusBarPanel — SurfaceTier::kShell
    kSidebar       = 5,   ///< Sidebar container — SurfaceTier::kWork
    kActivityBar   = 6,   ///< Activity bar — SurfaceTier::kShell
    kBottomPanel   = 7,   ///< Bottom panel area — SurfaceTier::kWork
    kDialog        = 8,   ///< Modal dialogs — SurfaceTier::kOverlay
};

/// Number of shell surface assignment entries.
[[nodiscard]] inline constexpr auto shell_surface_assignment_count() noexcept -> int { return 9; }


// ═══════════════════════════════════════════════════════════════════════════════
// Phase 03: Action Surface Convergence  (Menus, Toolbars, Command Palette)
// ═══════════════════════════════════════════════════════════════════════════════

/// Shared metrics for action surfaces: toolbar buttons, menu items, palette rows.
struct ActionSurfaceMetrics
{
    // ── Toolbar button metrics ───────────────────────────────────────────
    static constexpr int kButtonHeightDefault  = 28;
    static constexpr int kButtonHeightSmall    = 24;
    static constexpr int kButtonPaddingH       = 8;
    static constexpr int kButtonIconSize       = 16;
    static constexpr int kButtonGroupGap       = 8;    ///< Gap between toolbar groups
    static constexpr int kButtonSeparatorW     = 1;    ///< Separator line width

    // ── Menu / context menu metrics ──────────────────────────────────────
    static constexpr int kMenuRowHeight        = 28;
    static constexpr int kMenuPaddingH         = 12;
    static constexpr int kMenuIconWidth        = 20;
    static constexpr int kMenuAcceleratorGap   = 32;   ///< Gap before kbd shortcut
    static constexpr int kMenuSubmenuArrowW    = 12;
    static constexpr int kMenuSeparatorH       = 9;    ///< Total height of separator row
    static constexpr int kMenuBorderRadius     = 6;
    static constexpr int kMenuShadowBlur       = 8;

    // ── Command palette metrics ──────────────────────────────────────────
    static constexpr int kPaletteWidth         = 500;  ///< Fixed width
    static constexpr int kPaletteMaxHeight     = 400;
    static constexpr int kPaletteInputHeight   = 36;
    static constexpr int kPaletteRowHeight     = 28;
    static constexpr int kPalettePaddingH      = 12;
    static constexpr int kPaletteBorderRadius  = 8;
};

/// Toolbar zone identifiers for V26 three-zone toolbar layout.
enum class ToolbarZone : uint8_t
{
    kLeft   = 0,   ///< Run controls, process status
    kCenter = 1,   ///< Layout mode toggles
    kRight  = 2,   ///< Utility toggles, notification bell
};

/// Number of toolbar zones.
[[nodiscard]] inline constexpr auto toolbar_zone_count() noexcept -> int { return 3; }


// ═══════════════════════════════════════════════════════════════════════════════
// Phase 04: Navigation & Wayfinding  (Tabs, Breadcrumbs, Activity Bar)
// ═══════════════════════════════════════════════════════════════════════════════

/// Tab rendering metrics for premium convergence.
struct TabConvergenceMetrics
{
    // ── Dimensions ───────────────────────────────────────────────────────
    static constexpr int kTabHeight            = 34;
    static constexpr int kTabPaddingH          = 12;
    static constexpr int kTabGap               = 0;    ///< No gap between tabs
    static constexpr int kCloseBtnSize         = 14;
    static constexpr int kCloseBtnMargin       = 6;
    static constexpr int kModifiedDotSize      = 6;
    static constexpr int kActiveIndicatorH     = 2;    ///< Active tab bottom line
    static constexpr int kPinnedStripeW        = 2;    ///< Pinned tab left accent
    static constexpr int kOverflowChevronW     = 20;

    // ── State opacity ────────────────────────────────────────────────────
    static constexpr float kInactiveOpacity    = 0.7F;
    static constexpr float kHoverOpacity       = 0.9F;
    static constexpr float kPreviewOpacity     = 0.6F;

    // ── Layout helpers ───────────────────────────────────────────────────

    /// True when a tab is a standard close-on-click target.
    [[nodiscard]] static constexpr auto close_btn_visible_for_active() noexcept -> bool
    {
        return true;
    }
};

/// Breadcrumb rendering metrics.
struct BreadcrumbConvergenceMetrics
{
    static constexpr int kBarHeight            = 24;
    static constexpr int kSegmentPaddingH      = 6;
    static constexpr int kSeparatorWidth       = 14;   ///< Width for '›' separator
    static constexpr int kFontSizePt           = 11;
    static constexpr int kMaxVisibleSegments   = 6;    ///< Before ellipsis collapse
    static constexpr int kHoverUnderlineH      = 1;
    static constexpr float kMutedOpacity       = 0.6F; ///< Non-active segment opacity
};

/// Activity bar slot rendering metrics.
struct ActivityBarConvergenceMetrics
{
    static constexpr int kBarWidth             = 48;
    static constexpr int kSlotHeight           = 48;
    static constexpr int kIconSize             = 24;
    static constexpr int kBadgeSize            = 16;
    static constexpr int kBadgeFontSizePt      = 9;
    static constexpr int kActiveIndicatorW     = 3;    ///< Left-edge active indicator
    static constexpr int kActiveIndicatorH     = 20;   ///< Height of active indicator
    static constexpr float kInactiveOpacity    = 0.5F;
    static constexpr float kHoverOpacity       = 0.8F;
};

/// Status bar rendering metrics.
struct StatusBarConvergenceMetrics
{
    static constexpr int kHeight               = 24;
    static constexpr int kItemPaddingH         = 8;
    static constexpr int kItemGap              = 4;
    static constexpr int kIconSize             = 14;
    static constexpr int kFontSizePt           = 11;
    static constexpr int kSeparatorW           = 1;
    static constexpr int kProgressBarHeight    = 2;    ///< Thin progress bar at top
};

} // namespace markamp::ui
