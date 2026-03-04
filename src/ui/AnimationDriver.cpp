#include "AnimationDriver.h"

#include <algorithm>

namespace markamp::ui
{

auto AnimationDriver::start(double from,
                            double target_val,
                            double duration_ms,
                            Animation::UpdateCallback on_update,
                            Animation::CompleteCallback on_complete,
                            EasingFunction easing_fn) -> int
{
    Animation anim;
    anim.animation_id = next_id_++;
    anim.from = from;
    anim.to = target_val;
    anim.current = from;
    anim.duration_ms = duration_ms;
    anim.elapsed_ms = 0.0;
    anim.easing_fn = std::move(easing_fn);
    anim.on_update = std::move(on_update);
    anim.on_complete = std::move(on_complete);

    animations_.push_back(std::move(anim));
    return animations_.back().animation_id;
}

void AnimationDriver::tick(double delta_ms)
{
    for (auto& anim : animations_)
    {
        anim.elapsed_ms += delta_ms;
        const double raw_progress = anim.progress();
        const double eased = anim.easing_fn ? anim.easing_fn(raw_progress) : raw_progress;
        anim.current = anim.from + (anim.to - anim.from) * eased;

        if (anim.on_update)
        {
            anim.on_update(anim.current);
        }
    }

    // Complete finished animations
    for (auto& anim : animations_)
    {
        if (anim.is_complete() && anim.on_complete)
        {
            anim.on_complete();
        }
    }

    // Remove completed
    std::erase_if(animations_, [](const Animation& anim) { return anim.is_complete(); });
}

void AnimationDriver::cancel(int animation_id)
{
    std::erase_if(animations_,
                  [animation_id](const Animation& anim)
                  { return anim.animation_id == animation_id; });
}

void AnimationDriver::cancel_all()
{
    animations_.clear();
}

} // namespace markamp::ui
