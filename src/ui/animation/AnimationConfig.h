#pragma once
#include "EasingFunctions.h"

#include <chrono>

namespace markamp::ui::animation
{

struct AnimationConfig
{
    std::chrono::milliseconds duration{300};
    std::chrono::milliseconds delay{0};
    EasingType easing_type{EasingType::EaseOutQuad};
    int repeat_count{0}; // 0 = play once. -1 = infinite.
    bool auto_reverse{false};
};

} // namespace markamp::ui::animation
