#pragma once

/**
 * @file ControlAnimator.h
 * @brief Phase 31 Task 2: Apply motion tokens to controls.
 *
 * Applies ControlMotionTokens to wxWidgets controls for consistent
 * hover, press, reveal, and dismiss animations. Automatically respects
 * reduced-motion settings.
 */

#include "ControlMotionTokens.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::ui
{

/// Animation state for a single control.
enum class AnimationState : uint8_t
{
    kIdle,
    kRunning,
    kPaused,
    kComplete,
};

/// Active animation instance.
struct ActiveAnimation
{
    std::string control_id;
    MotionTokenId token_id{MotionTokenId::kHoverReveal};
    AnimationState state{AnimationState::kIdle};
    float progress{0.0F}; ///< 0.0 to 1.0
    int elapsed_ms{0};
    int total_ms{200};

    /// Check if animation is complete.
    [[nodiscard]] auto is_complete() const -> bool;

    /// Get remaining time in ms.
    [[nodiscard]] auto remaining_ms() const -> int;
};

/// Callback invoked on animation frame.
using AnimationFrameCallback = std::function<void(const std::string& control_id, float progress)>;

/**
 * @brief Applies motion tokens to controls with animation state tracking.
 *
 * Manages active animations, respects reduced-motion settings, and
 * uses FxMotionPreset intensity as a global scale factor.
 */
class ControlAnimator
{
public:
    ControlAnimator() = default;

    /// Start an animation on a control.
    void start(const std::string& control_id,
               MotionTokenId token_id,
               AnimationFrameCallback callback = nullptr);

    /// Cancel an animation on a control.
    void cancel(const std::string& control_id);

    /// Cancel all active animations.
    void cancel_all();

    /// Advance all animations by delta_ms. Returns number of active animations.
    auto tick(int delta_ms) -> int;

    /// Get the current animation for a control (nullptr if none).
    [[nodiscard]] auto animation_for(const std::string& control_id) const -> const ActiveAnimation*;

    /// Get the number of active animations.
    [[nodiscard]] auto active_count() const -> int;

    /// Check if a control has an active animation.
    [[nodiscard]] auto is_animating(const std::string& control_id) const -> bool;

    /// Enable/disable reduced motion mode.
    void set_reduced_motion(bool enabled);

    /// Check if reduced motion is enabled.
    [[nodiscard]] auto is_reduced_motion() const -> bool;

    /// Set global motion scale (0.0 = off, 1.0 = full, 2.0 = slow).
    void set_motion_scale(float scale);

    /// Get current motion scale.
    [[nodiscard]] auto motion_scale() const -> float;

private:
    struct AnimationEntry
    {
        ActiveAnimation animation;
        AnimationFrameCallback callback;
    };

    std::vector<AnimationEntry> animations_;
    bool reduced_motion_{false};
    float motion_scale_{1.0F};
};

} // namespace markamp::ui
