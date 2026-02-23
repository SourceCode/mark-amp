#pragma once

#include "AnimationTimeline.h"

#include <wx/gdicmn.h>

#include <functional>

namespace markamp::ui::animation
{

/// Manages the state and transition logic for an animated focus ring.
/// Slowly morphs bounds and fades alpha between active targets.
class FocusRingAnimator
{
public:
    explicit FocusRingAnimator(std::function<void()> redraw_callback);

    void set_target(const wxRect& current_bounds);
    void hide();

    [[nodiscard]] auto get_current_rect() const -> wxRect;
    [[nodiscard]] auto get_alpha() const -> float;
    [[nodiscard]] auto is_active() const -> bool;

private:
    std::function<void()> redraw_callback_;
    AnimationTimeline timeline_;

    wxRect current_rect_{0, 0, 0, 0};
    wxRect target_rect_{0, 0, 0, 0};

    float alpha_{0.0f};
    bool is_active_{false};
};

} // namespace markamp::ui::animation
