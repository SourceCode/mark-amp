/// @file V27VisualLanguage.h
/// @brief V27 Phase 02 — Premium visual language token system.
///
/// Redesigned spacing, density, depth, border/radius, and interaction state
/// tokens for the V27 visual overhaul. These supplement the existing
/// UILayoutTokens.h and VisualLanguageTokens.h systems with V27-specific
/// premium refinements.
#pragma once

#include <cstdint>

namespace markamp::core
{

// ============================================================================
// 1. V27 Depth Hierarchy — surface layering and elevation
// ============================================================================

/// V27 depth tier for surface layering.
enum class V27DepthTier
{
    kSunken,    ///< Below base level (e.g. gutters, wells)
    kBase,      ///< Standard workspace level
    kRaised,    ///< Slightly above base (e.g. cards, panels)
    kElevated,  ///< Clearly above base (e.g. toolbars, tab bars)
    kOverlay,   ///< Floating above content (e.g. dropdowns, menus)
    kPopover,   ///< Highest layer (e.g. modals, dialogs)
};

/// Total V27 depth tiers.
[[nodiscard]] constexpr auto v27_depth_tier_count() noexcept -> int { return 6; }

/// V27 depth tier shadow blur radius in pixels.
[[nodiscard]] constexpr auto v27_depth_blur_px(V27DepthTier tier) noexcept -> int
{
    switch (tier)
    {
    case V27DepthTier::kSunken:   return 0;
    case V27DepthTier::kBase:     return 0;
    case V27DepthTier::kRaised:   return 2;
    case V27DepthTier::kElevated: return 4;
    case V27DepthTier::kOverlay:  return 8;
    case V27DepthTier::kPopover:  return 16;
    }
    return 0;
}

/// V27 depth tier shadow Y offset in pixels.
[[nodiscard]] constexpr auto v27_depth_offset_y(V27DepthTier tier) noexcept -> int
{
    switch (tier)
    {
    case V27DepthTier::kSunken:   return 0;
    case V27DepthTier::kBase:     return 0;
    case V27DepthTier::kRaised:   return 1;
    case V27DepthTier::kElevated: return 2;
    case V27DepthTier::kOverlay:  return 4;
    case V27DepthTier::kPopover:  return 8;
    }
    return 0;
}

// ============================================================================
// 2. V27 Density Profiles — compact, default, comfortable
// ============================================================================

/// V27 density profile.
enum class V27DensityProfile
{
    kCompact,     ///< Tight density for power users
    kDefault,     ///< Standard density
    kComfortable, ///< Spacious density for readability
};

/// Total V27 density profiles.
[[nodiscard]] constexpr auto v27_density_profile_count() noexcept -> int { return 3; }

/// V27 density tokens for a given profile.
struct V27DensityTokens
{
    int row_height;          ///< Standard row height
    int row_height_compact;  ///< Compact row height
    int section_gap;         ///< Gap between sections
    int group_gap;           ///< Gap between groups within a section
    int item_gap;            ///< Gap between individual items

    /// Create tokens for a density profile.
    [[nodiscard]] static constexpr auto for_profile(V27DensityProfile profile) noexcept
        -> V27DensityTokens
    {
        switch (profile)
        {
        case V27DensityProfile::kCompact:
            return {20, 16, 16, 8, 2};
        case V27DensityProfile::kDefault:
            return {24, 20, 20, 12, 4};
        case V27DensityProfile::kComfortable:
            return {28, 24, 24, 16, 6};
        }
        return {24, 20, 20, 12, 4};
    }
};

// ============================================================================
// 3. V27 Premium Spacing Adjustments
// ============================================================================

/// V27 premium spacing overrides.
struct V27SpacingTokens
{
    static constexpr int kShellMarginH = 8;     ///< Horizontal shell margin
    static constexpr int kShellMarginV = 4;     ///< Vertical shell margin
    static constexpr int kPanelPadding = 12;    ///< Standard panel padding
    static constexpr int kCardPadding = 16;     ///< Card interior padding
    static constexpr int kDialogPadding = 24;   ///< Dialog interior padding
    static constexpr int kSectionDivider = 1;   ///< Section divider thickness
    static constexpr int kGroupDivider = 1;     ///< Group divider thickness
    static constexpr int kContentMaxWidth = 720; ///< Max content width for readability
};

// ============================================================================
// 4. V27 Premium Border / Radius Refinements
// ============================================================================

/// V27 radius tokens refined for premium feel.
struct V27RadiusTokens
{
    static constexpr int kNone = 0;            ///< Sharp corners
    static constexpr int kSubtle = 2;          ///< Minimal rounding
    static constexpr int kControl = 4;         ///< Standard control
    static constexpr int kCard = 6;            ///< Cards, previews, panels
    static constexpr int kDialog = 8;          ///< Dialogs, modals
    static constexpr int kLarge = 12;          ///< Large surfaces, heroes
    static constexpr int kPill = 9999;         ///< Fully rounded
};

/// V27 border weight tokens.
struct V27BorderTokens
{
    static constexpr int kThin = 1;            ///< Standard borders
    static constexpr int kMedium = 2;          ///< Emphasis borders
    static constexpr int kFocusRing = 2;       ///< Focus ring thickness
};

// ============================================================================
// 5. V27 Interaction State Refinements
// ============================================================================

/// V27 interaction state.
enum class V27InteractionState
{
    kIdle,
    kHover,
    kPressed,
    kFocused,
    kSelected,
    kDisabled,
    kDragging,
    kDropTarget,
};

/// Total V27 interaction states.
[[nodiscard]] constexpr auto v27_interaction_state_count() noexcept -> int { return 8; }

/// V27 interaction state visual properties.
struct V27InteractionVisuals
{
    double opacity;         ///< Opacity multiplier (0.0–1.0)
    int timing_ms;          ///< Transition duration in ms
    bool show_overlay;      ///< Whether to show hover/press overlay

    /// Get visuals for a state.
    [[nodiscard]] static constexpr auto for_state(V27InteractionState state) noexcept
        -> V27InteractionVisuals
    {
        switch (state)
        {
        case V27InteractionState::kIdle:       return {1.0, 0, false};
        case V27InteractionState::kHover:      return {1.0, 80, true};
        case V27InteractionState::kPressed:    return {0.9, 40, true};
        case V27InteractionState::kFocused:    return {1.0, 100, false};
        case V27InteractionState::kSelected:   return {1.0, 0, false};
        case V27InteractionState::kDisabled:   return {0.4, 0, false};
        case V27InteractionState::kDragging:   return {0.8, 0, true};
        case V27InteractionState::kDropTarget: return {1.0, 100, true};
        }
        return {1.0, 0, false};
    }
};

// ============================================================================
// 6. V27 Phase / Task Metadata
// ============================================================================

/// V27 phase count.
[[nodiscard]] constexpr auto v27_phase_count() noexcept -> int { return 20; }

/// V27 task count.
[[nodiscard]] constexpr auto v27_task_count() noexcept -> int { return 60; }

} // namespace markamp::core
