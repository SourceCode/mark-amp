// ============================================================================
// File: src/rendering/FxTransitionEngine.h
// Phase 28: FX Visual Effects System — Animated FX State Transitions
// ============================================================================
#pragma once

#include "FxPresetRegistry.h"

#include <cstdint>
#include <optional>
#include <string>

namespace markamp::core
{
class EventBus;
} // namespace markamp::core

namespace markamp::rendering
{

/// Easing functions for FX transitions.
enum class EasingFunction : uint8_t
{
    kLinear,    ///< Constant speed
    kEaseIn,    ///< Slow start, fast end
    kEaseOut,   ///< Fast start, slow end
    kEaseInOut, ///< Slow start and end
    kSpring,    ///< Overshoot with spring-like settle
};

/// Configuration for an FX transition.
struct TransitionConfig
{
    float duration_ms{500.0F};
    EasingFunction easing{EasingFunction::kEaseInOut};
    bool crossfade{true}; ///< Blend between presets vs. hard cut
};

/// Snapshot of a transition in progress.
struct TransitionState
{
    std::string from_preset;
    std::string to_preset;
    float elapsed_ms{0.0F};
    float duration_ms{500.0F};
    EasingFunction easing{EasingFunction::kEaseInOut};

    /// 0.0 = start (from), 1.0 = end (to).
    [[nodiscard]] auto progress() const noexcept -> float;
};

/// Manages animated transitions between FX presets.
class FxTransitionEngine
{
public:
    explicit FxTransitionEngine(core::EventBus& event_bus);

    // ── Transition lifecycle ──

    /// Start a transition from the current preset to a target preset.
    auto start_transition(const std::string& from_preset,
                          const std::string& to_preset,
                          const TransitionConfig& config = {}) -> void;

    /// Advance the transition by the given time delta. Returns true if active.
    auto update(float delta_ms) -> bool;

    /// Cancel the current transition immediately.
    auto cancel() -> void;

    /// Check if a transition is currently active.
    [[nodiscard]] auto is_transitioning() const noexcept -> bool;

    /// Get the current transition state (nullopt if not transitioning).
    [[nodiscard]] auto state() const -> std::optional<TransitionState>;

    // ── Interpolation ──

    /// Get the eased progress value (0.0–1.0) for the current transition.
    [[nodiscard]] auto eased_progress() const noexcept -> float;

    /// Interpolate a float value between from and to based on current progress.
    [[nodiscard]] auto interpolate(float from_val, float to_val) const noexcept -> float;

    /// Interpolate a MotionPreset between two presets based on current progress.
    [[nodiscard]] auto interpolate_motion(const MotionPreset& from_preset,
                                          const MotionPreset& to_preset) const -> MotionPreset;

    // ── Easing ──

    /// Apply an easing function to a linear progress value.
    [[nodiscard]] static auto apply_easing(float linear_progress, EasingFunction easing_func)
        -> float;

    /// Get the display name for an easing function.
    [[nodiscard]] static auto easing_name(EasingFunction easing_func) -> std::string_view;

    // ── Configuration ──

    /// Set/get default transition configuration.
    auto set_default_config(const TransitionConfig& config) -> void;
    [[nodiscard]] auto default_config() const noexcept -> const TransitionConfig&;

    /// Total number of transitions completed.
    [[nodiscard]] auto transitions_completed() const noexcept -> uint32_t;

    /// Whether at least one transition has completed.
    [[nodiscard]] auto has_completed() const noexcept -> bool
    {
        return transitions_completed_ > 0;
    }

    /// Alias for transitions_completed().
    [[nodiscard]] auto transition_count() const noexcept -> uint32_t
    {
        return transitions_completed_;
    }

private:
    core::EventBus& event_bus_;
    std::optional<TransitionState> state_;
    TransitionConfig default_config_;
    uint32_t transitions_completed_{0};
};

} // namespace markamp::rendering
