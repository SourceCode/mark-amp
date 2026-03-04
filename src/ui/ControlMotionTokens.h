#pragma once

/**
 * @file ControlMotionTokens.h
 * @brief Phase 31 Task 1: Standardized motion tokens for control surfaces.
 *
 * Defines reusable motion tokens (duration, easing, delay) for hover,
 * press, reveal, dismiss interactions. Supports reduced-motion variants.
 */

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Easing function identifier.
enum class EasingFunction : uint8_t
{
    kLinear,
    kEaseIn,
    kEaseOut,
    kEaseInOut,
    kSpring,
    kBounce,
};

/// A single motion token definition.
struct MotionToken
{
    std::string name;
    int duration_ms{200};
    EasingFunction easing{EasingFunction::kEaseOut};
    int delay_ms{0};

    /// Get the token with reduced-motion settings applied.
    [[nodiscard]] auto reduced() const -> MotionToken;

    /// Get the human-readable easing name.
    [[nodiscard]] auto easing_name() const -> std::string;

    /// Check if this is effectively instant (≤ 16ms, one frame).
    [[nodiscard]] auto is_instant() const -> bool;
};

/// Named motion token identifiers.
enum class MotionTokenId : uint8_t
{
    kHoverReveal,   ///< Mouse hover state transition
    kHoverDismiss,  ///< Mouse leave state transition
    kPressResponse, ///< Button/control press feedback
    kPressRelease,  ///< Button/control release return
    kFadeIn,        ///< Content appear animation
    kFadeOut,       ///< Content disappear animation
    kSlideIn,       ///< Panel/drawer slide in
    kSlideOut,      ///< Panel/drawer slide out
    kBounceIn,      ///< Playful emphasis entrance
    kPopIn,         ///< Scale-up entrance (notifications, badges)
    kDismiss,       ///< Quick dismissal animation
    kFocusRing,     ///< Focus indicator appearance
};

/**
 * @brief Registry of standard motion tokens for control surfaces.
 *
 * Provides named tokens with consistent timing across the UI, plus
 * reduced-motion variants that respect accessibility settings.
 */
class ControlMotionTokens
{
public:
    /// Get a specific motion token.
    [[nodiscard]] static auto get(MotionTokenId token_id) -> MotionToken;

    /// Get all defined motion tokens.
    [[nodiscard]] static auto all() -> std::vector<MotionToken>;

    /// Get token count.
    [[nodiscard]] static auto count() -> int;

    /// Get all tokens with reduced-motion applied.
    [[nodiscard]] static auto reduced_motion_variants() -> std::vector<MotionToken>;

    /// Scale all durations by a factor (from FxMotionPreset::motion_intensity).
    [[nodiscard]] static auto scaled(MotionTokenId token_id, float scale) -> MotionToken;

    /// Get the token ID name as a string.
    [[nodiscard]] static auto token_name(MotionTokenId token_id) -> std::string;

    /// Check if a duration is within the recommended latency budget.
    [[nodiscard]] static auto within_budget(int duration_ms) -> bool;

    /// Get the recommended maximum duration in ms.
    [[nodiscard]] static auto max_budget_ms() -> int;
};

} // namespace markamp::ui
