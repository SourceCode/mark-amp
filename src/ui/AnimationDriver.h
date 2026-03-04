#pragma once

#include <chrono>
#include <functional>
#include <vector>

namespace markamp::ui
{

/// Easing function type: maps [0, 1] → [0, 1].
using EasingFunction = std::function<double(double)>;

/// Predefined easing functions.
namespace easing
{
inline double linear(double progress)
{
    return progress;
}
inline double ease_in_quad(double progress)
{
    return progress * progress;
}
inline double ease_out_quad(double progress)
{
    return progress * (2.0 - progress);
}
inline double ease_in_out_quad(double progress)
{
    return progress < 0.5 ? 2.0 * progress * progress : -1.0 + (4.0 - 2.0 * progress) * progress;
}
inline double ease_out_cubic(double progress)
{
    const double inv = progress - 1.0;
    return inv * inv * inv + 1.0;
}
inline double ease_in_out_cubic(double progress)
{
    return progress < 0.5
               ? 4.0 * progress * progress * progress
               : (progress - 1.0) * (2.0 * progress - 2.0) * (2.0 * progress - 2.0) + 1.0;
}
} // namespace easing

/// Phase 41: A single running animation.
struct Animation
{
    int animation_id{0};
    double from{0.0};
    double to{1.0};
    double current{0.0};
    double duration_ms{300.0};
    double elapsed_ms{0.0};
    EasingFunction easing_fn{easing::ease_out_quad};

    using UpdateCallback = std::function<void(double)>;
    using CompleteCallback = std::function<void()>;
    UpdateCallback on_update;
    CompleteCallback on_complete;

    [[nodiscard]] auto is_complete() const -> bool
    {
        return elapsed_ms >= duration_ms;
    }
    [[nodiscard]] auto progress() const -> double
    {
        return duration_ms > 0.0 ? std::min(elapsed_ms / duration_ms, 1.0) : 1.0;
    }
};

/// Phase 41: Shared animation driver for frame-based interpolation.
class AnimationDriver
{
public:
    /// Start a new animation, returns its ID.
    auto start(double from,
               double target_val,
               double duration_ms,
               Animation::UpdateCallback on_update,
               Animation::CompleteCallback on_complete = nullptr,
               EasingFunction easing_fn = easing::ease_out_quad) -> int;

    /// Advance all animations by delta_ms. Call from a timer.
    void tick(double delta_ms);

    /// Cancel a running animation.
    void cancel(int animation_id);

    /// Cancel all animations.
    void cancel_all();

    /// Number of active animations.
    [[nodiscard]] auto active_count() const -> int
    {
        return static_cast<int>(animations_.size());
    }

    /// Are there any running animations?
    [[nodiscard]] auto has_active() const -> bool
    {
        return !animations_.empty();
    }

private:
    std::vector<Animation> animations_;
    int next_id_{1};
};

} // namespace markamp::ui
