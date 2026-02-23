#include "EasingFunctions.h"

#include <algorithm>

namespace markamp::ui::animation
{

auto Easing::calculate(float t, EasingType type) -> float
{
    // Clamp t to [0, 1] for safety
    t = std::clamp(t, 0.0f, 1.0f);

    switch (type)
    {
        case EasingType::Linear:
            return t;

        case EasingType::EaseInQuad:
            return t * t;
        case EasingType::EaseOutQuad:
            return t * (2.0f - t);
        case EasingType::EaseInOutQuad:
            return (t < 0.5f) ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t;

        case EasingType::EaseInCubic:
            return t * t * t;
        case EasingType::EaseOutCubic:
            t -= 1.0f;
            return t * t * t + 1.0f;
        case EasingType::EaseInOutCubic:
            return (t < 0.5f) ? 4.0f * t * t * t
                              : (t - 1.0f) * (2.0f * t - 2.0f) * (2.0f * t - 2.0f) + 1.0f;

        case EasingType::EaseInExpo:
            return (t == 0.0f) ? 0.0f : std::pow(2.0f, 10.0f * (t - 1.0f));
        case EasingType::EaseOutExpo:
            return (t == 1.0f) ? 1.0f : 1.0f - std::pow(2.0f, -10.0f * t);
        case EasingType::EaseInOutExpo:
            if (t == 0.0f || t == 1.0f)
                return t;
            if (t < 0.5f)
                return 0.5f * std::pow(2.0f, 20.0f * t - 10.0f);
            return 1.0f - 0.5f * std::pow(2.0f, -20.0f * t + 10.0f);

        case EasingType::EaseInElastic:
        {
            if (t == 0.0f || t == 1.0f)
                return t;
            return -std::pow(2.0f, 10.0f * (t - 1.0f)) * std::sin((t - 1.1f) * 5.0f * PI);
        }
        case EasingType::EaseOutElastic:
        {
            if (t == 0.0f || t == 1.0f)
                return t;
            return std::pow(2.0f, -10.0f * t) * std::sin((t - 0.1f) * 5.0f * PI) + 1.0f;
        }
        case EasingType::EaseInOutElastic:
        {
            if (t == 0.0f || t == 1.0f)
                return t;
            t *= 2.0f;
            if (t < 1.0f)
            {
                return -0.5f * std::pow(2.0f, 10.0f * (t - 1.0f)) *
                       std::sin((t - 1.1f) * 5.0f * PI);
            }
            return 0.5f * std::pow(2.0f, -10.0f * (t - 1.0f)) * std::sin((t - 1.1f) * 5.0f * PI) +
                   1.0f;
        }

        case EasingType::EaseOutBack:
        {
            const float c1 = 1.70158f;
            const float c3 = c1 + 1.0f;
            t -= 1.0f;
            return 1.0f + c3 * t * t * t + c1 * t * t;
        }

        case EasingType::EaseOutBounce:
        {
            const float n1 = 7.5625f;
            const float d1 = 2.75f;
            if (t < 1.0f / d1)
            {
                return n1 * t * t;
            }
            else if (t < 2.0f / d1)
            {
                t -= 1.5f / d1;
                return n1 * t * t + 0.75f;
            }
            else if (t < 2.5f / d1)
            {
                t -= 2.25f / d1;
                return n1 * t * t + 0.9375f;
            }
            else
            {
                t -= 2.625f / d1;
                return n1 * t * t + 0.984375f;
            }
        }

        default:
            return t;
    }
}

} // namespace markamp::ui::animation
