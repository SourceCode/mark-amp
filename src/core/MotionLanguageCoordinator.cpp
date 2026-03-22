#include "MotionLanguageCoordinator.h"

namespace markamp::core
{

auto MotionLanguageCoordinator::timing_for_intent(MotionIntent intent) const -> MotionTiming
{
    MotionTiming timing;

    switch (intent)
    {
        case MotionIntent::kInstant:
            timing.duration_ms = 0;
            timing.easing = "linear";
            break;
        case MotionIntent::kMicro:
            timing.duration_ms = 80;
            timing.easing = "cubic-bezier(0.2, 0, 0, 1)";
            break;
        case MotionIntent::kShort:
            timing.duration_ms = 150;
            timing.easing = "cubic-bezier(0.2, 0, 0, 1)";
            break;
        case MotionIntent::kMedium:
            timing.duration_ms = 250;
            timing.easing = "cubic-bezier(0.2, 0, 0, 1)";
            break;
        case MotionIntent::kLong:
            timing.duration_ms = 400;
            timing.easing = "cubic-bezier(0.4, 0, 0.2, 1)";
            break;
        case MotionIntent::kChoreography:
            timing.duration_ms = 500;
            timing.easing = "cubic-bezier(0.4, 0, 0.2, 1)";
            break;
    }

    return timing;
}

} // namespace markamp::core
