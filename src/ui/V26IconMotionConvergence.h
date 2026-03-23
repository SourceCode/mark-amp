/// @file V26IconMotionConvergence.h
/// @brief V26 Phases 16–17: Iconography, optical alignment, semantic completion,
///        motion language, transitions & responsiveness convergence.
#pragma once

#include <cstdint>

namespace markamp::ui
{

// ═══════════════════════════════════════════════════════════════════════════════
// Phase 16: Iconography, Optical Alignment & Semantic Completion
// ═══════════════════════════════════════════════════════════════════════════════

/// Icon size roles — standardized across all surfaces.
enum class IconSizeRole : uint8_t
{
    kTiny   = 0,   ///< 12px — inline badges, status dots
    kSmall  = 1,   ///< 14px — status bar, dense rows
    kMedium = 2,   ///< 16px — toolbar, tree rows, menus
    kLarge  = 3,   ///< 20px — command palette, settings
    kHero   = 4,   ///< 24px — activity bar, empty states
    kDisplay = 5,  ///< 32px+ — startup, onboarding
};

/// Icon presentation metrics.
struct IconConvergenceMetrics
{
    // ── Size values (pixels) ─────────────────────────────────────────
    static constexpr int kTinySize     = 12;
    static constexpr int kSmallSize    = 14;
    static constexpr int kMediumSize   = 16;
    static constexpr int kLargeSize    = 20;
    static constexpr int kHeroSize     = 24;
    static constexpr int kDisplaySize  = 32;

    // ── Optical alignment ────────────────────────────────────────────
    static constexpr int kTextGapH     = 6;    ///< Standard icon-to-text gap
    static constexpr int kTextGapHSmall = 4;   ///< Compact icon-to-text gap
    static constexpr int kOpticalNudge = 1;    ///< 1px vertical nudge for optical center

    // ── Icon button hit targets ──────────────────────────────────────
    static constexpr int kHitTargetMin = 24;   ///< Minimum touch/click target
    static constexpr int kHitTargetIdeal = 28; ///< Ideal interactive target

    /// Number of icon size roles.
    [[nodiscard]] static constexpr auto role_count() noexcept -> int { return 6; }

    /// Resolve pixel size for a given role.
    [[nodiscard]] static constexpr auto size_for_role(IconSizeRole role) noexcept -> int
    {
        switch (role)
        {
            case IconSizeRole::kTiny:    return kTinySize;
            case IconSizeRole::kSmall:   return kSmallSize;
            case IconSizeRole::kMedium:  return kMediumSize;
            case IconSizeRole::kLarge:   return kLargeSize;
            case IconSizeRole::kHero:    return kHeroSize;
            case IconSizeRole::kDisplay: return kDisplaySize;
        }
        return kMediumSize;
    }
};

/// Icon semantic categories for rationalized usage.
enum class IconCategory : uint8_t
{
    kNavigation  = 0,   ///< Arrows, chevrons, breadcrumbs
    kAction      = 1,   ///< Edit, delete, save, run
    kStatus      = 2,   ///< Error, warning, info, success
    kFile        = 3,   ///< File type icons, folder icons
    kUI          = 4,   ///< Collapse, expand, close, minimize
};


// ═══════════════════════════════════════════════════════════════════════════════
// Phase 17: Motion, Transitions & Responsiveness Feel
// ═══════════════════════════════════════════════════════════════════════════════

/// Motion duration tiers (aligned with V26PremiumTokens::MotionTier).
struct MotionDurationMetrics
{
    // ── Duration values (milliseconds) ───────────────────────────────
    static constexpr int kInstantMs    = 0;
    static constexpr int kMicroMs      = 50;    ///< Opacity flips, dot animations
    static constexpr int kFastMs       = 100;   ///< Hover state changes
    static constexpr int kNormalMs     = 200;   ///< Panel slides, menu opens
    static constexpr int kSlowMs       = 350;   ///< Dialog entrance, tab switch
    static constexpr int kDramaticMs   = 500;   ///< Startup reveal, theme switch

    /// Number of duration tiers.
    [[nodiscard]] static constexpr auto tier_count() noexcept -> int { return 6; }
};

/// Easing function references (CSS cubic-bezier equivalents as named constants).
enum class EasingCurve : uint8_t
{
    kLinear        = 0,
    kEaseOut       = 1,   ///< Decelerate: cubic-bezier(0, 0, 0.2, 1)
    kEaseIn        = 2,   ///< Accelerate: cubic-bezier(0.4, 0, 1, 1)
    kEaseInOut     = 3,   ///< S-curve: cubic-bezier(0.4, 0, 0.2, 1)
    kSpring        = 4,   ///< Overshoot bounce for playful UI
};

/// Transition application rules.
struct TransitionRules
{
    // ── Hover response ───────────────────────────────────────────────
    static constexpr int kHoverEnterMs  = 100;
    static constexpr int kHoverLeaveMs  = 150;  ///< Slightly slower exit
    static constexpr int kPressMs       = 50;   ///< Immediate press feedback

    // ── Panel transitions ────────────────────────────────────────────
    static constexpr int kPanelSlideMs  = 200;
    static constexpr int kPanelFadeMs   = 150;

    // ── Menu / palette ───────────────────────────────────────────────
    static constexpr int kMenuOpenMs    = 150;
    static constexpr int kMenuCloseMs   = 100;
    static constexpr int kPaletteOpenMs = 200;
    static constexpr int kPaletteCloseMs = 150;

    // ── Dialog ───────────────────────────────────────────────────────
    static constexpr int kDialogOpenMs  = 250;
    static constexpr int kDialogCloseMs = 200;

    /// Validate that enter transitions are >= exit transitions where appropriate.
    [[nodiscard]] static constexpr auto is_timing_consistent() noexcept -> bool
    {
        return kHoverLeaveMs >= kHoverEnterMs
            && kDialogOpenMs >= kDialogCloseMs;
    }
};

} // namespace markamp::ui
