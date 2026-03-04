#include "ControlAnimator.h"

#include <algorithm>
#include <cmath>

namespace markamp::ui
{

auto ActiveAnimation::is_complete() const -> bool
{
    return state == AnimationState::kComplete || progress >= 1.0F;
}

auto ActiveAnimation::remaining_ms() const -> int
{
    return total_ms - elapsed_ms;
}

void ControlAnimator::start(const std::string& control_id,
                            MotionTokenId token_id,
                            AnimationFrameCallback callback)
{
    // Cancel any existing animation on this control
    cancel(control_id);

    auto token = reduced_motion_ ? ControlMotionTokens::get(token_id).reduced()
                                 : ControlMotionTokens::scaled(token_id, motion_scale_);

    // If effectively instant, fire callback immediately and mark complete
    if (token.is_instant())
    {
        if (callback)
        {
            callback(control_id, 1.0F);
        }
        return;
    }

    ActiveAnimation anim;
    anim.control_id = control_id;
    anim.token_id = token_id;
    anim.state = AnimationState::kRunning;
    anim.progress = 0.0F;
    anim.elapsed_ms = 0;
    anim.total_ms = token.duration_ms;

    AnimationEntry entry;
    entry.animation = anim;
    entry.callback = std::move(callback);
    animations_.push_back(std::move(entry));
}

void ControlAnimator::cancel(const std::string& control_id)
{
    animations_.erase(std::remove_if(animations_.begin(),
                                     animations_.end(),
                                     [&control_id](const AnimationEntry& entry)
                                     { return entry.animation.control_id == control_id; }),
                      animations_.end());
}

void ControlAnimator::cancel_all()
{
    animations_.clear();
}

auto ControlAnimator::tick(int delta_ms) -> int
{
    for (auto& entry : animations_)
    {
        if (entry.animation.state != AnimationState::kRunning)
        {
            continue;
        }

        entry.animation.elapsed_ms += delta_ms;
        if (entry.animation.total_ms > 0)
        {
            entry.animation.progress = std::min(1.0F,
                                                static_cast<float>(entry.animation.elapsed_ms) /
                                                    static_cast<float>(entry.animation.total_ms));
        }
        else
        {
            entry.animation.progress = 1.0F;
        }

        if (entry.callback)
        {
            entry.callback(entry.animation.control_id, entry.animation.progress);
        }

        if (entry.animation.progress >= 1.0F)
        {
            entry.animation.state = AnimationState::kComplete;
        }
    }

    // Remove completed animations
    animations_.erase(std::remove_if(animations_.begin(),
                                     animations_.end(),
                                     [](const AnimationEntry& entry) {
                                         return entry.animation.state == AnimationState::kComplete;
                                     }),
                      animations_.end());

    return active_count();
}

auto ControlAnimator::animation_for(const std::string& control_id) const -> const ActiveAnimation*
{
    for (const auto& entry : animations_)
    {
        if (entry.animation.control_id == control_id)
        {
            return &entry.animation;
        }
    }
    return nullptr;
}

auto ControlAnimator::active_count() const -> int
{
    return static_cast<int>(animations_.size());
}

auto ControlAnimator::is_animating(const std::string& control_id) const -> bool
{
    return animation_for(control_id) != nullptr;
}

void ControlAnimator::set_reduced_motion(bool enabled)
{
    reduced_motion_ = enabled;
    if (enabled)
    {
        // Complete all active animations instantly
        for (auto& entry : animations_)
        {
            if (entry.callback)
            {
                entry.callback(entry.animation.control_id, 1.0F);
            }
        }
        animations_.clear();
    }
}

auto ControlAnimator::is_reduced_motion() const -> bool
{
    return reduced_motion_;
}

void ControlAnimator::set_motion_scale(float scale)
{
    motion_scale_ = std::max(0.0F, scale);
}

auto ControlAnimator::motion_scale() const -> float
{
    return motion_scale_;
}

} // namespace markamp::ui
