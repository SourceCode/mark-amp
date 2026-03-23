/// @file V27InteractionStates.h
/// @brief V27 Phase 02 T03 — Redesigned interaction state system.
///
/// Unified hover, press, focus, disabled, selected, drag, error, and warning
/// state definitions. Integrates with the canonical icon state system from
/// Phase 01 and the V27 visual language tokens.
#pragma once

#include <cstdint>

namespace markamp::core
{

// ============================================================================
// V27 State Color Adjustments
// ============================================================================

/// V27 state color adjustment — how a base theme color is modified for a state.
struct V27StateColorAdjust
{
    double alpha_multiply{1.0};  ///< Multiply the alpha channel
    int lightness_shift{0};      ///< Shift lightness (-100 to 100, 0 = no change)
    int saturation_shift{0};     ///< Shift saturation (-100 to 100)

    [[nodiscard]] constexpr auto is_identity() const noexcept -> bool
    {
        return alpha_multiply == 1.0 && lightness_shift == 0 && saturation_shift == 0;
    }
};

/// V27 icon-specific state properties.
struct V27IconStateProps
{
    double opacity{1.0};         ///< Icon opacity in this state
    int size_adjust_px{0};       ///< Size adjustment (e.g. -1 for pressed shrink)
    bool show_badge{true};       ///< Whether to show badge overlays in this state
};

// ============================================================================
// V27 Surface State — combines color adjust + icon props + timing
// ============================================================================

/// Complete V27 surface state definition.
struct V27SurfaceState
{
    V27StateColorAdjust bg_adjust;    ///< Background color adjustment
    V27StateColorAdjust fg_adjust;    ///< Foreground/text color adjustment
    V27StateColorAdjust border_adjust;///< Border color adjustment
    V27IconStateProps icon_props;     ///< Icon-specific state properties
    int transition_ms{80};            ///< Transition duration to this state
    bool cursor_pointer{false};       ///< Whether to show pointer cursor
};

// ============================================================================
// V27 State Definitions — default visuals for each interaction state
// ============================================================================

/// V27 default hover state.
[[nodiscard]] constexpr auto v27_state_hover() noexcept -> V27SurfaceState
{
    return {
        {1.0, 5, 0},   // bg: slight lighten
        {1.0, 0, 0},   // fg: unchanged
        {1.0, 0, 0},   // border: unchanged
        {1.0, 0, true}, // icon: full opacity
        80,             // 80ms transition
        true            // pointer cursor
    };
}

/// V27 default pressed state.
[[nodiscard]] constexpr auto v27_state_pressed() noexcept -> V27SurfaceState
{
    return {
        {1.0, -3, 0},  // bg: slight darken
        {1.0, 0, 0},   // fg: unchanged
        {1.0, 0, 0},   // border: unchanged
        {0.9, -1, true},// icon: slightly smaller + reduced opacity
        40,             // 40ms transition (fast snap)
        true
    };
}

/// V27 default focused state.
[[nodiscard]] constexpr auto v27_state_focused() noexcept -> V27SurfaceState
{
    return {
        {1.0, 0, 0},   // bg: unchanged
        {1.0, 0, 0},   // fg: unchanged
        {1.0, 15, 10}, // border: lightened + saturated (focus ring)
        {1.0, 0, true},
        100,
        false
    };
}

/// V27 default disabled state.
[[nodiscard]] constexpr auto v27_state_disabled() noexcept -> V27SurfaceState
{
    return {
        {0.5, 0, -20}, // bg: half alpha + desaturated
        {0.4, 0, -30}, // fg: low alpha + desaturated
        {0.3, 0, -20}, // border: very low alpha
        {0.35, 0, false}, // icon: low opacity, no badge
        0,              // instant (no transition to disabled)
        false
    };
}

/// V27 default selected state.
[[nodiscard]] constexpr auto v27_state_selected() noexcept -> V27SurfaceState
{
    return {
        {1.0, -8, 5},  // bg: darken + slight saturation
        {1.0, 0, 0},   // fg: unchanged
        {1.0, -5, 5},  // border: subtle emphasis
        {1.0, 0, true},
        100,
        false
    };
}

/// V27 default error state.
[[nodiscard]] constexpr auto v27_state_error() noexcept -> V27SurfaceState
{
    return {
        {1.0, 0, 0},   // bg: from theme error color
        {1.0, 0, 0},   // fg: from theme error fg
        {1.0, 0, 0},   // border: from theme error border
        {1.0, 0, true},
        100,
        false
    };
}

/// V27 default warning state.
[[nodiscard]] constexpr auto v27_state_warning() noexcept -> V27SurfaceState
{
    return {
        {1.0, 0, 0},   // bg: from theme warning color
        {1.0, 0, 0},   // fg: from theme warning fg
        {1.0, 0, 0},   // border: from theme warning border
        {1.0, 0, true},
        100,
        false
    };
}

/// V27 minimum hit target sizes.
struct V27HitTargets
{
    static constexpr int kMinimum = 24;    ///< Absolute minimum touch/click target
    static constexpr int kStandard = 28;   ///< Standard interactive target
    static constexpr int kComfortable = 32;///< Comfortable touch target
    static constexpr int kLarge = 36;      ///< Large action target
};

/// V27 focus ring configuration.
struct V27FocusRingConfig
{
    static constexpr int kThickness = 2;    ///< Ring thickness in px
    static constexpr int kOffset = 2;       ///< Ring offset from element bounds
    static constexpr int kRadius = 4;       ///< Ring corner radius
    static constexpr int kTransitionMs = 100;///< Appear/disappear duration
};

} // namespace markamp::core
