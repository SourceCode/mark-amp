#pragma once

#include <cmath>

namespace markamp::ui::animation
{

/// Supported easing curve types mapping a normalized time `t` [0,1] to an output progress.
enum class EasingType
{
    Linear,
    EaseInQuad,
    EaseOutQuad,
    EaseInOutQuad,
    EaseInCubic,
    EaseOutCubic,
    EaseInOutCubic,
    EaseInExpo,
    EaseOutExpo,
    EaseInOutExpo,
    EaseInElastic,
    EaseOutElastic,
    EaseInOutElastic,
    EaseOutBack,
    EaseOutBounce
};

/// Library of standard easing functions for smooth UI transitions.
class Easing
{
public:
    /// Calculates the eased progress given a normalized time `t` in [0.0, 1.0].
    [[nodiscard]] static auto calculate(float t, EasingType type) -> float;

private:
    static constexpr float PI = 3.14159265358979323846f;
};

} // namespace markamp::ui::animation
