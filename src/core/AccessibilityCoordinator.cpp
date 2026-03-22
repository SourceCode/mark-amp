#include "AccessibilityCoordinator.h"

namespace markamp::core
{

auto AccessibilityCoordinator::min_contrast_ratio(ContrastLevel level) const -> float
{
    switch (level)
    {
        case ContrastLevel::kAA:
            return 4.5F;
        case ContrastLevel::kAAA:
            return 7.0F;
        case ContrastLevel::kAALarge:
            return 3.0F;
    }
    return 4.5F;
}

} // namespace markamp::core
