/// @file V26AccessibilityPolishConvergence.h
/// @brief V26 Phases 18–20: Accessibility, micro-interactions, visual cleanup & QA convergence.
///
/// Defines accessibility standards (focus rings, contrast, reduced motion),
/// micro-interaction polish metrics (drag, resize, tactile response),
/// and visual QA gate criteria.
#pragma once

#include <cstdint>

namespace markamp::ui
{

// ═══════════════════════════════════════════════════════════════════════════════
// Phase 18: Accessibility, Visual Contrast, Focus & Reduced Motion
// ═══════════════════════════════════════════════════════════════════════════════

/// Focus ring and keyboard navigation visibility standards.
struct FocusRingStandards
{
    static constexpr int kRingWidth            = 2;
    static constexpr int kRingOffset           = 2;     ///< Gap between element and ring
    static constexpr int kRingRadius           = 4;
    static constexpr float kRingAlpha          = 0.8F;

    // ── Focus-visible only ───────────────────────────────────────────
    static constexpr bool kKeyboardOnly        = true;  ///< Only show on keyboard nav

    // ── Skip-link ────────────────────────────────────────────────────
    static constexpr int kSkipLinkHeight       = 32;
    static constexpr int kSkipLinkPaddingH     = 16;
};

/// Contrast and non-color differentiation rules.
struct ContrastStandards
{
    // ── WCAG AA minimum ratios ───────────────────────────────────────
    static constexpr float kNormalTextRatio    = 4.5F;
    static constexpr float kLargeTextRatio     = 3.0F;
    static constexpr float kUIComponentRatio   = 3.0F;

    // ── Non-color indicators ─────────────────────────────────────────
    static constexpr int kMinPatternTypes      = 2;     ///< e.g. color + icon/shape
    static constexpr int kMinStatusIndicators  = 2;     ///< e.g. color + text label

    /// True if a contrast ratio meets normal text requirement.
    [[nodiscard]] static constexpr auto passes_aa(float ratio) noexcept -> bool
    {
        return ratio >= kNormalTextRatio;
    }
};

/// Reduced motion fallback rules.
struct ReducedMotionRules
{
    // ── Replacement behavior ─────────────────────────────────────────
    static constexpr int kMaxAnimationMs       = 0;     ///< No animation in reduced mode
    static constexpr bool kInstantTransitions  = true;  ///< All transitions become instant
    static constexpr bool kDisableParallax     = true;
    static constexpr bool kStaticProgressBars  = true;  ///< No animated spinners
    static constexpr bool kReduceHoverEffects  = true;
};


// ═══════════════════════════════════════════════════════════════════════════════
// Phase 19: Micro-Interactions, Drag, Resize, Focus Transfer & Tactile Polish
// ═══════════════════════════════════════════════════════════════════════════════

/// Drag interaction visual metrics.
struct DragInteractionMetrics
{
    static constexpr int kDragThresholdPx      = 4;     ///< Min pixels before drag starts
    static constexpr int kDragGhostAlpha       = 70;    ///< 0-255 ghost opacity
    static constexpr int kDropTargetBorderW    = 2;
    static constexpr int kDropTargetRadius     = 4;
    static constexpr float kDropTargetAlpha    = 0.15F;
    static constexpr int kDragCursorOffset     = 8;
};

/// Resize interaction visual metrics.
struct ResizeInteractionMetrics
{
    static constexpr int kHandleHitWidth       = 8;     ///< Invisible hit zone
    static constexpr int kHandleVisualWidth    = 1;     ///< Visible divider
    static constexpr int kHandleHoverWidth     = 3;     ///< Expanded on hover
    static constexpr float kHandleHoverAlpha   = 0.5F;
    static constexpr int kMinPanelSize         = 100;   ///< Minimum resized panel
    static constexpr int kSnapGuideW           = 1;
};

/// Tactile response metrics for buttons, rows, and small affordances.
struct TactileResponseMetrics
{
    // ── Press feedback ───────────────────────────────────────────────
    static constexpr float kPressScale         = 0.97F;  ///< Slight scale-down on press
    static constexpr int kPressMs              = 50;
    static constexpr int kReleaseMs            = 100;

    // ── Ripple effect ────────────────────────────────────────────────
    static constexpr float kRippleAlpha        = 0.10F;
    static constexpr int kRippleDurationMs     = 300;

    // ── Toggle animation ─────────────────────────────────────────────
    static constexpr int kToggleMs             = 150;
    static constexpr int kCheckMs              = 100;
};


// ═══════════════════════════════════════════════════════════════════════════════
// Phase 20: Visual Cleanup, Consolidation & UI QA Gates
// ═══════════════════════════════════════════════════════════════════════════════

/// Visual QA acceptance criteria.
struct VisualQAGates
{
    // ── Token adoption ───────────────────────────────────────────────
    static constexpr int kMaxHardcodedColors   = 0;
    static constexpr int kMaxLocalDerivations  = 0;
    static constexpr int kMaxMagicNumbers      = 0;

    // ── Consistency targets ──────────────────────────────────────────
    static constexpr int kRequiredInteractionStates = 5; ///< rest, hover, active, focus, disabled
    static constexpr int kRequiredThemeVariants = 2;     ///< Light + dark minimum
    static constexpr int kRequiredDensityModes  = 3;     ///< Comfort, default, compact

    // ── Surface coverage ─────────────────────────────────────────────
    static constexpr int kCoreJourneyCount     = 8;      ///< Major user journeys to validate
    static constexpr int kSurfaceGroupCount    = 10;     ///< Surface groups to audit

    /// True when all hardcoded/magic-number targets are met.
    [[nodiscard]] static constexpr auto passes_cleanup_gate() noexcept -> bool
    {
        return kMaxHardcodedColors == 0
            && kMaxLocalDerivations == 0
            && kMaxMagicNumbers == 0;
    }
};

/// Core user journey definitions for QA matrix.
enum class CoreJourney : uint8_t
{
    kNewDocument       = 0,
    kOpenEditSave      = 1,
    kSearchReplace     = 2,
    kSettingsTheme     = 3,
    kExportPublish     = 4,
    kPluginInstall     = 5,
};

/// Number of core journeys.
[[nodiscard]] inline constexpr auto core_journey_count() noexcept -> int { return 6; }

} // namespace markamp::ui
