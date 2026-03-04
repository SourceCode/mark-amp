#include "ScrollPhysicsEngine.h"

#include <algorithm>
#include <cmath>

namespace markamp::ui
{

auto ScrollPhysicsModel::tick(double dt_ms) -> bool
{
    if (!animating_)
        return false;

    const double dt_sec = dt_ms / 1000.0;

    switch (mode_)
    {
        case ScrollMode::kImmediate:
            position_ = target_;
            animating_ = false;
            break;

        case ScrollMode::kSmooth:
        {
            // Ease-out interpolation toward target
            const double diff = target_ - position_;
            if (std::abs(diff) < kMinVelocity)
            {
                position_ = target_;
                animating_ = false;
            }
            else
            {
                // Exponential ease-out
                position_ += diff * (1.0 - std::pow(0.001, dt_sec));
            }
            break;
        }

        case ScrollMode::kMomentum:
        {
            // Apply friction to velocity
            velocity_ *= std::pow(friction_, dt_sec * 60.0); // normalize to 60fps

            if (std::abs(velocity_) < kMinVelocity)
            {
                velocity_ = 0.0;
                if (!is_overscrolling())
                {
                    animating_ = false;
                }
            }

            position_ += velocity_ * dt_sec;

            // Overscroll bounce-back
            if (is_overscrolling())
            {
                if (bounce_enabled_)
                {
                    apply_spring(dt_sec);
                }
                else
                {
                    apply_bounds();
                    velocity_ = 0.0;
                    animating_ = false;
                }
            }
            break;
        }
    }

    return animating_;
}

void ScrollPhysicsModel::scroll_to(double target, ScrollMode mode)
{
    target_ = std::clamp(target, 0.0, max_scroll());
    mode_ = mode;

    if (mode == ScrollMode::kImmediate)
    {
        position_ = target_;
        animating_ = false;
    }
    else
    {
        animating_ = true;
    }
    velocity_ = 0.0;
}

void ScrollPhysicsModel::fling(double velocity_px_per_sec)
{
    velocity_ = velocity_px_per_sec;
    mode_ = ScrollMode::kMomentum;
    animating_ = true;
}

void ScrollPhysicsModel::stop()
{
    velocity_ = 0.0;
    animating_ = false;
    apply_bounds();
}

auto ScrollPhysicsModel::position() const -> double
{
    return position_;
}

void ScrollPhysicsModel::set_position(double pos)
{
    position_ = pos;
    velocity_ = 0.0;
    animating_ = false;
}

void ScrollPhysicsModel::set_content_range(double visible, double content)
{
    visible_height_ = visible;
    content_height_ = content;
    apply_bounds();
}

auto ScrollPhysicsModel::max_scroll() const -> double
{
    return std::max(0.0, content_height_ - visible_height_);
}

auto ScrollPhysicsModel::visible_height() const -> double
{
    return visible_height_;
}
auto ScrollPhysicsModel::content_height() const -> double
{
    return content_height_;
}
auto ScrollPhysicsModel::is_animating() const -> bool
{
    return animating_;
}

auto ScrollPhysicsModel::is_overscrolling() const -> bool
{
    return position_ < 0.0 || position_ > max_scroll();
}

auto ScrollPhysicsModel::overscroll_distance() const -> double
{
    if (position_ < 0.0)
        return position_;
    if (position_ > max_scroll())
        return position_ - max_scroll();
    return 0.0;
}

void ScrollPhysicsModel::set_friction(double friction)
{
    friction_ = std::clamp(friction, 0.8, 0.999);
}
auto ScrollPhysicsModel::friction() const -> double
{
    return friction_;
}
void ScrollPhysicsModel::set_bounce_enabled(bool enabled)
{
    bounce_enabled_ = enabled;
}
auto ScrollPhysicsModel::bounce_enabled() const -> bool
{
    return bounce_enabled_;
}
void ScrollPhysicsModel::set_spring_stiffness(double stiffness)
{
    spring_stiffness_ = stiffness;
}
auto ScrollPhysicsModel::spring_stiffness() const -> double
{
    return spring_stiffness_;
}

void ScrollPhysicsModel::apply_bounds()
{
    position_ = std::clamp(position_, 0.0, max_scroll());
}

void ScrollPhysicsModel::apply_spring(double dt_sec)
{
    // Hooke's law: F = -k * x
    const double overshoot = overscroll_distance();
    const double spring_force = -spring_stiffness_ * overshoot;
    velocity_ += spring_force * dt_sec;

    // Damping
    velocity_ *= 0.9;

    if (std::abs(overshoot) < kMinVelocity && std::abs(velocity_) < kMinVelocity)
    {
        apply_bounds();
        velocity_ = 0.0;
        animating_ = false;
    }
}

} // namespace markamp::ui
