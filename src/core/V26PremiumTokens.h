/// @file V26PremiumTokens.h
/// @brief V26 Phase 01 — Premium visual system convergence tokens.
///
/// Consolidates layout roles, typography roles, surface tiers, divider
/// grammar, and interaction-state semantics into one shared header that
/// all V26 surface refinements consume. This header is additive to the
/// existing UILayoutTokens.h / VisualLanguageTokens.h / ThemeTokens.h
/// infrastructure.
///
/// Tokens here represent the *premium convergence layer* — high-level
/// semantic roles that prevent surface-by-surface drift.

#pragma once

#include <cstdint>

namespace markamp::core
{

// ============================================================================
// 1. V26 Surface Tier System
// ============================================================================

/// Semantic surface classification for consistent background / border layering.
/// Every visible surface must map to exactly one tier.
enum class SurfaceTier : uint8_t
{
    kApp       = 0, /// Application-level background (main window)
    kShell     = 1, /// Top-level shell chrome (title bar, activity bar)
    kSurface   = 2, /// Standard work surfaces (panels, sidebars)
    kElevated  = 3, /// Elevated surfaces (dialogs, command palette, popovers)
    kOverlay   = 4, /// Topmost overlay surfaces (tooltips, drag ghosts, toasts)
    kSunken    = 5, /// Recessed surfaces (editor background, input fields)
    kCanvas    = 6, /// Canvas workspace background (special creative surface)
};

/// Return the token count for surface tiers.
[[nodiscard]] constexpr auto surface_tier_count() -> std::size_t { return 7; }

// ============================================================================
// 2. V26 Divider Grammar
// ============================================================================

/// Semantic divider roles to prevent ad-hoc line/border proliferation.
enum class DividerRole : uint8_t
{
    kSectionSeparator = 0, /// Separates major sections within a surface
    kGroupDivider     = 1, /// Separates logical groups within a section
    kInlineSeparator  = 2, /// Separates inline elements (toolbar groups, menu sections)
    kSurfaceBorder    = 3, /// Border between adjacent surfaces (panel edges, sidebar)
    kSubtleRule       = 4, /// Very subtle rule for dense list/table contexts
};

/// Resolve a divider role to its thickness in logical pixels.
[[nodiscard]] constexpr auto resolve_divider_thickness(DividerRole role) -> int
{
    switch (role)
    {
        case DividerRole::kSectionSeparator: return 1;
        case DividerRole::kGroupDivider:     return 1;
        case DividerRole::kInlineSeparator:  return 1;
        case DividerRole::kSurfaceBorder:    return 1;
        case DividerRole::kSubtleRule:       return 1;
    }
    return 1;
}

/// Resolve the vertical padding (above + below combined) around a divider.
[[nodiscard]] constexpr auto resolve_divider_padding(DividerRole role) -> int
{
    switch (role)
    {
        case DividerRole::kSectionSeparator: return 16; // 8px above + 8px below
        case DividerRole::kGroupDivider:     return 8;  // 4px + 4px
        case DividerRole::kInlineSeparator:  return 8;  // 4px + 4px
        case DividerRole::kSurfaceBorder:    return 0;  // flush
        case DividerRole::kSubtleRule:       return 4;  // 2px + 2px
    }
    return 0;
}

/// Return the token count for divider roles.
[[nodiscard]] constexpr auto divider_role_count() -> std::size_t { return 5; }

// ============================================================================
// 3. V26 Typography Role System
// ============================================================================

/// Semantic typography roles for premium hierarchy convergence.
/// These map to underlying TypeScaleToken values but add semantic meaning.
enum class TypographyRole : uint8_t
{
    kPageTitle        = 0,  /// Largest heading — startup, onboarding hero
    kSectionTitle     = 1,  /// Major section heading — settings category, panel group
    kSubsectionTitle  = 2,  /// Subsection heading — settings group, form section
    kRowLabel         = 3,  /// Standard row label — settings row, list item primary
    kRowDescription   = 4,  /// Supporting description — settings help, metadata
    kCaption          = 5,  /// Smallest text — timestamps, counts, tertiary
    kMonoCode         = 6,  /// Monospace — code, paths, command palette shortcuts
    kMonoCodeSmall    = 7,  /// Small monospace — status bar items, compact metadata
    kBadgeLabel       = 8,  /// Badge/chip labels — notification counts, tags
    kButtonLabel      = 9,  /// Button text — primary/secondary/ghost actions
    kEmptyStateTitle  = 10, /// Empty state heading — empty panels, no-results
    kEmptyStateBody   = 11, /// Empty state body text — guidance, instructions
};

/// Resolve a typography role to its point size.
[[nodiscard]] constexpr auto resolve_typography_role_pt(TypographyRole role) -> int
{
    switch (role)
    {
        case TypographyRole::kPageTitle:       return 20;
        case TypographyRole::kSectionTitle:    return 14;
        case TypographyRole::kSubsectionTitle: return 13;
        case TypographyRole::kRowLabel:        return 12;
        case TypographyRole::kRowDescription:  return 11;
        case TypographyRole::kCaption:         return 10;
        case TypographyRole::kMonoCode:        return 12;
        case TypographyRole::kMonoCodeSmall:   return 10;
        case TypographyRole::kBadgeLabel:      return 10;
        case TypographyRole::kButtonLabel:     return 12;
        case TypographyRole::kEmptyStateTitle: return 16;
        case TypographyRole::kEmptyStateBody:  return 12;
    }
    return 12;
}

/// Resolve a typography role to its line height in px.
[[nodiscard]] constexpr auto resolve_typography_role_lh(TypographyRole role) -> int
{
    switch (role)
    {
        case TypographyRole::kPageTitle:       return 28;
        case TypographyRole::kSectionTitle:    return 20;
        case TypographyRole::kSubsectionTitle: return 18;
        case TypographyRole::kRowLabel:        return 18;
        case TypographyRole::kRowDescription:  return 16;
        case TypographyRole::kCaption:         return 14;
        case TypographyRole::kMonoCode:        return 18;
        case TypographyRole::kMonoCodeSmall:   return 14;
        case TypographyRole::kBadgeLabel:      return 14;
        case TypographyRole::kButtonLabel:     return 18;
        case TypographyRole::kEmptyStateTitle: return 22;
        case TypographyRole::kEmptyStateBody:  return 18;
    }
    return 18;
}

/// Whether a typography role is bold.
[[nodiscard]] constexpr auto typography_role_is_bold(TypographyRole role) -> bool
{
    switch (role)
    {
        case TypographyRole::kPageTitle:       return true;
        case TypographyRole::kSectionTitle:    return true;
        case TypographyRole::kSubsectionTitle: return true;
        case TypographyRole::kRowLabel:        return false;
        case TypographyRole::kRowDescription:  return false;
        case TypographyRole::kCaption:         return false;
        case TypographyRole::kMonoCode:        return false;
        case TypographyRole::kMonoCodeSmall:   return false;
        case TypographyRole::kBadgeLabel:      return true;
        case TypographyRole::kButtonLabel:     return true;
        case TypographyRole::kEmptyStateTitle: return true;
        case TypographyRole::kEmptyStateBody:  return false;
    }
    return false;
}

/// Whether a typography role uses monospace font.
[[nodiscard]] constexpr auto typography_role_is_mono(TypographyRole role) -> bool
{
    return role == TypographyRole::kMonoCode || role == TypographyRole::kMonoCodeSmall;
}

/// Return the token count for typography roles.
[[nodiscard]] constexpr auto typography_role_count() -> std::size_t { return 12; }

// ============================================================================
// 4. V26 Interaction State Semantics
// ============================================================================

/// Canonical interaction states that all interactive controls must support.
/// This unifies the state language across toolbar, tabs, settings, panels.
enum class InteractionState : uint8_t
{
    kRest     = 0, /// Default state — no user interaction
    kHover    = 1, /// Pointer is over the element
    kPressed  = 2, /// Mouse button is down
    kFocused  = 3, /// Keyboard focus ring
    kSelected = 4, /// Item is selected / active
    kDisabled = 5, /// Element is non-interactive
    kDragging = 6, /// Element is being dragged
    kDropTarget = 7, /// Element is a valid drop target
};

/// Return the token count for interaction states.
[[nodiscard]] constexpr auto interaction_state_count() -> std::size_t { return 8; }

// ============================================================================
// 5. V26 Panel Spacing Roles
// ============================================================================

/// Semantic spacing roles for panel / card / container contexts.
/// Prevents each surface from inventing its own spacing constants.
struct PanelSpacingTokens
{
    static constexpr int kHeaderPaddingH   = 12; // panel header horizontal padding
    static constexpr int kHeaderPaddingV   = 8;  // panel header vertical padding
    static constexpr int kContentPaddingH  = 12; // panel body horizontal padding
    static constexpr int kContentPaddingV  = 8;  // panel body vertical padding
    static constexpr int kSectionGap       = 16; // gap between major panel sections
    static constexpr int kGroupGap         = 8;  // gap between groups within a section
    static constexpr int kCardPaddingH     = 12; // card horizontal padding
    static constexpr int kCardPaddingV     = 8;  // card vertical padding
    static constexpr int kCardGap          = 8;  // gap between adjacent cards
    static constexpr int kInlineActionGap  = 4;  // gap between inline action icons
};

// ============================================================================
// 6. V26 Control Metrics
// ============================================================================

/// Standardized control dimensions for premium consistency.
struct ControlMetricTokens
{
    static constexpr int kInputHeight      = 28; // text input, dropdown height
    static constexpr int kInputHeightSmall = 24; // compact input height
    static constexpr int kToggleWidth      = 36; // toggle switch width
    static constexpr int kToggleHeight     = 20; // toggle switch height
    static constexpr int kCheckboxSize     = 16; // checkbox touch target
    static constexpr int kRadioSize        = 16; // radio button touch target
    static constexpr int kButtonHeight     = 28; // standard button height
    static constexpr int kButtonHeightSm   = 24; // compact button height
    static constexpr int kButtonPaddingH   = 12; // button horizontal padding
    static constexpr int kControlGap       = 8;  // gap between adjacent controls
    static constexpr int kLabelGap         = 4;  // gap between label and control
    static constexpr int kHelpTextGap      = 2;  // gap between control and help text
};

// ============================================================================
// 7. V26 Radius Roles (semantic aliases for CornerRadiusToken)
// ============================================================================

/// Semantic corner radius roles for consistent shape language.
enum class RadiusRole : uint8_t
{
    kControl       = 0, /// Standard controls — buttons, inputs, selects (4px)
    kCard          = 1, /// Cards, preview tiles, gallery items (6px)
    kPanel         = 2, /// Panels, floating surfaces (8px)
    kDialog        = 3, /// Dialogs, command palette (8px)
    kTooltip       = 4, /// Tooltips, compact overlays (4px)
    kBadge         = 5, /// Badges, pills, tags (9999px = fully rounded)
    kInlineControl = 6, /// Inline controls, small chips (2px)
};

/// Resolve a radius role to logical pixels.
[[nodiscard]] constexpr auto resolve_radius_role(RadiusRole role) -> int
{
    switch (role)
    {
        case RadiusRole::kControl:       return 4;
        case RadiusRole::kCard:          return 6;
        case RadiusRole::kPanel:         return 8;
        case RadiusRole::kDialog:        return 8;
        case RadiusRole::kTooltip:       return 4;
        case RadiusRole::kBadge:         return 9999;
        case RadiusRole::kInlineControl: return 2;
    }
    return 4;
}

/// Return the token count for radius roles.
[[nodiscard]] constexpr auto radius_role_count() -> std::size_t { return 7; }

// ============================================================================
// 8. V26 Feedback Severity Tokens
// ============================================================================

/// Canonical severity levels for all feedback surfaces (inline, toast, banner).
enum class FeedbackSeverity : uint8_t
{
    kInfo    = 0, /// Informational — neutral accent
    kSuccess = 1, /// Success — green family
    kWarning = 2, /// Warning — amber family
    kError   = 3, /// Error — red family
};

/// Return the token count for feedback severities.
[[nodiscard]] constexpr auto feedback_severity_count() -> std::size_t { return 4; }

// ============================================================================
// 9. V26 Motion Tier Tokens
// ============================================================================

/// Canonical motion tiers for consistent animation pacing.
enum class MotionTier : uint8_t
{
    kInstant  = 0, /// 0ms — reduced motion, immediate state changes
    kMicro    = 1, /// 80ms — hover/press response (high-frequency controls)
    kFast     = 2, /// 120ms — selection, focus ring, small state change
    kStandard = 3, /// 200ms — overlay appearance, panel reveal
    kGentle   = 4, /// 350ms — deliberate transitions, onboarding
};

/// Resolve a motion tier to milliseconds.
[[nodiscard]] constexpr auto resolve_motion_tier_ms(MotionTier tier) -> int
{
    switch (tier)
    {
        case MotionTier::kInstant:  return 0;
        case MotionTier::kMicro:    return 80;
        case MotionTier::kFast:     return 120;
        case MotionTier::kStandard: return 200;
        case MotionTier::kGentle:   return 350;
    }
    return 200;
}

/// Return the token count for motion tiers.
[[nodiscard]] constexpr auto motion_tier_count() -> std::size_t { return 5; }

// ============================================================================
// 10. V26 Phase/Task Counts
// ============================================================================

/// Total number of V26 phases.
[[nodiscard]] constexpr auto v26_phase_count() -> int { return 20; }

/// Total number of V26 tasks (3 per phase).
[[nodiscard]] constexpr auto v26_task_count() -> int { return 60; }

} // namespace markamp::core
