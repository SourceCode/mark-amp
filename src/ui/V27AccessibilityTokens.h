/// @file V27AccessibilityTokens.h
/// @brief V27 Phase 18 — Accessibility visuals, contrast, focus, reduced motion.
#pragma once
#include <cstdint>
namespace markamp::ui
{
struct V27FocusRingTokens {
    static constexpr int kThickness = 2;
    static constexpr int kOffset = 2;
    static constexpr int kRadius = 4;
    static constexpr int kTransitionMs = 100;
    static constexpr int kHighContrastThickness = 3;
    static constexpr int kHighContrastOffset = 1;
};
struct V27ContrastTokens {
    static constexpr int kMinContrastNormal = 45;  ///< 4.5:1 ratio (x10)
    static constexpr int kMinContrastLarge = 30;   ///< 3.0:1 ratio (x10)
    static constexpr int kMinContrastUI = 30;      ///< 3.0:1 for UI elements
    static constexpr int kEnhancedContrastNormal = 70; ///< 7.0:1 for AAA
    static constexpr int kEnhancedContrastLarge = 45;  ///< 4.5:1 for AAA large
};
struct V27ReducedMotionTokens {
    static constexpr int kTransitionMs = 0;       ///< Instant transitions
    static constexpr int kFadeMs = 0;             ///< No fades
    static constexpr bool kDisableSpring = true;   ///< No spring physics
    static constexpr bool kDisableParallax = true; ///< No parallax
    static constexpr bool kStaticFocusRing = true; ///< Solid focus ring
    static constexpr int kProgressBarMs = 0;       ///< Static progress
};
} // namespace markamp::ui
