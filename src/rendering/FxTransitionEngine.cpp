// ============================================================================
// File: src/rendering/FxTransitionEngine.cpp
// Phase 28: FX Visual Effects System — Animated FX State Transitions
// ============================================================================

#include "FxTransitionEngine.h"

#include "core/EventBus.h"
#include "core/Events.h"

#include <algorithm>
#include <cmath>

namespace markamp::rendering
{

// ── TransitionState ──

auto TransitionState::progress() const noexcept -> float
{
    if (duration_ms <= 0.0F)
    {
        return 1.0F;
    }
    return std::clamp(elapsed_ms / duration_ms, 0.0F, 1.0F);
}

// ── FxTransitionEngine ──

FxTransitionEngine::FxTransitionEngine(core::EventBus& event_bus)
    : event_bus_(event_bus)
{
}

auto FxTransitionEngine::start_transition(const std::string& from_preset,
                                          const std::string& to_preset,
                                          const TransitionConfig& config) -> void
{
    // Cancel any in-progress transition.
    if (state_.has_value())
    {
        cancel();
    }

    TransitionState new_state;
    new_state.from_preset = from_preset;
    new_state.to_preset = to_preset;
    new_state.elapsed_ms = 0.0F;
    new_state.duration_ms = config.duration_ms;
    new_state.easing = config.easing;
    state_ = std::move(new_state);

    core::events::FxTransitionStartedEvent started_event;
    started_event.preset_name = to_preset;
    started_event.duration_ms = static_cast<int32_t>(config.duration_ms);
    event_bus_.publish(started_event);
}

auto FxTransitionEngine::update(float delta_ms) -> bool
{
    if (!state_.has_value())
    {
        return false;
    }

    state_->elapsed_ms += delta_ms;

    if (state_->elapsed_ms >= state_->duration_ms)
    {
        // Transition complete.
        core::events::FxTransitionCompletedEvent completed;
        completed.preset_name = state_->to_preset;
        completed.was_cancelled = false;
        event_bus_.publish(completed);

        ++transitions_completed_;
        state_.reset();
        return false;
    }

    return true;
}

auto FxTransitionEngine::cancel() -> void
{
    if (!state_.has_value())
    {
        return;
    }

    core::events::FxTransitionCompletedEvent completed;
    completed.preset_name = state_->to_preset;
    completed.was_cancelled = true;
    event_bus_.publish(completed);

    state_.reset();
}

auto FxTransitionEngine::is_transitioning() const noexcept -> bool
{
    return state_.has_value();
}

auto FxTransitionEngine::state() const -> std::optional<TransitionState>
{
    return state_;
}

auto FxTransitionEngine::eased_progress() const noexcept -> float
{
    if (!state_.has_value())
    {
        return 0.0F;
    }
    return apply_easing(state_->progress(), state_->easing);
}

auto FxTransitionEngine::interpolate(float from_val, float to_val) const noexcept -> float
{
    const float progress = eased_progress();
    return from_val + (to_val - from_val) * progress;
}

auto FxTransitionEngine::interpolate_motion(const MotionPreset& from_preset,
                                            const MotionPreset& to_preset) const -> MotionPreset
{
    MotionPreset result;
    result.name = is_transitioning() ? "Transitioning" : to_preset.name;
    result.preset_id = MotionPresetId::kCustom;

    result.motion_intensity = interpolate(from_preset.motion_intensity, to_preset.motion_intensity);
    result.glow_intensity = interpolate(from_preset.glow_intensity, to_preset.glow_intensity);
    result.bloom_intensity = interpolate(from_preset.bloom_intensity, to_preset.bloom_intensity);
    result.chroma_intensity = interpolate(from_preset.chroma_intensity, to_preset.chroma_intensity);

    result.stiffness = interpolate(from_preset.stiffness, to_preset.stiffness);
    result.damping = interpolate(from_preset.damping, to_preset.damping);
    result.overshoot = interpolate(from_preset.overshoot, to_preset.overshoot);
    result.duration_scale = interpolate(from_preset.duration_scale, to_preset.duration_scale);

    // Boolean fields: snap when past 50%.
    const float progress = eased_progress();
    result.text_distortion_allowed =
        progress < 0.5F ? from_preset.text_distortion_allowed : to_preset.text_distortion_allowed;
    result.safe_text_clamp =
        progress < 0.5F ? from_preset.safe_text_clamp : to_preset.safe_text_clamp;

    return result;
}

auto FxTransitionEngine::apply_easing(float linear_progress, EasingFunction easing_func) -> float
{
    const float val = std::clamp(linear_progress, 0.0F, 1.0F);

    switch (easing_func)
    {
        case EasingFunction::kLinear:
            return val;

        case EasingFunction::kEaseIn:
            return val * val;

        case EasingFunction::kEaseOut:
            return 1.0F - (1.0F - val) * (1.0F - val);

        case EasingFunction::kEaseInOut:
        {
            if (val < 0.5F)
            {
                return 2.0F * val * val;
            }
            return 1.0F - ((-2.0F * val + 2.0F) * (-2.0F * val + 2.0F)) / 2.0F;
        }

        case EasingFunction::kSpring:
        {
            // Overshoot spring approximation.
            constexpr float kOvershoot = 1.70158F;
            constexpr float kOvershootPlus = kOvershoot + 1.0F;
            return kOvershootPlus * val * val * val - kOvershoot * val * val;
        }
    }
    return val;
}

auto FxTransitionEngine::easing_name(EasingFunction easing_func) -> std::string_view
{
    switch (easing_func)
    {
        case EasingFunction::kLinear:
            return "Linear";
        case EasingFunction::kEaseIn:
            return "Ease In";
        case EasingFunction::kEaseOut:
            return "Ease Out";
        case EasingFunction::kEaseInOut:
            return "Ease In-Out";
        case EasingFunction::kSpring:
            return "Spring";
    }
    return "Unknown";
}

auto FxTransitionEngine::set_default_config(const TransitionConfig& config) -> void
{
    default_config_ = config;
}

auto FxTransitionEngine::default_config() const noexcept -> const TransitionConfig&
{
    return default_config_;
}

auto FxTransitionEngine::transitions_completed() const noexcept -> uint32_t
{
    return transitions_completed_;
}

} // namespace markamp::rendering
