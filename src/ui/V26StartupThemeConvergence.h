/// @file V26StartupThemeConvergence.h
/// @brief V26 Phases 14–15: Startup, welcome, onboarding, empty-state elegance,
///        and theme parity / token adoption / cross-surface correctness.
///
/// Defines premium metrics for the startup/welcome experience, shared empty-state
/// templates, onboarding cues, and theme validation/parity rules.
#pragma once

#include <cstdint>

namespace markamp::ui
{

// ═══════════════════════════════════════════════════════════════════════════════
// Phase 14: Startup, Welcome, Onboarding & Empty-State Elegance
// ═══════════════════════════════════════════════════════════════════════════════

/// Startup / welcome surface metrics.
struct StartupV26Metrics
{
    // ── Shell ────────────────────────────────────────────────────────
    static constexpr int kMaxContentWidth      = 700;
    static constexpr int kPaddingH             = 48;
    static constexpr int kPaddingV             = 40;

    // ── Logo / hero ──────────────────────────────────────────────────
    static constexpr int kLogoSize             = 64;
    static constexpr int kHeroTitlePt          = 28;
    static constexpr int kHeroSubtitlePt       = 16;
    static constexpr int kHeroGap              = 12;

    // ── Quick actions grid ───────────────────────────────────────────
    static constexpr int kActionCardW          = 200;
    static constexpr int kActionCardH          = 80;
    static constexpr int kActionCardRadius     = 8;
    static constexpr int kActionCardGap        = 12;
    static constexpr int kActionIconSize       = 24;
    static constexpr int kActionTitlePt        = 13;
    static constexpr int kActionDescPt         = 11;

    // ── Recent files list ────────────────────────────────────────────
    static constexpr int kRecentRowH           = 32;
    static constexpr int kRecentIconSize       = 16;
    static constexpr int kRecentMaxItems       = 8;
};

/// Shared empty-state premium template metrics (extends EmptyStateMetrics in V26PanelConvergence.h).
struct EmptyStateV26Metrics
{
    // ── Illustration ─────────────────────────────────────────────────
    static constexpr int kIllustrationSize     = 120;   ///< Large empty-state SVG
    static constexpr float kIllustrationAlpha  = 0.35F;

    // ── Action prompts ───────────────────────────────────────────────
    static constexpr int kPrimaryButtonH       = 32;
    static constexpr int kPrimaryButtonPadH    = 16;
    static constexpr int kSecondaryLinkPt      = 12;
    static constexpr int kActionSpacing        = 12;

    // ── Educational hints ────────────────────────────────────────────
    static constexpr int kHintCardW            = 260;
    static constexpr int kHintCardH            = 100;
    static constexpr int kHintCardRadius       = 6;
    static constexpr int kHintIconSize         = 20;
    static constexpr int kHintTitlePt          = 12;
    static constexpr int kHintBodyPt           = 11;
    static constexpr int kHintCardGap          = 8;
};

/// Onboarding / discoverability surface metrics.
struct OnboardingV26Metrics
{
    // ── Feature spotlight overlay ─────────────────────────────────────
    static constexpr int kSpotlightPadding     = 8;
    static constexpr int kSpotlightRadius      = 8;
    static constexpr int kSpotlightBorderW     = 2;

    // ── Coachmark tooltip ────────────────────────────────────────────
    static constexpr int kCoachmarkMaxW        = 280;
    static constexpr int kCoachmarkPadH        = 12;
    static constexpr int kCoachmarkPadV        = 10;
    static constexpr int kCoachmarkRadius      = 6;
    static constexpr int kCoachmarkArrowSize   = 8;
    static constexpr int kCoachmarkTitlePt     = 13;
    static constexpr int kCoachmarkBodyPt      = 11;

    // ── Dot indicators ───────────────────────────────────────────────
    static constexpr int kDotSize              = 8;
    static constexpr int kDotGap               = 6;
    static constexpr float kDotInactiveAlpha   = 0.3F;
    static constexpr float kDotActiveAlpha     = 1.0F;
};


// ═══════════════════════════════════════════════════════════════════════════════
// Phase 15: Theme Parity, Token Adoption & Cross-Surface Correctness
// ═══════════════════════════════════════════════════════════════════════════════

/// Theme parity validation metrics and rules.
struct ThemeParityRules
{
    // ── Minimum contrast ratios ──────────────────────────────────────
    static constexpr float kTextOnBgMinContrast     = 4.5F;   ///< WCAG AA (normal text)
    static constexpr float kLargeTextMinContrast    = 3.0F;   ///< WCAG AA (large text)
    static constexpr float kUIComponentMinContrast  = 3.0F;   ///< Non-text UI components

    // ── Token adoption targets ───────────────────────────────────────
    static constexpr int kMaxHardcodedColors        = 0;       ///< Target: zero hardcoded
    static constexpr int kMaxLocalDerivations       = 0;       ///< Target: zero local derives

    // ── State coverage ───────────────────────────────────────────────
    static constexpr int kRequiredStates            = 5;       ///< rest, hover, active, focus, disabled

    /// Number of required contrast checks per surface.
    [[nodiscard]] static constexpr auto contrast_check_count() noexcept -> int { return 3; }

    /// Validate a given contrast ratio against text minimum.
    [[nodiscard]] static constexpr auto is_text_contrast_valid(float ratio) noexcept -> bool
    {
        return ratio >= kTextOnBgMinContrast;
    }

    /// Validate a given contrast ratio against large text minimum.
    [[nodiscard]] static constexpr auto is_large_text_contrast_valid(float ratio) noexcept -> bool
    {
        return ratio >= kLargeTextMinContrast;
    }
};

} // namespace markamp::ui
