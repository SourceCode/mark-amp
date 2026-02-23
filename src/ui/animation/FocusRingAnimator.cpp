#include "FocusRingAnimator.h"

namespace markamp::ui::animation
{

FocusRingAnimator::FocusRingAnimator(std::function<void()> redraw_callback)
    : redraw_callback_(std::move(redraw_callback))
{
}

void FocusRingAnimator::set_target(const wxRect& bounds)
{
    is_active_ = true;

    if (current_rect_.IsEmpty())
    {
        // First time focus, just fade in
        current_rect_ = bounds;
        target_rect_ = bounds;

        AnimationConfig config;
        config.duration = std::chrono::milliseconds(150);
        config.easing_type = EasingType::EaseOutQuad;

        timeline_.animate<float>(alpha_,
                                 1.0f,
                                 config,
                                 [this](const float& val)
                                 {
                                     alpha_ = val;
                                     if (redraw_callback_)
                                         redraw_callback_();
                                 });
    }
    else
    {
        // Morph from current to new bounds
        target_rect_ = bounds;

        AnimationConfig morph_config;
        morph_config.duration = std::chrono::milliseconds(200);
        morph_config.easing_type = EasingType::EaseOutCubic;

        timeline_.animate<wxRect>(current_rect_,
                                  target_rect_,
                                  morph_config,
                                  [this](const wxRect& val)
                                  {
                                      current_rect_ = val;
                                      if (redraw_callback_)
                                          redraw_callback_();
                                  });

        // Ensure alpha is fully visible
        if (alpha_ < 1.0f)
        {
            AnimationConfig alpha_config;
            alpha_config.duration = std::chrono::milliseconds(100);
            alpha_config.easing_type = EasingType::Linear;
            timeline_.animate<float>(
                alpha_, 1.0f, alpha_config, [this](const float& val) { alpha_ = val; });
        }
    }
}

void FocusRingAnimator::hide()
{
    if (!is_active_)
        return;

    AnimationConfig config;
    config.duration = std::chrono::milliseconds(150);
    config.easing_type = EasingType::EaseOutQuad;

    timeline_.animate<float>(
        alpha_,
        0.0f,
        config,
        [this](const float& val)
        {
            alpha_ = val;
            if (redraw_callback_)
                redraw_callback_();
        },
        [this]()
        {
            is_active_ = false;
            current_rect_ = wxRect(); // Reset when hidden fully
        });
}

auto FocusRingAnimator::get_current_rect() const -> wxRect
{
    return current_rect_;
}

auto FocusRingAnimator::get_alpha() const -> float
{
    return alpha_;
}

auto FocusRingAnimator::is_active() const -> bool
{
    return is_active_;
}

} // namespace markamp::ui::animation
